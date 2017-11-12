#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "scc.h"
#include <stdint.h>
#define INCLUDED_N 20
#define SCC_N 500
using namespace std;

Stronglycc::Stronglycc(int size)
{
	components=(Component**)malloc(SCC_N *sizeof(Component*));
	capacity=SCC_N;
	comp_count=0;
	id_belongs_to_component=(uint32_t *)malloc(size*sizeof(uint32_t));   
	memset(id_belongs_to_component,UINT32_MAX,size);
}

Stronglycc::~Stronglycc()
{
	for(uint32_t i=0;i<comp_count;i++)
		delete components[i];
	free(components);
	free(id_belongs_to_component);
}

uint32_t Stronglycc::get_count()
{
	return comp_count;
}

void Stronglycc::add(Component* comp)
{
	if(comp_count==capacity)			//need to realloc in order to fit new component
	{
		components=(Component**)realloc(components,2*capacity*sizeof(Component*));
		capacity=capacity*2;
	}
	components[comp_count]=comp;
	comp_count++;
}

void Stronglycc::print_all()
{
	cout<<"Found "<<comp_count<<" scc\n";
	for(uint32_t i=0;i<comp_count;i++)
	{
		components[i]->printa();
	}
}


void Stronglycc::set_component(uint32_t node,uint32_t cc)
{
	id_belongs_to_component[node]=cc;
}

uint32_t Stronglycc::get_component(int node)
{
	return id_belongs_to_component[node];
}

Component* Stronglycc::get_comp(int offset)
{
	return components[offset];
}

Component::Component(uint32_t identifier)
{
	comp_id=identifier;
	included_count=0;
	capacity=INCLUDED_N;
	rank[0]=0;
	rank[1]=0;
	min_rank[0]=UINT32_MAX;
	min_rank[1]=UINT32_MAX;
	count_visited[0]=0;
	count_visited[1]=0;
	included_ids=(uint32_t* )malloc(INCLUDED_N*sizeof(uint32_t));	
}


void Component::add(uint32_t node)
{
	if(included_count==capacity)              //need to realloc in order to fit node in  array
	{
		included_ids=(uint32_t *)realloc(included_ids,2*capacity*sizeof(uint32_t));
		capacity=capacity*2;
	}
	included_ids[included_count]=node;
	included_count++;
}


uint32_t Component::get_rank(int which)
{
	return this->rank[which-1];
}

uint32_t Component::get_min_rank(int which)
{
	return this->min_rank[which-1];
}

void Component::set_visited_count()
{
	count_visited[0]=0;
}

uint32_t Component::get_visited_count(int which)
{
	return count_visited[which-1];
}

void Component::visited_count_plus(int which)
{
	(count_visited[which-1])++;
	return;
}

void Component::set_rank(uint32_t rank,int which)
{
	this->rank[which-1]=rank;
	
}

void Component::set_min_rank(uint32_t min_rank,int which)
{
	this->min_rank[which-1]=min_rank;
}

void Component::printa()
{
	cout <<"C"<<comp_id<<":";
	for(uint32_t j=0;j<included_count;j++)
		cout <<" "<<included_ids[j];
	cout<<endl;
}

uint32_t Component::get_count()
{
	return included_count;
}

uint32_t Component::get_id()
{
	return comp_id;
}

uint32_t Component::get_included_id(uint32_t offset)
{
	if(offset>included_count)
		return -1;
	else
		return included_ids[offset];
}

void Component::free_array_included_ids()
{
	free(included_ids);
}


Component_Cursor::Component_Cursor(int count)
{
	comp_ptr=NULL;
	comp_count=count;
	current=-1;
}

Component_Cursor::~Component_Cursor()
{
	free(comp_ptr);
}

void Component_Cursor::start_cursor()
{
	current=0;
}

bool Component_Cursor::next_cursor()
{
	if(current+1<comp_count)
	{
		current++;
		return true;
	}
	else
		return false;
}

Component* Component_Cursor::get_cursor()
{
	return comp_ptr;
}

