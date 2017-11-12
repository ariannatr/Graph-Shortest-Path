#ifndef SCC_H
#define SCC_H
#include <stdint.h>
using namespace std;

class Component   /*index's members*/
{
	uint32_t comp_id;
	uint32_t included_count;
	uint32_t capacity;
	uint32_t* included_ids;

	uint32_t rank[2];
	uint32_t min_rank[2];
	uint32_t count_visited[2];

	public:
		Component(uint32_t identifier);
		uint32_t get_rank(int which);
		uint32_t get_min_rank(int which);
		void set_visited_count();
		uint32_t get_visited_count(int which);
		void visited_count_plus(int which);
		void set_rank(uint32_t rank,int which);
		void set_min_rank(uint32_t min_rank,int which);
		void add(uint32_t node);
		void printa();    //used for checking results
		uint32_t get_count();
		uint32_t get_id();
		uint32_t get_included_id(uint32_t offset);

		void free_array_included_ids();
};

class Stronglycc     /*index*/
{
	Component** components;
	uint32_t capacity;
	uint32_t comp_count;
	uint32_t* id_belongs_to_component;
	public:
		Stronglycc(int size);
		~Stronglycc();
		uint32_t get_count();
		void add(Component* comp); 
		void print_all();      //used for checking results
		void set_component(uint32_t node,uint32_t cc);
		uint32_t get_component(int node);
		Component* get_comp(int offset);
		
};

class Component_Cursor
{
	Component* comp_ptr;
	int comp_count;
	int current;
	public:
		Component_Cursor(int count);
		~Component_Cursor();
		void start_cursor();
		bool next_cursor();
		Component* get_cursor();
};
#endif
