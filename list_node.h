#ifndef NODE_H
#define NODE_H
#include <stdint.h>

class List_node
{
	uint32_t* neighbor; //the ids of the neighbor nodes
	class List_node* next;
	uint32_t* property;
	int next_free_neighbor;
	public:
		List_node();
		~List_node();
		int add_neighbor(uint32_t node2,uint32_t property);
		int add_neighbor2(uint32_t node2,List_node* ptr,uint32_t property);
		void print_all();
		int exists_neighbor(uint32_t node2);
		uint32_t get_neighbor(int i);
		uint32_t get_property(int i);
		uint32_t get_neighbor_from_start(int i);
		int get_next_free_neighbor();
		List_node * get_next();
		int  compare_neighbors(List_node* temp);
};


#endif
