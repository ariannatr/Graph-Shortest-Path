#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <pthread.h>
#include "job_scheduler.h"

#include <iostream>
#include <stdint.h>

#include "node_index.h"
#include "buffer.h"
#include "scc.h"
#include "queue.h"
#include "other_functions.h"
#include "cc_index.h"
#include "graph.h"
using namespace std;
#define RESULTS 60000
#define QUERIES 20

void  *thread_f( void * argp )     /* Thread function */
{ 
    JobScheduler* sch=(JobScheduler*)argp;
    Arguments_Needed args=sch->get_args();
    int * marked_table_in=NULL;
    int * marked_table_out=NULL;
    int *markedflag=(int*)malloc(sizeof(int));
    *markedflag=1;
    Queue <uint32_t> queue_out;
    Queue <uint32_t> queue_in;
    int depth,found,i;
    marked_table_in=(int*)calloc(args.mytable_in->get_size(),sizeof(int));
    marked_table_out=(int*)calloc(args.mytable_out->get_size(),sizeof(int)); 
    int what,k;
    Queue <Job> myjobs;   
    Job j;
    while(1)
    {
        sch->lock_data();  
        while(sch->get_num_of_queries()==0 )    //while no queries are left in the queue_job
        {
            if(sch->get_terminate_threads()==1) //no other queries to be performed
            {
                free(marked_table_in);
                free(marked_table_out);
                free(markedflag);
                sch->unlock_data();     //unlock mtx_for_data for other threads to be  able to free their sources
                pthread_exit(NULL); 
            }
            sch->wait_execute();            //wait on cond_execute thta is being broadcasted when 'F' is  found 
        }
        for(k=0;k<QUERIES;k++)
        {
            j=sch->get_front_job();   //get first job from the queue
            if(j.id==-1)      // queue is empty
            {   
                myjobs.push(j);
                k++;
                break;
            } 
            sch->decrease_numof_queries(1);
            myjobs.push(j);
        }   
        sch->unlock_data();
     

        for(i=0;i<k;i++)
        {
            j=myjobs.front();   //take first in the local queue
            myjobs.pop();
            if(j.id==-1)      //queue is empty
            {
                break;           
            }
            if(args.graphtype==2)
            {
                depth=0;
                found=0;
                (*markedflag)=(*markedflag)+1;
                queue_out.clear();
                queue_in.clear();
                if((args.Scc)->get_component(j.node1)==(args.Scc)->get_component(j.node2))
                        found=the_bbfs(j.node1,j.node2,args.Scc,args.mytable_in,args.mytable_out,args.index,args.ingoing_index,&depth,marked_table_out,marked_table_in,*markedflag,&queue_out,&queue_in,0 );
                else
                {
                    if(grail_question(args.Scc,j.node1,j.node2) >0)
                        found=the_bbfs(j.node1,j.node2,args.Scc,args.mytable_in,args.mytable_out,args.index,args.ingoing_index,&depth,marked_table_out,marked_table_in,*markedflag,&queue_out,&queue_in ,0);
                    else
                        found=-1;   
                }

                if(found==1)
                    sch->submit_result(j.id,depth);
                else
                    sch->submit_result(j.id,-1);             
            }
            else if(args.graphtype==1)
            {
                depth=0;
                found=0;
                (*markedflag)=*markedflag+1;
                what=args.cc_index->exists_path(j.node1,j.node2);
                if(what==0)
                    sch->submit_result(j.id,-1);
                else
                {
                    queue_out.clear();
                    queue_in.clear();
                    found=the_bbfs(j.node1,j.node2,NULL,args.mytable_in,args.mytable_out,args.index,args.ingoing_index,&depth,marked_table_out,marked_table_in,*markedflag,&queue_out,&queue_in,j.version);
                    if(found==1)
                        sch->submit_result(j.id,depth);
                    else
                    {
                        sch->submit_result(j.id,-1);
                    }
                }
            }
        }
        sch->decrease_numof_results(i);   //decrease num of results calculated +signal master thread to continue if necessary
    }
} 




JobScheduler::JobScheduler(int ex,Graph* G)
{
    execution_threads=ex;
    pthread_mutex_init (& mtx_for_jobs , NULL);
    pthread_mutex_init (& mtx_for_data , NULL);
    pthread_cond_init (& cond_start , NULL);
    pthread_cond_init (& cond_execute , NULL);
    results=(int *) calloc(RESULTS,sizeof(int));
    results_size=RESULTS;
    numof_results=0;
    numof_queries=0;
    priority=0;
    terminate_threads=0;
    ready=0;
    args.mytable_in=G->get_table_in();
    args.mytable_out=G->get_table_out();
    args.index=G->get_buffer_out();
    args.ingoing_index=G->get_buffer_in();  
}


