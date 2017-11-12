#ifndef COMPONENT_INDEX_H
#define COMPONENT_INDEX_H
#include <stdint.h>

class Component_info
{
	int index_num;
	int lowlink_num;
	bool onstack;

	int vindex;
	int caller;

	public:
		Component_info()
		{
			index_num=0;
			lowlink_num=0;
			onstack=false;
		};
		~Component_info(){};
		void starter_sets()
		{
			index_num=0;
			lowlink_num=0;
			onstack=false;
		};
		void set_index_num(int in){index_num=in;};
		int get_index_num(){return index_num;};
		void set_lowlink_num(int ll) {lowlink_num=ll;};
		int get_lowlink_num(){return lowlink_num;};
		
		bool get_onstack(){return onstack;};
		void set_onstack(bool b){onstack=b;};
		
		void set_vindex(int vin){vindex=vin;};
		int get_vindex(){return vindex;};
		void set_caller(int ll) {caller=ll;};
		int get_caller(){return caller;};
};

class Component_Index
{
	Component_info* cinfo;
	int size;
	public:
		Component_Index(int n);
		~Component_Index();
		void set_index(uint32_t node,int in);
		int get_index(uint32_t node);
		void set_lowlink(uint32_t node,int ll) ;
		int get_lowlink(uint32_t node);
		
		int get_size();
		bool onStack(uint32_t node);
		void set_onStack(uint32_t id,bool b);
		void set_start();


		void set_vindex(uint32_t id,int vin);
		int get_vindex(uint32_t id);
		void set_caller(uint32_t id,int ll);
		int get_caller(uint32_t id);
};

#endif
