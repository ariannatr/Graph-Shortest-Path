#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "list_node.h"
#define N 50

using namespace std;


List_node::List_node()
{
	neighbor=(uint32_t*)malloc(sizeof(uint32_t)*N);
	property=(uint32_t*)calloc(N,sizeof(uint32_t));
	next=NULL;
	next_free_neighbor=0;
}

List_node::~List_node()
{
	free(neighbor);
	free(property);
}

int List_node::add_neighbor(uint32_t node2,uint32_t property)
{
	List_node * temp=this;
	if(temp!=NULL)
	{
		if(temp->next_free_neighbor<N)
		{
			temp->neighbor[temp->next_free_neighbor]=node2;
			temp->property[temp->next_free_neighbor]=property;
			temp->next_free_neighbor++;
			return 0;
		}
	}
	return -1;    //if new List node must be alloced for the new neighbor
}

List_node * List_node::get_next()
{
	return this->next;
}

uint32_t List_node::get_neighbor(int i)
{
	return this->neighbor[i];
}

uint32_t List_node::get_property(int i)
{
	return this->property[i];
}

uint32_t List_node::get_neighbor_from_start(int i)   
{ 
	//search all linked listnodes to find the i-th neighbor
	if( i< this->next_free_neighbor)    //if its in the first listnode
		return this->neighbor[i];
	else
	{
		int searched=this->next_free_neighbor;
		int k;
		List_node* temp=this->next;
		while(temp!=NULL)       //search in the rest listnodes
		{
			for(k=0;k<temp->next_free_neighbor;k++)
			{
				if(i==searched+k)
					return temp->neighbor[k];
			}
			searched+=k;
			temp=temp->next;
		}
		return UINT32_MAX;
	}
}

int List_node::get_next_free_neighbor()
{
	return this->next_free_neighbor;     //return the next free position in the array of neighbors
}


int  List_node::add_neighbor2(uint32_t node2,List_node* ptr,uint32_t property)
{
	List_node* tmp;
	tmp=this;
	while(tmp->next!=NULL)   
		tmp=tmp->next;
	tmp->next=ptr;      
	return tmp->next->add_neighbor(node2,property);       //add neighbor in the newly allocated listnode (because neiughbor didnt feet before)
}

void List_node::print_all()
{
	int i;
	for(i=0;i<next_free_neighbor;i++)
	{
		cout << neighbor[i] << endl;
	}
	if(next!=NULL)
		next->print_all();
}

int  List_node::exists_neighbor(uint32_t node2)
{
	List_node* temp=this;
	while(temp!=NULL)
	{	
		for(int i=0;i<temp->next_free_neighbor;i++)
		{
			if(temp->neighbor[i]==node2)
				return 0;
		}
		temp=temp->next;
	}
	return -1;

}


int List_node::compare_neighbors(List_node* temp)
{
	List_node* mytemp=this;
	while(mytemp!=NULL)
	{	
		for(int i=0;i<mytemp->next_free_neighbor;i++)
		{
			if(temp->exists_neighbor(neighbor[i])==0)
				return 1;
		}
		mytemp=mytemp->next;
	}
	
	return -1;
	
}
