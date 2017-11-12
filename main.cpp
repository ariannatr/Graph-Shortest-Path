#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "graph.h"
#include "other_functions.h"
#include "cc_index.h"
#include "scc.h"
#include "job_scheduler.h"
#include <stdint.h>
using namespace std;


int main(int argc, char* argv[])
{
	if(argc>3)
		cout<<"Invalid Argument's Input \n";
	else if(argc==3)
	{
		FILE * file;
		file=fopen(argv[1],"r");
		if(file==NULL)
		{
			cout<<"Couldn't open the file \n";
			return -1;
		}
		Graph G;
		Stronglycc* Scc=NULL;
		CC_Index* cc_dynamic=NULL;
		int * marked_table_in=NULL;
		int * marked_table_out=NULL;
        int *markedflag=(int*)malloc(sizeof(int));
		*markedflag=1;
		char * tmp=new char[100];
		string line;
		uint32_t node1,node2;
		int mytable_in_size=0;
		int mytable_out_size=0;
		float queries2=0;
		int queries=0;
		float metric;
		Job temp_Job;
		uint32_t version=0;
		char prev_char='A';
		while(!feof(file))		//start reading file
		{
			if(fscanf(file,"%u %u",&node1,&node2)!=2)
			{
				cerr<<"End of Graph\n";	
				break;
			}
			else
			{	
				G.insert_nodes(node1,node2);
				G.add_edge(node1,node2,0) ;
			}
		}
		fclose(file);

		JobScheduler Sch(8,&G);    //2 einai t threads p dhmioyrgw kai pernaw ta arguments g ts synarthseis
		FILE * file2;
		file2=fopen(argv[2],"r");
		if(file2==NULL)
		{
			cout<<"Couldn't open the file2 \n";
			return -1;
		}
		char type[20];
		if ( fscanf(file2,"%s",type) <0)
			cerr<<"erron on fscanf\n";
		if(strcmp(type,"DYNAMIC")==0)
		{
			G.set_graphtype(1); 
			mytable_in_size=(G.get_table_out())->get_size();
			mytable_out_size=(G.get_table_in())->get_size();
			marked_table_in=(int*)calloc(mytable_in_size,sizeof(int));
			marked_table_out=(int*)calloc(mytable_out_size,sizeof(int)); 
			
			if(mytable_in_size>mytable_out_size)
				cc_dynamic=new CC_Index(mytable_in_size);
			else
				cc_dynamic=new CC_Index(mytable_out_size);
			cc_dynamic->cc_create(&G,marked_table_out,marked_table_in);
	
			cc_dynamic->create_update_index();
		}
		else if(strcmp(type,"STATIC")==0)
		{
			G.set_graphtype(2); 
			Scc=new Stronglycc((G.get_table_out())->get_size());
			tarjan(G.get_table_in(),G.get_table_out(),G.get_buffer_out(),Scc);
			cerr<<"Tarjan is done \n";
			fix_scc_neighbors_and_grail(Scc,G.get_table_out(),G.get_buffer_out());
			cerr<<"Grail is done\n";
			
		}
		Sch.pass_rest_arguments(G.get_graphtype(),Scc,cc_dynamic);      //gt prin ta Scc kai cc-dynamic htn null

		while(!feof(file2))		//start reading file
		{
			if (fscanf(file2,"%s %u %u",tmp,&node1,&node2) <0)
				cerr<<"error on fscanf\n";
			if(tmp[0]=='Q')
			{
				prev_char='Q';
				temp_Job.node1=node1;
				temp_Job.node2=node2;
				temp_Job.version=version;
				queries++;
				queries2++;
				Sch.submit_job(temp_Job);
			}
			else if(tmp[0]=='F')
			{
				Sch.execute_all_jobs(queries);
				Sch.wait_all_tasks_finish();

				queries=0;
				if(G.get_graphtype()==1)
				{
					metric=(float)cc_dynamic->get_use()/queries2;
					if(metric>0.01)//0,01
					{
						delete cc_dynamic;
						if(mytable_in_size>mytable_out_size)
							cc_dynamic=new CC_Index(mytable_in_size);
						else
							cc_dynamic=new CC_Index(mytable_out_size);
						cc_dynamic->cc_create(&G,marked_table_out,marked_table_in);
						cc_dynamic->create_update_index();
						queries2=0;
					}
				}
			}
			else if(tmp[0]=='A')
			{
				if(prev_char=='Q')
					version++;
				prev_char='A';
				G.insert_nodes(node1,node2);
				G.add_edge(node1,node2,version);

				if(G.get_graphtype()==1)
						cc_dynamic->update_cc_indexes(node1,node2);
			}
		}

		Sch.execute_all_jobs(0);     //g na kanei to has to end kai na termatisei ta threads
		cerr<<"Has to print results\n";
		Sch.print_results();
		

		delete [] tmp;
		if(G.get_graphtype()==2)
			delete Scc;
		else
			delete cc_dynamic;
		free(marked_table_in);
		free(marked_table_out);
		free(markedflag);
		fclose(file2);	
	}
	else
	{
		cout<<"Please give an input file \n";
		return -1;
	}
	return 0;
}