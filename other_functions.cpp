#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "other_functions.h"
#include "node_index.h"
#include "scc.h"
#include "buffer.h"
#include "stack.h"
#include "queue.h"
#include "info_for_scc_build.h"
#include "cc_index.h"
#include "graph.h"
using namespace std;


void tarjan(Node_Index* table_in,Node_Index* table_out,Buffer* buffer_out,Stronglycc* Scc)
{
	Stack S;
	int index=1;
	int size;
	Component_Index* comp_table=new Component_Index(table_out->get_size());
	if(table_out->get_size()<=table_in->get_size())
		size=table_out->get_size();
	else
		size=table_in->get_size();
	
	for(uint32_t v=0;v<(uint32_t)size;v++)
	{
		if( ( (table_in->exists_node(v)==0)||(table_out->exists_node(v)==0)) && (comp_table->get_index(v) ==0 ))
		{
			iterative_strongconnect(v,table_out,buffer_out,Scc,&S,&index,comp_table) ;     //run strongconnect function
		}	
		
	}
	cerr<<"Found scc:"<<Scc->get_count()<<endl;
	
	//Scc->print_all();				//put in comment when running large files
	delete comp_table;
}

void iterative_strongconnect(uint32_t v,Node_Index * table_out,Buffer* buffer_out,Stronglycc* Scc,Stack* S,int * index,Component_Index* comp_table)
{
	List_node* temp_out=NULL;
	int where,adjacent_nodes;
	uint32_t newlast,top,neighbor,num;
	comp_table->set_index(v,(*index));
	comp_table->set_lowlink(v,(*index));
	(*index)++;
	comp_table->set_vindex(v,0);
	comp_table->set_caller(v,UINT32_MAX);
	comp_table->set_onStack(v,true);
	S->push(v);
	uint32_t last=v;
	while(1)
	{
		where=table_out->get_offset(last);    //get where in buffer i can find the list head of last node
		if(where<0)
			temp_out=NULL;
		else
			temp_out=buffer_out->getListHead(where);	
		adjacent_nodes=table_out->get_neighbors(last);			//how many neighbors has last-node

		if( comp_table->get_vindex(last) < adjacent_nodes  )		//if i haven't checked all neighbors
		{
			neighbor=temp_out->get_neighbor_from_start(comp_table->get_vindex(last));	    //get the node-id of the "last"'s neighbor that i haven't checked yet
			comp_table->set_vindex(last,comp_table->get_vindex(last)+1);              //last->vindex++;	
			if(comp_table->get_index(neighbor) ==0)
			{
				comp_table->set_caller(neighbor,last);
				comp_table->set_vindex(neighbor,0);
				comp_table->set_index(neighbor,(*index));
				comp_table->set_lowlink(neighbor,(*index));
				(*index)++;
				S->push(neighbor);	           //push neighbor on stack
				comp_table->set_onStack(neighbor,true);			
				last=neighbor;	
			}
			else if(comp_table->onStack(neighbor) /*is true*/)
			{
				if(comp_table->get_lowlink((int)last) > comp_table->get_index(neighbor))      //last->lowlink = min(last->lowlink, w->index);
					comp_table->set_lowlink((int)last,comp_table->get_index(neighbor));
			}

		}
		else
		{
			if(comp_table->get_lowlink(last)==comp_table->get_index((int)last))
			{
				num=Scc->get_count();
				Component * new_comp=new Component(num);    //start building new component
					
				do{
					top=S->pop();           			 //pop from stack return which item is popped
					comp_table->set_onStack(top,false);
					Scc->set_component(top,num);    		 //set id_belongs_to_component[]
					new_comp->add(top);						//top belong to new_comp
				}while(top!=last);

                Scc->add(new_comp);         //add new_comp in the array of components of Scc structure
			}

			newlast = comp_table->get_caller(last);   //Go up one recursive call
            if(newlast != UINT32_MAX) {      
                if(comp_table->get_lowlink((int)newlast) > comp_table->get_lowlink((int)last))      // newLast->lowlink = min(newLast->lowlink, last->lowlink);
					comp_table->set_lowlink((int)newlast,comp_table->get_lowlink((int)last));
                last = newlast;
            } 
            else    //We've seen all the nodes
                break;
		}
	}
}


void fix_scc_neighbors_and_grail(Stronglycc* Scc,Node_Index * table_out,Buffer* buffer_out)
{
	Node_Index hypergraph(Scc->get_count());
	Buffer hyper_buffer;
	update_neighbors(Scc,table_out,buffer_out,&hypergraph,&hyper_buffer);       //fill hyper_buffer with hyper edges (from cc to cc)
	cerr<<"Done updating hyper neighbors\n";
	grail_1(Scc,&hypergraph,&hyper_buffer);
	//grail_2(Scc,&hypergraph,&hyper_buffer);
	
}

