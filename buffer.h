#ifndef BUFFER_H
#define BUFFER_H
#include "list_node.h"

class Buffer
{
	List_node ** buffer;
	int next_available;
	int size;
	public:
		Buffer();
		~Buffer();
		int insert_node_in_buffer();
		List_node* getListHead(int offset);
		int getNextAvailable();
};

#endif
