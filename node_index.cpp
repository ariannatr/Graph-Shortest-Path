#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include "node_index.h"

using namespace std;


Node_Index::Node_Index(int n)
{
	index=(Node_info**)calloc(n,sizeof(Node_info*));
	size=n;
}

Node_Index::~Node_Index()
{
	int i;
	for( i=0;i<size;i++)
		free(index[i]);
	free(index);
	cerr<<"An index has been destroyed \n";
}

int Node_Index::get_size()
{
	return this->size;
}

void Node_Index::set_offset(int node,int offset)
{
	if(node>(size-1))
	{
		int new_megethos=2;
		while(node>(new_megethos*size))
		{
			new_megethos=new_megethos*2;  //calculate how much bigger must index be in order to feet the new node
		}
		this->index=(Node_info**)realloc(this->index,new_megethos*size*sizeof(Node_info*));   //realloc index 
		
		memset(index+size,0,((new_megethos*size)-size)*sizeof(Node_info*));     //set the new positions of index array to 0
		this->size=new_megethos*(this->size);
	}
	index[node]=(Node_info*)malloc(sizeof(Node_info));    
	index[node]->set_start_offset(offset);
	index[node]->set_sum_of_neighbors(0);
	set_last_offset(node,offset);    //when node is created the start offset is also the last offset
}



int Node_Index::get_offset(int node)
{
	if(node >size-1)
		return -1;
	if(index[node]==NULL)
		return -1;
	return index[node]->get_start_offset();
}


int Node_Index::exists_node(int node)
{
	if (get_offset(node)==-1)
		return -1;
	else
		return 0;
}

void Node_Index::set_last_offset(int node,int last_offset)
{
	index[node]->set_last_offset(last_offset);
}

int Node_Index::get_last_offset(int node)
{
	if(node >size-1)
		return -1;
	if(index[node]==NULL)
		return -1;
	return index[node]->get_last_offset();
}

void Node_Index::set_neighbors(int node)
{
	index[node]->set_sum_of_neighbors(index[node]->get_sum_of_neighbors()+1);    //added one neighbor
}



int Node_Index::get_neighbors(int node)
{
	if(node >size-1)
		return -1;
	if(index[node]==NULL)
		return -1;
	return index[node]->get_sum_of_neighbors();
}


