#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "node_index.h"
#include "buffer.h"
#include "graph.h"
#include <stdint.h>
using namespace std;




Graph::Graph()
{

	mytable_out=new Node_Index(30);
	mytable_in=new Node_Index(30);
	buffer=new Buffer();
	ingoing_buffer=new Buffer();
}

Graph::~Graph()
{
	delete mytable_in;
	delete mytable_out;
	delete buffer;
	delete ingoing_buffer;
}


void Graph::insert_nodes(uint32_t node1,uint32_t node2)
{
	int where_on_index;
	where_on_index=mytable_out->get_offset(node1);		//where in mytable is node1
	if( where_on_index <0)									//if it isn't in mytable ,add it and insert in index 
	{
		mytable_out->set_offset(node1,buffer->insert_node_in_buffer());
	}
	where_on_index=mytable_in->get_offset(node2);		//where in mytable is node1
	if( where_on_index <0)									//if it isn't in mytable ,add it and insert in index 
	{
		mytable_in->set_offset(node2,ingoing_buffer->insert_node_in_buffer());
	}
}

int Graph::add_edge(uint32_t node1,uint32_t node2 ,int version)
{
	int flag,nextavailable;
	if(mytable_out->get_neighbors(node1)<=mytable_in->get_neighbors(node2)) 		//find if neighbor exists in the node that has less neighbors
		flag=(buffer->getListHead(mytable_out->get_offset(node1)))->exists_neighbor(node2);
	else
		flag=(ingoing_buffer->getListHead(mytable_in->get_offset(node2)))->exists_neighbor(node1);
	if( flag<0)			//if the neighbor doesnt already exist
	{
		if((buffer->getListHead(mytable_out->get_last_offset(node1)))->add_neighbor(node2,version) <0)  //if there isnot space in the last listnode
		{
			nextavailable=buffer->getNextAvailable();
			(buffer->getListHead(mytable_out->get_last_offset(node1)))->add_neighbor2(node2,buffer->getListHead(nextavailable),version);		//add neighbor in a new List_node
			mytable_out->set_last_offset(node1,nextavailable);
		}			
		mytable_out->set_neighbors(node1);	//need in order to keep track of the sum of neighbors of its node
		
		//fix the ingoing edge
					
		if((ingoing_buffer->getListHead(mytable_in->get_last_offset(node2)))->add_neighbor(node1,version) <0)   //if there isnot space in the last listnode
		{
			nextavailable=ingoing_buffer->getNextAvailable();
			(ingoing_buffer->getListHead(mytable_in->get_last_offset(node2)))->add_neighbor2(node1,ingoing_buffer->getListHead(nextavailable),version);   //add neighbor in a new List_node
			mytable_in->set_last_offset(node2,nextavailable);
		}

		mytable_in->set_neighbors(node2);         //need in order to keep track of the sum of neighbors of its node

		return 0;
	}
	else
		return 1;
}



void Graph::set_graphtype(int type)
{
	graphtype=type;
}

int Graph::get_graphtype()
{
	return graphtype;
}

Node_Index* Graph::get_table_out()
{
	return mytable_out;
}

Node_Index* Graph::get_table_in()
{
	return mytable_in;
}

Buffer* Graph::get_buffer_out()
{
	return buffer;
}
		
Buffer* Graph::get_buffer_in()
{
	return ingoing_buffer;
}