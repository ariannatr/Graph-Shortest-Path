OBJS = main.o graph.o list_node.o buffer.o node_index.o queue.o other_functions.o info_for_scc_build.o stack.o scc.o cc_index.o job_scheduler.o
SOURCE = main.cpp graph.cpp list_node.cpp buffer.cpp node_index.cpp queue.cpp other_functions.cpp info_for_scc_build.cpp stack.cpp scc.cpp cc_index.cpp   job_scheduler.cpp
HEADER = main.h graph.h list_node.h buffer.h node_index.h queue.h other_functions.h info_for_scc_build.h stack.h scc.h cc_index.h   job_scheduler.h
OUT = graph
CC = g++ -std=c++11 -pthread
FLAGS = -g -c -Wall -Wextra  -O2

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

graph.o: graph.cpp
	$(CC) $(FLAGS) graph.cpp

list_node.o: list_node.cpp
	$(CC) $(FLAGS) list_node.cpp

buffer.o: buffer.cpp
	$(CC) $(FLAGS) buffer.cpp

node_index.o: node_index.cpp
	$(CC) $(FLAGS) node_index.cpp

queue.o: queue.cpp
	$(CC) $(FLAGS) queue.cpp

other_functions.o: other_functions.cpp
	$(CC) $(FLAGS) other_functions.cpp

info_for_scc_build.o: info_for_scc_build.cpp
	$(CC) $(FLAGS) info_for_scc_build.cpp

stack.o: stack.cpp
	$(CC) $(FLAGS) stack.cpp

scc.o: scc.cpp
	$(CC) $(FLAGS) scc.cpp

cc_index.o: cc_index.cpp
	$(CC) $(FLAGS) cc_index.cpp

job_scheduler.o: job_scheduler.cpp
	$(CC) $(FLAGS) job_scheduler.cpp

clean :
	rm -f $(OBJS) $(OUT)

count :
	wc $(SOURCE) $(HEADER)
