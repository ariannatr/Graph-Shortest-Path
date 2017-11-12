#ifndef INDEX_H
#define INDEX_H

class Node_info
{
	int start_offset;
	int last_offset;
	int sum_of_neighbors;
	public:
		Node_info()
		{
			start_offset=-1;
			last_offset=-1;
			sum_of_neighbors=0;
		}
		void set_start_offset(int start){ start_offset=start;}
		void set_last_offset(int last){last_offset=last;}
		void set_sum_of_neighbors(int sum){sum_of_neighbors=sum;}
		int get_start_offset(){return start_offset;}
		int get_last_offset(){return last_offset;}
		int get_sum_of_neighbors(){return sum_of_neighbors;}
};

class Node_Index
{
	Node_info **index;
	int size;
	public:
		Node_Index(int n);
		~Node_Index();
		void set_offset(int node,int offset);
		void set_last_offset(int node,int last_offset);
		int get_offset(int node);
		int get_last_offset(int node);
		int exists_node(int node);
		int get_size();
		int get_neighbors(int node);
		void set_neighbors(int node);
};

#endif
