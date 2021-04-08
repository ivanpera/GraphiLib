#ifndef GRAPH_STRUCT
#define GRAPH_STRUCT

#include <vector>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
template <typename E>
class Edge {
    private:
        E cost;
        bool bidirectional;
        int from, to;
    public:
        Edge() = default;
        Edge(int const idFrom, int const idTo, E const cost = 0, bool const bidirectional = true) : from(idFrom), to(idTo), cost(cost), bidirectional(bidirectional) {};
        int const getFrom() const;
        int const getTo() const;
        E const getCost() const;
        bool const isBidirectional() const;
        bool operator<(Edge<E> const& other) const;
        bool operator>(Edge<E> const& other) const;
        bool operator==(Edge<E> const& other) const;
};


template <typename N, typename E>
class Node
{
    private:
        N cost;
        int id;
        std::vector<float> coords;
        std::vector<Edge<E>> adjacentEdges;
    public:
        Node() = default;
        Node(int id, std::vector<float> const& coords, N cost = 0) : cost(cost), id(id), coords(coords) {}
        std::vector<float> const& getCoords() const;
        N const getCost() const;
        int const getId() const;
        void addAdjacentEdge(Edge<E> const& edge) {
            adjacentEdges.push_back(edge);
        }
        std::vector<Edge<E>> const& getAdjacentEdges() const{
            return adjacentEdges;
        }
        bool operator ==(Node<N,E> const& other) const {
            return (/*getCoords() == other.getCoords() || */id == other.getId());
        }
        bool operator !=(Node<N,E> const& other) const {
            return !(*this == other);
        }
};

template <typename N, typename E>
class Graph {
    private:
        std::unordered_map<int,Node<N,E>> nodes;
        std::vector<Edge<E>> edges;
        unsigned int num_edges;
        unsigned int num_nodes;
    public:
        Graph(unsigned int const num_edges = 0, unsigned int const num_nodes = 0) : num_edges(num_edges), num_nodes(num_nodes) {};
        Node<N,E> const& getNode(int const id) const;
        Node<N,E> & getNode(int const id);
        int getNumNodes() const;
        int getNumEdges() const;
        std::vector<Node<N,E>> const getNodes() const;
        std::vector<Edge<E>> const& getEdges() const;
        bool addNode(Node<N,E> const& node);
        void addEdge(int const fromId, int const toId, E const cost, bool const bidirectional);
};

template <typename N, typename E>
std::vector<float> const& Node<N,E>::getCoords() const {
    return this->coords;
}

template <typename E>
bool Edge<E>::operator<(Edge<E> const& other) const {
    return this->cost < other.cost;
}

template <typename E>
bool Edge<E>::operator==(Edge<E> const& other) const {
    return  this->getCost() == other.getCost()  &&
            this->getFrom() == other.getTo()    &&
            this->getTo() == other.getTo()      &&
            this->isBidirectional() == other.isBidirectional();
}

template <typename E>
bool Edge<E>::operator>(Edge<E> const& other) const {
    return !((*this) < other);
}

template <typename N, typename E>
int const Node<N,E>::getId() const {
    return this->id;
}

template <typename N, typename E>
N const Node<N,E>::getCost() const {
    return this->cost;
}

template <typename E>
int const Edge<E>::getFrom() const{
    return this->from;
}

template <typename E>
int const Edge<E>::getTo() const{
    return this->to;
}

template <typename E>
E const Edge<E>::getCost() const{
    return this->cost;
}

template <typename E>
bool const Edge<E>::isBidirectional() const{
    return this->bidirectional;
}

template <typename N, typename E>
Node<N,E> const& Graph<N,E>::getNode(int const id) const{
    return nodes.at(id);
}

template <typename N, typename E>
Node<N,E> & Graph<N,E>::getNode(int const id) {
    return nodes.at(id);
}

template <typename N, typename E>
int Graph<N,E>::getNumNodes() const {
    return this->num_nodes;
}

template <typename N, typename E>
int Graph<N,E>::getNumEdges() const {
    return this->num_edges;
}


template <typename N, typename E>
void Graph<N,E>::addEdge(int const fromId, int const toId, E const cost, bool const bidirectional) {
    Node<N,E> & from = this->getNode(fromId), &to = this->getNode(toId);
    Edge<E> const edge(fromId, toId, cost, bidirectional);
    from.addAdjacentEdge(edge);
    edges.push_back(edge);
    num_edges++;
};

template <typename N, typename E>
bool Graph<N,E>::addNode(Node<N,E> const& node) {
    bool const inserted = this->nodes.try_emplace(node.getId(), node).second;
    if(inserted) {
        num_nodes++;
    }
    return inserted;
};

template <typename N, typename E>
std::vector<Node<N,E>> const Graph<N,E>::getNodes() const {
    std::vector<Node<N,E>> result;
    //result.reserve(this->num_nodes * sizeof(Node<N,E>));
    for (auto const& pair : this->nodes) {
        result.push_back(pair.second);
    }

    return result;
}

template <typename N, typename E>
std::vector<Edge<E>> const& Graph<N,E>::getEdges() const {
    return edges;
}


#endif