int grail_question(Stronglycc* Scc,uint32_t node1,uint32_t node2)
{
	Component* component1,*component2;
	component1=Scc->get_comp(Scc->get_component(node1));
	component2=Scc->get_comp(Scc->get_component(node2));
	if(	component2->get_min_rank(1)>=component1->get_min_rank(1) && component2->get_rank(1)<=component1->get_rank(1) )
		return 1;
	else 
		return -1;

}


void update_neighbors(Stronglycc* Scc,Node_Index * table_out,Buffer* buffer_out,Node_Index* hyper_index,Buffer* hyper_buffer)
{
	List_node* temp_out;
	Component * comp;
	int where,nextavailable;
	uint32_t hnode2;

	for(uint32_t k=0;k<Scc->get_count();k++)			//for all components
	{
		comp=Scc->get_comp(k);
		hyper_index->set_offset(k,hyper_buffer->insert_node_in_buffer());
		
		for(uint32_t j=0;j<comp->get_count();j++)		//for all nodes in component
		{
			
			where=table_out->get_offset( comp->get_included_id(j));
			if(where<0)
			{
				temp_out=NULL;
				continue;
			}	
			else
			{
				temp_out=buffer_out->getListHead(where);
				while(temp_out!=NULL)
				{
					for(int i=0;i<temp_out->get_next_free_neighbor();i++)				//for all neighbors of node in component
					{
						if(Scc->get_component(temp_out->get_neighbor(i))!=comp->get_id())
						{

							hnode2=Scc->get_component(temp_out->get_neighbor(i));
							if((hyper_buffer->getListHead(hyper_index->get_last_offset(k)))->add_neighbor(hnode2,0) <0)
							{
								nextavailable=hyper_buffer->getNextAvailable();
								(hyper_buffer->getListHead(hyper_index->get_last_offset(k)))->add_neighbor2(hnode2,hyper_buffer->getListHead(nextavailable),0);		//add neighbor in a new List_node
								hyper_index->set_last_offset(k,nextavailable);
						}			
							hyper_index->set_neighbors(k);	    //need in order to keep track of the sum of neighbors of its node
						}
					}
					temp_out=temp_out->get_next();
				}
			}	
		}

		//now that my job with the neighbor components is done i can delete some memory i dont't need anymore
		comp->free_array_included_ids();
	}
}

			

void grail_1(Stronglycc* Scc,Node_Index* hyper_index,Buffer* hyper_buffer)
{
	uint32_t i;
	uint32_t rank;
	uint32_t * order=(uint32_t*)malloc(sizeof(uint32_t));
	*order=1;
	Component* comp=NULL;
	
	for(i=0;i<Scc->get_count();i++)
	{
		comp=Scc->get_comp(i);
		rank=comp->get_rank(1);     //get rank belonging to the first execution of grail
		if(rank==0)
		{
			dfs(i,Scc,order,hyper_index,hyper_buffer,1);        
		}
	}
	free(order);
}

void grail_2(Stronglycc* Scc,Node_Index* hyper_index,Buffer* hyper_buffer)            //not used
{
	uint32_t i,j;
	uint32_t rank;
	uint32_t * order=(uint32_t*)malloc(sizeof(uint32_t));
	*order=1;
	Component* comp=NULL;
	for(i=0;i<Scc->get_count()/2;i++)
	{
		comp=Scc->get_comp(i);
		rank=comp->get_rank(2);
		if(rank==0)
		{
			dfs(i,Scc,order,hyper_index,hyper_buffer,2);
		}
	}
	//cerr<<"i="<<i<<endl;
	for(j=Scc->get_count()-1;j>=i;j--)
	{
		comp=Scc->get_comp(j);
		rank=comp->get_rank(2);
		if(rank==0)
		{
			dfs(j,Scc,order,hyper_index,hyper_buffer,2);
		}
	}
	//cerr<<"j="<<j<<endl;
	//cerr<<"order :"<<*order<<endl; 
	free(order);
}


