#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "info_for_scc_build.h"

using namespace std;

Component_Index::Component_Index(int n)
{ 
	cinfo=(Component_info*)calloc(n,sizeof(Component_info));
	size=n;
}
Component_Index::~Component_Index()
{
	free(cinfo);
}

void Component_Index::set_index(uint32_t node,int in)
{
	
	cinfo[node].set_index_num(in);
}

int Component_Index::get_index(uint32_t node)
{
	return (cinfo[node].get_index_num());
}

void Component_Index::set_lowlink(uint32_t node,int ll) 
{	
	cinfo[node].set_lowlink_num(ll);
}

int Component_Index::get_lowlink(uint32_t node)
{
	return (cinfo[node].get_lowlink_num());
}

int Component_Index::get_size()
{
	return size;
}

bool Component_Index::onStack(uint32_t id)
{
	return cinfo[id].get_onstack();
}

void Component_Index::set_onStack(uint32_t id,bool b)
{
	cinfo[id].set_onstack(b);
}

void Component_Index::set_start()
{
	for(int x=0;x<size;x++)
		cinfo[x].starter_sets();	
}

void Component_Index::set_vindex(uint32_t node,int vin)
{
	
	cinfo[node].set_vindex(vin);
}

int Component_Index::get_vindex(uint32_t node)
{
	return (cinfo[node].get_vindex());
}

void Component_Index::set_caller(uint32_t node,int ll)
{
	
	cinfo[node].set_caller(ll);
}

int Component_Index::get_caller(uint32_t node)
{
	return (cinfo[node].get_caller());
}
