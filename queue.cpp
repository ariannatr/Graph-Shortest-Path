#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <stdint.h>
#include "job_scheduler.h"

#define PLITHOS 600

template <class T>	   
Queue<T>::Queue()
{
	head =  0;
    tail = 0;
	counter = 0;
	capacity=PLITHOS;
	array=(T *)malloc(sizeof(T)*capacity);
}

template <class T>	   
Queue<T>::~Queue()
{
	free(array);
}

template <class T>	   
int Queue<T>::empty()
{
	return (counter==0);
}

template <class T>	   
void Queue<T>::clear()
{
	head =  0;
    tail = 0;
	counter = 0;	
}

template <class T>	   
void Queue<T>::push(T data)
{
	if (tail==capacity)//
	{
		array=(T*)realloc(array,sizeof(T)*capacity*2);
		
		capacity=capacity*2;
	}
	counter++;
	array[tail]=data;
	tail=(tail+1);
}

template <class T>
void Queue<T>::pop()
{
	if (this->empty())
		return; 
	else
	{
		counter--;
		head=(head+1);
	}
	if(this->empty())
	{
		head=0;
		tail=0;
		counter=0;
	}
}

template <class T>
T Queue<T>::front()
{
	//if(this->empty())
	//	return NULL;
	return array[head];
}

template <class T>
uint32_t Queue<T>::get_size()
{
	return counter;
}


template class Queue<uint32_t>;
template class Queue<Job>;