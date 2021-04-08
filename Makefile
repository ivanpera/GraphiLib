EXE_NAME=kruskal
PARAMS=--std=c++17

all: 				kruskal graph_utils.hh graph_utils_algorithms.hh

kruskal: 			kruskal.o
	 			g++ kruskal.o -o ${EXE_NAME} ${PARAMS}

kruskal.o: 			kruskal.cpp
	 			g++ -c kruskal.cpp ${PARAMS}
 
.PHONY:				clean

clean:		
				rm -f *.o ${EXE_NAME}
