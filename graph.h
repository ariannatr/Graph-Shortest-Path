#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "node_index.h"
#include "buffer.h"
#include <stdint.h>
using namespace std;


class Graph
{
	Node_Index *mytable_in;//26
	Node_Index *mytable_out;//120
	Buffer *buffer;				//index for outgoing edges
	Buffer *ingoing_buffer;		//index for ingoing edges
	int graphtype;
	
	public:
		Graph();
		~Graph();
		void insert_nodes(uint32_t node1,uint32_t node2);
		int add_edge(uint32_t node1,uint32_t node2,int version);		//returns 0 for ok,1 if its already there,-1 when failure
		void set_graphtype(int type);
		int get_graphtype();
		void build_strongly_cc_and_grail_index(int size);
		Node_Index* get_table_out();
		Node_Index* get_table_in();
		Buffer* get_buffer_out();
		Buffer* get_buffer_in();
};

#endif
