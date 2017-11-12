#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include <stdint.h>
using namespace std;

#define SIZE 600
	   
Stack::Stack()
{
    front = -1;
	counter = 0;
	size=SIZE;
	array=(uint32_t *)malloc(sizeof(uint32_t)*SIZE);
}

Stack::~Stack()
{
	free(array);
}

int Stack::empty()
{
	return (front==-1);
}

int Stack::get_count()
{
	return counter;
}

void Stack::push(uint32_t data)
{
	if (front==size-1)          //need to realloc to fit in
	{
		array=(uint32_t *)realloc(array,sizeof(uint32_t)*2*size);
		size=2*size;
	}
	counter++;
	front++;
	array[front]=data;
}

uint32_t Stack::pop()
{
	if (this->empty())
		return -1; 
	else
	{
		counter--;
		int toreturn=array[front];
		front--;
		return toreturn;
	 }
}

int Stack::onStack(uint32_t id)
{
	for(int i=0;i<=front;i++)
	{
		if(array[i]==id)
			return 1;
	}
	return -1;
}

uint32_t Stack::get_front()
{
	if(front==-1)
		return -1;
	return array[front];
}


