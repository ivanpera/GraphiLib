#ifndef GRAPH_BUILDER
#define GRAPH_BUILDER

#include "graph_struct.hh"
#include <random>
#include <optional>
#include <sstream>
#include <iostream>
#include <fstream>

enum DirectMode {ALLDIRECT, ALLBIDIRECTIONAL, MIXED};

template <typename N, typename E>
class GraphBuilder {
    private:
        bool weightedNodes = false, weightedEdges = true, acyclic = false, connected = true;
        E maxEdgeWeight = 100, minEdgeWeight = 0;
        N maxNodeWeight = 100, minNodeWeight = 0;
        DirectMode directMode = ALLBIDIRECTIONAL;
        unsigned int numNodes = 10, numEdges = 15;
        void writeToFile(Graph<N,E> const& graph) const;
    public:
        GraphBuilder() = default;
        GraphBuilder & setNumNodes(const unsigned int value);
        GraphBuilder & setNumEdges(const unsigned int value);
        GraphBuilder & setWeightedNodes(const bool value);
        GraphBuilder & setWeightedEdges(const bool value);
        GraphBuilder & setAcyclic(const bool value);
        GraphBuilder & setConnected(const bool value);
        GraphBuilder & setDirect(const DirectMode value);
        GraphBuilder & setNodeWeightLimits(const N min, const N max);
        GraphBuilder & setEdgeWeightLimits(const E min, const E max);
        std::optional<Graph<N,E>> build();
};

template <typename N,typename E>
void GraphBuilder<N,E>::writeToFile(Graph<N,E> const& graph) const {
    std::string line;
    std::ofstream file("last.graph");
    //TODO: remove hardcoded dimensions
    file << "2 " << numNodes << " " << numEdges << std::endl;
    for (Node<N,E> const& node : graph.getNodes())
    {
        file << node.getId() << " ";
        for (auto &&i : node.getCoords())
        {
            file << i << " ";
        }
        file << " " << node.getCost() << std::endl;
    }
    for (auto &&i : graph.getEdges())
    {
        file << i.getFrom() << " " << i.getTo() << " " << i.getCost() << " " << i.isBidirectional() << std::endl;
    }
    file.close();
}

template <typename C>
C const getRandomCost(C const minWeigth, C const maxWeigth) {
    std::random_device rd;
    std::mt19937 gen(rd());
    C cost;
    if( std::is_same_v<C, int>) {
        std::uniform_int_distribution<int> costGen(minWeigth, maxWeigth);
        cost = costGen(gen);
    } else {
        std::uniform_real_distribution<float> costGen(minWeigth, maxWeigth);
        cost = costGen(gen);
    }
    //TODO: implement bidirectional, currently hardcoded to true
    return cost;
}

template <typename N, typename E>
std::optional<Graph<N,E>> GraphBuilder<N,E>::build() {
    //Do a random walk generating a node each step, then add random remaining edges
    Graph<N,E> graph;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> connect(0.0, 1.0);
    std::uniform_real_distribution<float> coords(-100, 101);
    
    if(this->connected && (this->numEdges < this->numNodes - 1)) {
        return {};
    }
    for (int i = 0; i < this->numNodes;)
    {
        //NOTE: as of now, we are hardcoded to two dimensions and costs are ignored
        Node<N,E> n(i, {coords(gen), coords(gen)}, 0);
        if(graph.addNode(n)) {
            i++;
        }
    }
    //For connected graphs. For unconnected\random ones, just connect random nodes
    if(this->connected) {
        std::vector<int> unvisitedNodes;
        for (size_t i = 0; i < numNodes; i++)
        {
            unvisitedNodes.push_back(i);
        }
        
        int currentId = unvisitedNodes.at(0);
        unvisitedNodes.erase(unvisitedNodes.begin());
        while(!unvisitedNodes.empty()) {
            std::uniform_int_distribution<int> offsetGen(0, unvisitedNodes.size() - 1);
            int offset = offsetGen(gen);
            int targetId = unvisitedNodes.at(offset);
            //TODO:implement cost and bidir
            graph.addEdge(currentId, targetId, getRandomCost<E>(this->minEdgeWeight, this->maxEdgeWeight), true); 
            currentId = targetId;
            unvisitedNodes.erase(unvisitedNodes.begin() + offset);
        }
    }
    //Filling remaining edges (or creating a fully random graph)
    for (int i = graph.getNumEdges(); i < this->numEdges; i++)
    {
        std::uniform_int_distribution<int> nodeId(0, this->numNodes - 1);
        int idFrom = nodeId(gen), idTo = nodeId(gen);
        graph.addEdge(idFrom, idTo, getRandomCost<E>(this->minEdgeWeight, this->maxEdgeWeight), true);
    }
    
    writeToFile(graph);

    return {graph};
}

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setNumEdges(const unsigned int value) {
    this->numEdges = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setNumNodes(const unsigned int value) {
    this->numNodes = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setConnected(const bool value) {
    this->connected = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setAcyclic(const bool value) {
    this->acyclic = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setDirect(const DirectMode value) {
    this->directMode = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setWeightedEdges(const bool value) {
    this->weightedEdges = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E>& GraphBuilder<N,E>::setWeightedNodes(const bool value) {
    this->weightedNodes = value;
    return *this;
};

template <typename N, typename E>
GraphBuilder<N,E> & GraphBuilder<N,E>::setNodeWeightLimits(const N min, const N max) {
    this->minNodeWeight = min; this->maxNodeWeight = max;
    return *this;
}

template <typename N, typename E>
GraphBuilder<N,E> & GraphBuilder<N,E>::setEdgeWeightLimits(const E min, const E max) {
    this->minEdgeWeight = min; this->maxEdgeWeight = max;
    return *this;
}

#endif