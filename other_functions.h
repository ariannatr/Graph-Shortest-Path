#ifndef OTHERF_H 
#define OTHERF_H
#include "graph.h"
#include "node_index.h"
#include "scc.h"
#include "buffer.h"
#include "stack.h"
#include "queue.h"
#include "info_for_scc_build.h"
#include "cc_index.h"

void tarjan(Node_Index* table_in,Node_Index* table_out,Buffer* buffer_out,Stronglycc* Scc);
void fix_scc_neighbors_and_grail(Stronglycc* Scc,Node_Index * table_out,Buffer* buffer_out);
int grail_question(Stronglycc* Scc,uint32_t node1,uint32_t node2);

void grail_1(Stronglycc* Scc,Node_Index* hyper_index,Buffer* hyper_buffer);
void grail_2(Stronglycc* Scc,Node_Index* hyper_index,Buffer* hyper_buffer);

void dfs(uint32_t start,Stronglycc* Scc,uint32_t * order,Node_Index* hyper_index,Buffer* hyper_buffer,int which);
int bfs(uint32_t start,Graph* G,int* marked_out,int* marked_in,CC_Index * cc_dynamic,uint32_t cc_num,int check);

void iterative_strongconnect(uint32_t v,Node_Index * table_out,Buffer* buffer_out,Stronglycc* Scc,Stack* S,int * index,Component_Index* comp_table);
void update_neighbors(Stronglycc* Scc,Node_Index* table_out,Buffer* buffer_out,Node_Index* hyper_index,Buffer* hyper_buffer);

int the_bbfs(uint32_t start,uint32_t end,Stronglycc* Scc,Node_Index* table_in,Node_Index* table_out,Buffer* buffer_out,Buffer* buffer_in,int* depth,int* marked_out,int* marked_in,int check,Queue<uint32_t>* queue_out,Queue<uint32_t>* queue_in,uint32_t version);

#endif 