void dfs(uint32_t start,Stronglycc* Scc,uint32_t * order,Node_Index* hyper_index,Buffer* hyper_buffer,int which_grail)
{
	Component* s_component=NULL;
	uint32_t num=start;
	Stack* stack;
	stack=new Stack();
	stack->push(num);
	int where;
	uint32_t temp_neighbor;
	Component* temp_component=NULL;
	List_node* temp_out;
	int flag,j;
	uint32_t neighbors_count;
	while(!stack->empty())
	{
		flag=0;
		num=stack->get_front();
		s_component=Scc->get_comp(num);
		neighbors_count=hyper_index->get_neighbors(num);					//how many neighbors has last-node
		if(s_component->get_rank(which_grail)==0 && neighbors_count==0)    		//not visited,has no neighbors
		{
			s_component->set_rank(*order,which_grail);
			s_component->set_min_rank(*order,which_grail);
			stack->pop();
			(*order)++;
			continue;
		}	
		else if(s_component->get_rank(which_grail)==0 && neighbors_count!=0 && s_component->get_visited_count(which_grail)!= neighbors_count)  //not ranked,has more childern to be visited
		{
			where=hyper_index->get_offset(num);
			temp_out=hyper_buffer->getListHead(where);
			while(temp_out!=NULL && flag!=1)              //for all neighbors of node
			{
				for(j=0;j<temp_out->get_next_free_neighbor();j++)
				{												
					temp_neighbor=temp_out->get_neighbor(j);               //which node is the j-th neighbor
					temp_component=Scc->get_comp(temp_neighbor);           //in which scc h-th neighbor belongs to
					if(temp_component->get_rank(which_grail)==0)
					{
						stack->push(temp_neighbor);							
						s_component->visited_count_plus(which_grail);
						flag=1;
						break;                //next go to while(!stack->empty())
					}
					else
					{
						if(temp_component->get_min_rank(which_grail)<s_component->get_min_rank(which_grail))
							s_component->set_min_rank(temp_component->get_min_rank(which_grail),which_grail);
					}
				}
				temp_out=temp_out->get_next();
			}	

			if(flag!=1)      //all neighbors of s-component have rank
			{
				while(s_component->get_visited_count(which_grail)!=neighbors_count)
					s_component->visited_count_plus(which_grail);
			}
			continue;
		}
		else if(s_component->get_rank(which_grail)==0 && neighbors_count!=0 && s_component->get_visited_count(which_grail)== neighbors_count)
		{
			s_component->set_rank(*order,which_grail);
			stack->pop();
			(*order)++;
		}
	}
	delete stack;
}
	
	
int bfs(uint32_t start,Graph* G,int* marked_out,int * marked_in,CC_Index * cc_dynamic,u_int32_t cc_num,int check)
{
	uint32_t node1=start;
	List_node* temp_out=NULL;
	List_node* temp_in=NULL;
	int j,count,where2;
	uint32_t neighbor;
	int size_out,size_in;
	Queue <uint32_t>* queue_out;
	Queue <uint32_t>* queue_in;
	queue_out= new Queue <uint32_t>;
	queue_in= new Queue <uint32_t>;

	Node_Index* table_out=G->get_table_out();
	Node_Index* table_in=G->get_table_in();
	Buffer* buffer_out=G->get_buffer_out();
	Buffer* buffer_in=G->get_buffer_in();

	queue_in->push(node1);
	queue_out->push(node1);
	marked_out[node1]=check;
	marked_in[node1]=check;
	cc_dynamic->set_cc(node1,cc_num);
	
	while((!queue_out->empty())||(!queue_in->empty()))
	{
		size_out=queue_out->get_size();
		size_in=queue_in->get_size();
		/****work for queue_out****/
		for(count=0;count<size_out;++count)
		{
			node1=queue_out->front();
			if(marked_out[node1]!=check)			//if first on queue_out not marked out,mark and set_cc
			{
				marked_out[node1]=check;
				cc_dynamic->set_cc(node1,cc_num);
			}			
			queue_out->pop();
			where2=table_out->get_offset(node1);
			if(where2<0)
				temp_out=NULL;
			else
				temp_out=buffer_out->getListHead(where2);
			while(temp_out!=NULL)          //for all neighbors of node
			{
				for(j=0;j<temp_out->get_next_free_neighbor();++j)
				{												
					neighbor=temp_out->get_neighbor(j);				//take the j-th neighbor
					if(marked_out[neighbor]!=check)
					{								
						marked_out[neighbor]=check;
						queue_out->push(neighbor);          //if not marked_out push on queue_out
						cc_dynamic->set_cc(neighbor,cc_num);

						if(marked_in[neighbor]!=check)								
							queue_in->push(neighbor);             //if not marked_in push on queue_in
					}
				}
				temp_out=temp_out->get_next();
			}	
		}
		/****work for queue_in****/
		for (count=0;count<size_in;++count)
		{
			node1=queue_in->front();
			if(marked_in[node1]!=check)          //if first on queue_in not marked in,mark and set_cc
			{
				marked_in[node1]=check;
				cc_dynamic->set_cc(node1,cc_num);
			}
			queue_in->pop();
			where2=table_in->get_offset(node1);
			if(where2<0)
				temp_in=NULL;
			else
				temp_in=buffer_in->getListHead(where2);
			while(temp_in!=NULL)                //for all neighbors of node
			{
				for(j=0;j<temp_in->get_next_free_neighbor();++j)
				{												
					neighbor=temp_in->get_neighbor(j);			 //take the j-th neighbor
					if(marked_in[neighbor]!=check)
					{								
						marked_in[neighbor]=check;
						queue_in->push(neighbor);			 //if not marked_in push on queue_in
						cc_dynamic->set_cc(neighbor,cc_num);

						if(marked_out[neighbor]!=check)			 //if not marked_out push on queue_out
							queue_out->push(neighbor);
					}
				}
				temp_in=temp_in->get_next();
			}	
		}
	}
	delete queue_out;
	delete queue_in;
	return 1;
}


