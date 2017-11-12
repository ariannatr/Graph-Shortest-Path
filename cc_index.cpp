#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "cc_index.h"
#include "buffer.h"
#include "node_index.h"
#include "other_functions.h"
#include "graph.h"
#include <stdint.h>

using namespace std;

CC_Index::CC_Index(uint32_t n)
{
	cc=(uint32_t*)calloc(n,sizeof(uint32_t));
	update=NULL;
	size=n;
	num_of_cc=0;
	use_update=0;
}

CC_Index::~CC_Index()
{
	free(cc);
	uint32_t i;
	for(i=0;i<(num_of_cc+1);i++)
	{
		if(update[i]!=NULL)
		delete update[i];
	}
	free(update);
}

uint32_t CC_Index::get_cc(uint32_t i)
{
	if(i>=size)				//node id is larger than size
		return 0;
	return cc[i];
}

void CC_Index::set_cc(uint32_t i,uint32_t count)
{
	if(i>=size)			//node id is larger than size
		double_size(i);

	cc[i]=count;
}

uint32_t CC_Index::get_size()
{
	return size;
}

void CC_Index::double_size(uint32_t i)
{
	int new_megethos=2;
	while(i>=new_megethos*size)					//calculate until node id can fit in cc
	{
		new_megethos=new_megethos*2;
	}
	cc=(uint32_t *)realloc(cc,new_megethos*size*(sizeof(uint32_t)));

	memset(cc+size,0,(new_megethos*size-size)*sizeof(uint32_t));			//set the newly alloced memory to 0
	size=new_megethos*size;
}

void CC_Index::cc_create(Graph* G,int* marked_out,int * marked_in)
{
	uint32_t cc_num=0;
	uint32_t *check=(uint32_t*)malloc(sizeof(uint32_t));		//used for marking
	*check=-1;
	uint32_t node=0;
	uint32_t size=this->get_size();
	
	while(node<size)		//find the nodes belong to a cc with a bfs search
	{
		if(this->get_cc(node)==0&& ((G->get_table_out())->exists_node(node)==0||(G->get_table_in())->exists_node(node)==0))	//if the node doesn't belong yet to any cc ,have a bfs search to make the next cc
		{
			cc_num++;
			bfs(node,G,marked_out,marked_in,this,cc_num,*check);
		}
		(*check)=(*check)-1;
		node++;
	}
	this->num_of_cc=cc_num;
	free(check);
}

void CC_Index::create_update_index()
{
	update=(Node_upd **)calloc(num_of_cc+1,sizeof(Node_upd*));
}

void CC_Index::update_cc_indexes(uint32_t node1,uint32_t node2)
{
	uint32_t cc_node1,cc_node2;
	cc_node1=this->get_cc(node1);
	cc_node2=this->get_cc(node2);
	if(cc_node1==0&&cc_node2!=0) 		//if cc_node1 is now inserted in the graph,it belongs to cc_node2's cc
	{
		this->set_cc(node1,cc_node2);
	}
	else if(cc_node2==0&&cc_node1!=0)	//if cc_node2 is now inserted in the graph,it belongs to cc_node1's cc
	{
		this->set_cc(node2,cc_node1);
	}
	else if(cc_node1==0&&cc_node2==0)			//practicly this case is ureachable but needed for checking
	{
		cerr<<"unreachable case\n";
		return;
	}
	else if(cc_node1==cc_node2)			//no need for changes
	{
		return;
	}
	else							//if node1,belongs to different cc from node2
	{
		if(update[cc_node1]==NULL)	//node1's cc hasn't any merges yet
			update[cc_node1]= new Node_upd();
		update[cc_node1]->add_update(cc_node2);
		if(update[cc_node2]==NULL)	//node2's cc hasn't any merges yet
			update[cc_node2]= new Node_upd();
		update[cc_node2]->add_update(cc_node1);
	}
}

int CC_Index::exists_path(uint32_t node1,uint32_t node2)
{
	uint32_t cc_node1,cc_node2;
	cc_node1=get_cc(node1);
	cc_node2=get_cc(node2);

	if(cc_node1>num_of_cc || cc_node2>num_of_cc )			//practicly this case is ureachable but needed for checking
	{
		return 0;
	}
	if(cc_node1==0 || cc_node2==0)			    //node doesn't exist
	{
		return 0;
	}
	else if(cc_node1==cc_node2 && cc_node2!=0)		//they belong to the same cc
	{
		return 1;
	}
	else if(cc_node1!=cc_node2)		//they do not belong to the same cc
	{
		if((update[cc_node1]!=NULL) && (update[cc_node2]!=NULL) && update[cc_node1]->get_next_available()<update[cc_node2]->get_next_available())	//node1's cc has less updates,check firts node1;s updates
		{
			if((update[cc_node1]!=NULL)&&(update[cc_node1]->exists(cc_node2)==1)) //node1'cc,node2's cc are merged 	
			{
				this->use_update++;		//update the metric of using updateindex properly
				return 2;
			}
			else if((update[cc_node2]!=NULL)&&(update[cc_node2]->exists(cc_node1)==1))
			{
				this->use_update++;		//update the metric of using updateindex properly
				return 2;
			}
			else
				return 0;
		}
		else		//node2's cc has less updates(or equal),check first node2's updates
		{
			if((update[cc_node2]!=NULL)&&(update[cc_node2]->exists(cc_node1)==1))
			{
				this->use_update++;	//update the metric of using updateindex properly
				return 2;
			}
			else if((update[cc_node1]!=NULL)&&(update[cc_node1]->exists(cc_node2)==1))
			{
				this->use_update++;	//update the metric of using updateindex properly
				return 2;
			}
			else
				return 0;
		}
	}
	return 0;
}
