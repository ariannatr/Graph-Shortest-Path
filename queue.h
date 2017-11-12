#ifndef QUEUE_H 
#define QUEUE_H
#include <stdint.h>
using namespace std;

template <class T>
class Queue
{
	uint32_t head;	
	uint32_t tail;	
	uint32_t counter;
	T* array;
	uint32_t capacity;
	public:
		Queue();
		~Queue();
		int empty();
		void push(T data);
		void pop();
		T front();
		uint32_t get_size();
		void clear();
};


#endif 