int the_bbfs(uint32_t start,uint32_t end,Stronglycc* Scc,Node_Index* table_in,Node_Index* table_out,Buffer* buffer_out,Buffer* buffer_in,int* depth,int* marked_out,int* marked_in,int check,Queue<uint32_t>* queue_out,Queue<uint32_t>* queue_in,uint32_t version)
{   //search for path using all the structures you've created
	uint32_t node1,node2,scc_num,neighbor;
	node1=start;
	node2=end;	
	List_node* temp_out=NULL;
	List_node* temp_in=NULL;
	queue_out->push(node1);
	queue_in->push(node2);
	marked_out[node1]=check;
	marked_in[node2]=check;
	int size_in,size_out,count,flag;
	int where2,where1,j;
	int flag_not_scc_bbfs=0;
	scc_num=UINT32_MAX;		//to avoid warning--practicly is of no use

	if(Scc!=NULL)
	{
		if(Scc->get_component(start)!=Scc->get_component(end))
			flag_not_scc_bbfs=1;
		else
			scc_num=Scc->get_component(start);	
	}
	
	while((!queue_out->empty())&&(!queue_in->empty()))
	{
		size_in=queue_in->get_size();
		size_out=queue_out->get_size();
		if(size_in<=size_out)
			flag=1;
		else
			flag=2;
		if(!queue_in->empty() && flag==1)
		{
			(*depth)++;
			for(count=0;count<size_in;count++)
			{
				node2=queue_in->front();
				queue_in->pop();                           //pop first on queue_in
				where2=table_in->get_offset(node2);			//get offset  to find listhead on buffer
				if(where2<0)
					temp_in=NULL;
				else
					temp_in=buffer_in->getListHead(where2);
				if(marked_out[node2]==check)
				{
					(*depth)--;		
					return 1;                    //found path
				}
				while(temp_in!=NULL)
				{
					for(j=0;j<temp_in->get_next_free_neighbor();j++)
					{			
						neighbor=temp_in->get_neighbor(j);                 //get the j-th neighbor
						if(temp_in->get_property(j)>version)				//check for version
							continue;
						/*******************************************************/
						if(Scc!=NULL)			//not the simple bbfs
						{	
							if(flag_not_scc_bbfs)		//grail needed
							{	if(grail_question(Scc,start,neighbor)<0)
									continue;
							}
							else    //scc_bbfs
							{
								if(Scc->get_component(neighbor)!=scc_num)
									continue;
							}
						}

						/******************************************************/
						if(marked_in[neighbor]!=check)
						{								
							if(marked_out[neighbor]==check)
							{
								return 1;					//found path
							}
							marked_in[neighbor]=check;
							queue_in->push(neighbor);      //push neighbor on queue_in
						}
					}
					temp_in=temp_in->get_next();
				}
			}
		}			
		if(!queue_out->empty() && flag==2)
		{
			(*depth)++;
			for(count=0;count<size_out;count++)
			{
				node1=queue_out->front();
				queue_out->pop();						//pop first on queue_out
				where1=table_out->get_offset(node1);	//get offset  to find listhead on buffer
				if(where1<0)
					temp_out=NULL;
				else
					temp_out=buffer_out->getListHead(where1);
				if(marked_in[node1]==check)
				{
					(*depth)--;					
					return 1;				//found path
				}
				while(temp_out!=NULL)
				{
					for(j=0;j<temp_out->get_next_free_neighbor();j++)
					{												
						neighbor=temp_out->get_neighbor(j);				//get the j-th neighbor
						if(temp_out->get_property(j)>version)			//check for version
							continue;
						/*******************************************************/
						if(Scc!=NULL)		//not the simple bbfs
						{
							if(flag_not_scc_bbfs)    //grail needed
							{
								if(grail_question(Scc,neighbor,end)<0)
									continue;
							}
							else           //scc_bbfs
							{
								if(Scc->get_component(neighbor)!=scc_num)
									continue;
							}
						}
						/******************************************************/		
						if(marked_out[neighbor]!=check)
						{								
							if(marked_in[neighbor]==check)
							{
								return 1;					//found path
							}
							marked_out[neighbor]=check;
							queue_out->push(neighbor);				//push neighbor on queue_out
						}
					}
					temp_out=temp_out->get_next();
				}	
			}
		}		
	}
	return -1;			//no path found
}