#ifndef GRAPH_UTILS
#define GRAPH_UTILS

#include "graph_struct.hh"
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <optional>
#include <unordered_set>
#include <vector>


template <typename N, typename E>
static bool addNodes(Graph<N,E>& graph, int const dim, std::istream& file, int numNodes);
template <typename N, typename E>
static bool addEdges(Graph<N,E>& graph, std::istream& file, int numEdges);

namespace myGraphUtils
{   
    template <typename N, typename E> 
    bool initGraph(std::string const filename, Graph<N,E>& graph);
    template <typename N, typename E>
    bool const isDirect(Graph<N,E> const& graph);
    template <typename N, typename E>
    bool const isConnected(Graph<N,E> const& graph);
    template <typename N, typename E>
    Graph<N,E> const makeDirect(Graph<N,E> const& graph);
    template <typename N, typename E>
    void drawGraph(Graph<N,E> const& graph);
    template <typename N, typename E>
    Graph<N,E> const stripRedundantEdges(Graph<N,E> const& graph, bool const takeMin);
}

template <typename N, typename E>
void myGraphUtils::drawGraph(Graph<N,E> const& graph) {
    for (Edge<E> const& e : graph.getEdges())
    {
        std::string junction = e.isBidirectional() ? " <-> " : " -> ";
        std::cout << e.getFrom() << junction << e.getTo() << " cost: " << e.getCost() << std::endl;
    }
}

template <typename N, typename E>
bool myGraphUtils::initGraph(std::string const filename, Graph<N,E>& graph)
{
    std::string line;
    std::ifstream file(filename);
    unsigned int dim, num_Edges, num_Nodes;
    if (!file.is_open()) {
        return false;
    }
    file >> dim >> num_Nodes >> num_Edges;
    std::getline(file, line);
    if(!addNodes(graph, dim, file, num_Nodes)) {
        return false;
    }
    if(!addEdges(graph, file, num_Edges)) {
        return false;
    }
    return true;
}

template <typename N, typename E>
static bool addNodes(Graph<N,E>& graph, int const dim, std::istream& file, int numNodes) {
    std::string line, word;
    while (numNodes-- > 0)
    {
        std::vector<float> coords;
        int id;
        N cost;
        std::getline(file, line);
        std::stringstream ss(line);
        if(!(ss >> word)) {
            return false;
        }
        id = std::stoi(word);
        for (size_t i = 0; i < dim; i++)
        {
            if(!(ss >> word)) {
                return false;
            }
            coords.push_back(std::stof(word));
        }
        if(!(ss >> cost)) {
            return false;
        }
        if(!graph.addNode(Node<N,E>(id, coords, cost))) {
            return false;
        }
    }
    return true;
}

template <typename N, typename E>
static bool addEdges(Graph<N,E>& graph, std::istream& file, int numEdges) {
    std::string line;
    while (numEdges-- > 0)
    {
        //std::unordered_set<Node> nodes = graph.getNodes();
        int fromid, toid;
        E cost;
        bool bidirectional;
        file >> fromid >> toid >> cost >> bidirectional;
        graph.addEdge(fromid, toid, cost, bidirectional);
        std::getline(file, line);
    }
    return true;
}

template <typename N, typename E>
Graph<N,E> const myGraphUtils::makeDirect(Graph<N,E> const& graph) {
    Graph<N,E> copy(graph);
    for (Edge<E> e : graph.getEdges()) {
        if(e.isBidirectional() && e.getFrom() != e.getTo()) {
            copy.addEdge(e.getTo(), e.getFrom(), e.getCost(), e.isBidirectional());
        }
    }
    return copy;
}


template <typename N, typename E>
bool const myGraphUtils::isConnected(Graph<N,E> const& graph) {
    int const target = graph.getNumNodes();
    Graph<N,E> copy;
    copy = graph;
    myGraphUtils::makeDirect(copy);
    std::unordered_map<int, bool> flag; //Given node id, it tells whether the node is part (connected) of the graph
    std::queue<Node<N,E>> nodes;
    Node<N,E> current = (*copy.getNodes().begin());
    for (auto const& n : copy.getNodes())
    {
        flag[n.getId()] = false;
    }
    flag[current.getId()] = true;
    nodes.push(current);
    int k = 1; //Number of nodes visited
    while(!nodes.empty() && k < target) {
        current = nodes.front();
        nodes.pop();
        
        for (Edge<E> const& e : current.getAdjacentEdges())
        {
            Node<N,E> const& n = graph.getNode(e.getTo());
            if(!flag[n.getId()]) {
                nodes.push(n);
                flag[n.getId()] = true;
                k++;
            }
        }
    }
    return k == target;
}

template <typename N, typename E>
bool const myGraphUtils::isDirect(Graph<N,E> const& graph) {
    for (Edge<E> const e : graph.getEdges())
    {
        if(!e.isBidirectional()) {
            return true;
        }
    }
    return false;
}

template <typename N, typename E>
Graph<N,E> const myGraphUtils::stripRedundantEdges(Graph<N,E> const& graph, bool const takeMin) {
    Graph<N,E> strippedGraph;
    for (Node<N,E> const& node : graph.getNodes()) {
        strippedGraph.addNode(node);
    }
    for (Node<N,E> const& node : graph.getNodes())
    {
        std::unordered_map<int, Edge<E>> best;
        for (Edge<E> edge : node.getAdjacentEdges())
        {
            auto previous = best.find(edge.getTo());
            if(previous == best.end()) {
                best.insert_or_assign(edge.getTo(), edge);
            } else {
                Edge<E> bestEdge;
                if(takeMin) {
                    bestEdge = (*previous).second < edge ? (*previous).second : edge;
                } else {
                    bestEdge = (*previous).second > edge ? (*previous).second : edge;
                }
                best.insert_or_assign(bestEdge.getTo(), bestEdge);
            }
        }
        for (auto const& pair : best)
        {
            Edge<E> e = pair.second;

            strippedGraph.addEdge(e.getFrom(), e.getTo(), e.getCost(), e.isBidirectional());
        }
    }
    return strippedGraph;
}
#endif