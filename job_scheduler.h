#ifndef JOB_SCHEDULER_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "queue.h"

#include "node_index.h"
#include "buffer.h"
#include "scc.h"
#include "cc_index.h"
#include "graph.h"

using namespace std;

class Job
{
	public:
		uint32_t node1;
		uint32_t node2;
		int id;
		int version;
};

class Arguments_Needed
{
	public :
	int graphtype;
	Node_Index* mytable_in;
	Node_Index* mytable_out;
	Buffer* index;
	Buffer* ingoing_index;
	Stronglycc* Scc;
	CC_Index* cc_index;

};

class JobScheduler
{
	int execution_threads;
	int priority;
	Queue <Job> queue_job;
	pthread_t* tids;
	int terminate_threads;
	pthread_mutex_t mtx_for_data ;
	pthread_mutex_t mtx_for_jobs ;
	pthread_cond_t cond_start ;
	pthread_cond_t cond_execute ;
	Arguments_Needed args;
	int * results;
	int results_size;
	int numof_results;
	int numof_queries;
	public:
		int ready;
		JobScheduler(int ex,Graph* G);
		~JobScheduler();
		void pass_rest_arguments(int type,Stronglycc* scc,CC_Index* cc_dynamic);
		void submit_result(int pr,int result);
		void submit_job(Job j);
		void  execute_all_jobs(int num);
		void wait_execute();
		void print_results();
		Job get_front_job();
		void decrease_numof_results(int num);
		void decrease_numof_queries(int num);
		int get_num_of_results();
		int get_num_of_queries();
		int get_terminate_threads();
		int get_queue_size();
		int get_priority();
		void lock_data();
		void unlock_data();
		void wait_all_tasks_finish();
		void signal_master_continue();
		Arguments_Needed get_args();

};

#endif
