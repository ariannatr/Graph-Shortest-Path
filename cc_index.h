#ifndef CC_INDEX_H 
#define CC_INDEX_H
#include "buffer.h"
#include "node_index.h"
#include "graph.h"
#include <stdint.h>
#define N 20


class Node_upd
{
	uint32_t * updates;
	uint32_t next_available;
	uint32_t size;
	public:
		Node_upd()
		{
			updates=(uint32_t *)calloc(N,sizeof(uint32_t));		//used for keeping cc merges
			next_available=0;
			size=N;
		};
		~Node_upd()
		{
			free(updates);
		};
		uint32_t get_next_available(){return next_available;};
		void set_next_available(){next_available++;};
		void add_update(uint32_t add)	//add a new cc that is merged
		{
			if(next_available>=size)//extend the updates index
			{
				updates=(uint32_t*)realloc(updates,2*size*sizeof(uint32_t));
				size=2*size;
			}
			updates[next_available]=add;
			next_available++;
		}
		int exists(uint32_t a)		//check if a particular cc belongs to the updates of a particular cc
		{
			for(uint32_t i=0;i<next_available;i++)
			{
				if(updates[i]==a)
					return 1;
			}
			return -1;
		};
};

class CC_Index
{
	uint32_t * cc;
	Node_upd ** update;
	uint32_t size;
	uint32_t num_of_cc;
	uint32_t use_update;
	public:
		CC_Index(uint32_t n);
		~CC_Index();
		uint32_t get_cc(uint32_t i);
		void set_cc(uint32_t i,uint32_t count);
		uint32_t get_size();
		void double_size(uint32_t i);
		void cc_create(Graph* G,int* marked_out,int * marked_in);
		void create_update_index();
		void update_cc_indexes(uint32_t node1,uint32_t node2);
		int exists_path(uint32_t node1,uint32_t node2);
		uint32_t get_use(){return use_update;};
		uint32_t get_num_of_cc(){return num_of_cc;};
};

#endif