void  JobScheduler::execute_all_jobs(int num)
{
    if(queue_job.empty() && num==0)
    {
        terminate_threads=1;   //set the terminating value 
        numof_results=0;
        numof_queries=0;
       	pthread_cond_broadcast(&cond_execute);    //wake all threads to end
        return;
    }
    numof_results=num;
    numof_queries=num;
    if(this->priority>this->results_size)   // if we need to realloc result array
    {
        this->results=(int*)realloc(results,this->priority*sizeof(int));
        this->results_size=this->priority;
    }    
    pthread_cond_broadcast(& cond_execute );// threads have work to do,wake up one and rest will follow
}

void JobScheduler::wait_all_tasks_finish()
{
    pthread_cond_wait(&cond_start,&mtx_for_jobs);
}


int JobScheduler::get_queue_size()
{
    pthread_mutex_lock(&mtx_for_jobs) ;    
    if(queue_job.empty())
    {
        pthread_mutex_unlock(&mtx_for_jobs) ;
        return 0;
    } 
    else
    {    
        pthread_mutex_unlock(&mtx_for_jobs) ;
        return 1;   
    }
}

void JobScheduler::signal_master_continue()
{
    pthread_cond_signal(&cond_start);  
}

void JobScheduler::wait_execute()
{
    pthread_cond_wait(&cond_execute,&mtx_for_data);
}

void JobScheduler::decrease_numof_results(int num)
{
    pthread_mutex_lock(&mtx_for_jobs);
    numof_results=numof_results-num;
    if(numof_results==0 )
    {
        pthread_mutex_unlock(&mtx_for_jobs);  
        this->signal_master_continue();  
    }
    else
        pthread_mutex_unlock(&mtx_for_jobs);
    
}

void JobScheduler::decrease_numof_queries(int num)
{
    numof_queries=numof_queries-num;
}


Job JobScheduler::get_front_job( ) 
{   
    Job j;
    if(queue_job.empty())      //need to return -1 to thread_f in order to inform that the queue ended
    {
          j.id=-1;
        return j;
    }
    j=queue_job.front();
    queue_job.pop();
    return j;
}

int JobScheduler::get_num_of_results()
{
    return numof_results;
}
int JobScheduler::get_num_of_queries()
{

    int ret=numof_queries;
    return ret;
}

Arguments_Needed JobScheduler::get_args()
{
    return args;
}


JobScheduler::~JobScheduler()
{
    int status;
    for(int i=0;i<execution_threads;i++)
    {           
        if (  pthread_join( *(tids+i) , ( void **) & status ) !=0) 
        {  
            perror (" pthread_join "); 
            exit (1) ;
        }
    }
    cerr<<"Joined pthreads\n";
    pthread_cond_destroy(& cond_start ) ;
    pthread_cond_destroy(& cond_execute );
    pthread_mutex_destroy(& mtx_for_jobs );   
    pthread_mutex_destroy(& mtx_for_data );
    
    free(tids);
    free(results);
    cerr<<"Jsc destructor\n";
}

int JobScheduler::get_priority()
{
    return priority;
}

void JobScheduler::submit_job(Job j)          //every query from file entering queue_job
{
    j.id=priority;
    priority++;
    queue_job.push(j);
}



int JobScheduler::get_terminate_threads()
{
   return terminate_threads;;
}


void JobScheduler::submit_result(int pr,int result)
{
    this->results[pr]=result;
}

void JobScheduler::pass_rest_arguments(int type,Stronglycc* scc,CC_Index* cc_dynamic)
{
    args.Scc=scc;
    args.graphtype=type;
    args.cc_index=cc_dynamic;
    tids=(pthread_t *) malloc(sizeof(pthread_t)*execution_threads);

    for(int i=0;i<execution_threads;i++)
    {
        if ( pthread_create(&tids[i] , NULL , thread_f ,(void*)this) !=0)  /* New thread */
        {
            cerr<<"error in threads\n";
            exit(1);
        }
    }
    cerr<<"Created pthreads\n";

    return ;
}

void JobScheduler::print_results()
{
    int i;
    for(i=0;i<this->priority;i++)
    {
        printf("%d\n",results[i]);
    }
}

void JobScheduler::lock_data()
{
    pthread_mutex_lock(&mtx_for_data);
}

void JobScheduler::unlock_data()
{
    pthread_mutex_unlock(&mtx_for_data);
}