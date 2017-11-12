#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#define N 500
#include "buffer.h"
#include "list_node.h"

using namespace std;


Buffer::Buffer()
{
	buffer=(List_node**) malloc(N*sizeof(List_node*));
	next_available=0;
	size=N;
}

Buffer::~Buffer()
{	
	int i;
	for(i=0;i<next_available;i++)
		delete buffer[i];
	free(buffer);
}

int Buffer::insert_node_in_buffer()
{
	int toreturn;
	if(next_available==size)   //need to realloc buffer
	{
		buffer=(List_node**)realloc(buffer,2*size*sizeof(List_node*));
		this->size=size*2;
	}
	toreturn=next_available;
	buffer[toreturn]=new List_node();   //create new List_node for inserting node 
	next_available++;
	return toreturn;
}

List_node* Buffer::getListHead(int offset)
{
	if(offset<0)
		return NULL;
	return (buffer[offset]);
}

int Buffer::getNextAvailable()
{
	int toreturn;
	if(next_available==size)   //need to realloc buffer
	{
		buffer=(List_node**)realloc(buffer,2*size*sizeof(List_node*));
		size=size*2;
	}
	toreturn=next_available;
	buffer[toreturn]=new List_node(); //create new List_node for use of a previously inserted node 
	next_available++;
	return toreturn;
}
