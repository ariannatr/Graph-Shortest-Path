#ifndef STACK_H
#define STACK_H
#include <stdint.h>

class Stack
{
	uint32_t* array;
	int counter;
	int size;
	int front;
	public:
		Stack();
		~Stack();
		void push(uint32_t data);
		uint32_t pop();
		int onStack(uint32_t id);
		int empty();
		int get_count();
		uint32_t get_front();		
};  

#endif
