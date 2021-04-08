#include "graph_utils.hh"
#include "graph_utils_algorithms.hh"
#include "graph_struct.hh"
#include "graph_utils_builder.hh"
#include <vector>
#include <iostream>
#include <algorithm>


#include <ctime>
int main(int argc, char** argv) 
{
    //TODO: Argument parsing, see below
    if(argc != 2) {
        //should parse arguments: -f <filename> OR nothing for random graph, additional -s parameter for saving the generated graph on a file
        std::cout << argv[0] << " usage <graph_file_name>" << std::endl;
        return 1;
    }
    GraphBuilder<int,int> gb;
    std::time_t start = std::time(nullptr);
    Graph<int,int> g = (gb.setNumNodes(5).setNumEdges(7).build()).value();
    std::time_t stop = std::time(nullptr);

    std::cout << "ORIGINAL GRAPH" << std::endl;
    myGraphUtils::drawGraph(g);
    std::cout << "Graph creation time: " << stop - start << " seconds" << std::endl;
    std::optional<Graph<int, int>> result = compute_SP_Floyd_Warshall(g);
    if(!result) {
        std::cout << "Graph not connected" << std::endl;
        return 0;
    }
    
    std::cout << "SP" << std::endl;
    myGraphUtils::drawGraph(result.value());
}
