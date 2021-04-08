#ifndef GRAPH_UTILS_ALGS
#define GRAPH_UTILS_ALGS

#include "graph_utils.hh"
#include "graph_struct.hh"
#include <optional>
#include <map>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <queue>
#include <utility>

#pragma region SST

/* Computes the shortest spanning tree using Kruskal's algorithm 
    Parameters:
        -graph: a reference to the original graph
    Returns: an optional containing the sst graph if the operation was succesful, otherwise an empty one */
template <typename N, typename E>
std::optional<Graph<N,E>> compute_SST_Kruskal(Graph<N,E> const& graph)
{
    if(myGraphUtils::isDirect(graph) && graph.getNumNodes() > 0) {
        return {};
    }
    // We don't need the make the tree direct since the algorithm does not need to navigate the graph
    int const numNodes = graph.getNumNodes(), numEdges = graph.getNumEdges();
    int k = 0;
    Graph<N,E> copy;
    copy = graph;
    Graph<N,E> sst;
    // The map that stores under which sub-graph a node resides
    std::unordered_map<int, int> nodeTags;
    std::vector<Edge<E>> orderedEdges = copy.getEdges();
    std::sort(orderedEdges.begin(), orderedEdges.end());
    //Initialing nodeTags: each node is assigned to a different sub-graph
    for (Node<N,E> const& n : copy.getNodes())
    {
        nodeTags[n.getId()] = n.getId();
    }
    
    for (Edge<E> const& e : orderedEdges)
    {
        Node<N,E> & from = copy.getNode(e.getFrom());
        Node<N,E> & to = copy.getNode(e.getTo());
        int fromTag = nodeTags[from.getId()];
        int toTag = nodeTags[to.getId()];
        // If the nodes at the two extremes of the edge are from a different sub-graph, we add the the edge
        // and the nodes to the SST
        if (fromTag != toTag) {
            k++;
            sst.addNode(from);
            sst.addNode(to);
            sst.addEdge(fromTag, toTag, e.getCost(), e.isBidirectional());
            // Change the tag of every node which was part of the sub-graph with the tag "toTag"
            for (std::pair<int,int> tag : nodeTags) {
                if (tag.second == toTag) {
                    nodeTags[tag.first] = fromTag;
                }
            }
            // Exit if the sst contains numNodes - 1 nodes
            if (k >= numNodes - 1) {
                break;
            }
        }
    }
    // If the sst does not have numNodes - 1 nodes, the algorithm has failes to find the SST
    if(k < numNodes - 1) {
        return {};
    }
    return {sst};
}

#include <iostream>

/* Computes the shortest spanning tree using Prim's algorithm 
    Parameters:
        -graph: a reference to the original graph
    Returns: an optional containing the sst graph if the operation was succesful, otherwise an empty one */
template <typename N, typename E>
std::optional<Graph<N,E>> compute_SST_Prim(Graph<N,E> const& graph) {
    //Pred map is not needed in this implementation since we have defined edges as objects, with both ends stored in them as a reference

    //Check that the graph is direct
    if(myGraphUtils::isDirect(graph) && graph.getNumNodes() > 0 && !myGraphUtils::isConnected(graph)) {
        return {};
    }
    //A copy of the graph which will be turned to direct to make navigation easier and the code cleaner
    Graph<N,E> const& directg = myGraphUtils::makeDirect(graph);

    std::unordered_map<int, bool> flag; //Given a node id, tells if the node is part of the mst (true) or not (false)
    std::unordered_map<int, E> min; //Given a node id, tells the cost of the edge currently in use to reach it
    Graph<N,E> sst;
    Node<N,E> current = (*directg.getNodes().begin());
    std::priority_queue<Edge<E>, std::vector<Edge<E>>, std::greater<Edge<E>>> cutEdges;
    //Setting the initial state
    for (Edge<E> const& e : directg.getEdges())
    {
        //All nodes are labeled as not visited, their "min" value is initialized
        //to max if they are not connected to the starting node
        flag.insert({e.getFrom(), false});
        flag.insert({e.getTo(), false});
        min.insert({e.getTo(), std::numeric_limits<E>().max()});
    }

    for (Edge<E> const& e : current.getAdjacentEdges()) {
        if(e.getCost() < min.at(e.getTo())) {
            min.insert_or_assign(e.getTo(), e.getCost());
            cutEdges.push(e);
        }
    }
    sst.addNode(current);
    flag.insert_or_assign(current.getId(), true);
    min.insert_or_assign(current.getId(), 0);
    
    const int targetNumNodes = directg.getNumNodes() - 1;
    int k;
    for (k = 0; k < targetNumNodes && !cutEdges.empty(); k++) {
        Edge<E> bestEdge;
        do {
             bestEdge = cutEdges.top();
             cutEdges.pop();
        } while (flag.at(bestEdge.getTo()));
        current = directg.getNode(bestEdge.getTo());
        //Adding the target node to the the MST
        flag.insert_or_assign(current.getId(), true);
        sst.addNode(current);
        sst.addEdge(bestEdge.getFrom(), bestEdge.getTo(), bestEdge.getCost(), bestEdge.isBidirectional());
        //Updating the edges with new information from the newly added node
        for (Edge<E> const& e : current.getAdjacentEdges())
        {
            Node<N,E> const& target = directg.getNode(e.getTo());
            if(!flag.at(target.getId()) && min.at(target.getId()) > e.getCost()) {
                min.insert_or_assign(target.getId(), e.getCost());
                cutEdges.push(e);
            }
        }
    }
    if( k == targetNumNodes) {
        return sst;
    }
    return {};
}

#pragma endregion

#pragma region SHORTEST_PATH

/* Computes the shortest path using Dijkstra's algorithm 
    Parameters:
        -graph:  a reference to the original graph
        -fromId: the id of the starting node
        -toId:   the id of the end node
    Returns: an optional containing the shortest path if the operation was succesful, otherwise an empty graph*/
template <typename N, typename E>
std::optional<Graph<N,E>> compute_SP_Dijkstra(Graph<N,E> const& graph, int const fromId, int const toId) {
    //You should check for negative cycles, and that from and to are actually part of the same graph
    
    //label all nodes as not part of the path
    Graph<N,E> const& directg = myGraphUtils::makeDirect(graph);
    Graph<N,E> sp; //shortest path
    std::unordered_map<int, bool> flag;
    std::unordered_map<int, E> min;
    std::unordered_map<int, Edge<E>> prev; //for a given node id, it tells which node preceedes it and with what weight on the edge
    std::priority_queue<Edge<E>, std::vector<Edge<E>>, std::greater<Edge<E>>> edgesQueue;
    for (Node<N,E> const& n : directg.getNodes())
    {
        min.insert_or_assign(n.getId(), std::numeric_limits<E>().max());
        flag.insert_or_assign(n.getId(), false);
    }

    //Start from "from", add all of the edges starting from it to the queue
    Node<N,E> current = directg.getNode(fromId);
    for (auto const& e : current.getAdjacentEdges()) {
        if(min.at(e.getTo()) > e.getCost()) {
            min.insert_or_assign(e.getTo(), e.getCost());
            edgesQueue.push(e);
            prev.insert_or_assign(e.getTo(), e);
        }
    }
    min.insert_or_assign(current.getId(), 0);
    while(current.getId() != toId && !edgesQueue.empty()) {
        Edge<E> best;
        do {
            best = edgesQueue.top();
            edgesQueue.pop();
        } while(flag.at(best.getTo()));
        current = directg.getNode(best.getTo());

        for (Edge<E> const& e : current.getAdjacentEdges())
        {
            int const targetId = e.getTo();
            if(!flag.at(targetId) && e.getCost() + min.at(current.getId()) < min.at(targetId)) {
                min.insert_or_assign(targetId, e.getCost() + min.at(current.getId()));
                edgesQueue.push(e);
                prev.insert_or_assign(targetId, e);
            }
        }
        flag.at(current.getId()) = true;
    }
    //Building the path
    do {
        Edge<E> const& edge = prev.at(current.getId());
        Node<N,E> const& prevNode = directg.getNode(edge.getFrom());
        sp.addNode(current);
        sp.addNode(prevNode);
        sp.addEdge(edge.getFrom(), edge.getTo(), edge.getCost(), edge.isBidirectional());
        current = prevNode;
    } while(current.getId() != fromId);

    return {sp};
}


template<typename N, typename E>
std::optional<Graph<N,E>> compute_SP_Floyd_Warshall(Graph<N,E> const& graph) {
    std::map<std::pair<int,int>, E> min; //given idFrom and idTo (in the pair), returns current minimum path cost to reach it
    std::map<std::pair<int,int>,Edge<E>> prev; //given node id, returns the id of the preceding node in the path
    Graph<N,E> const& strippedg = myGraphUtils::stripRedundantEdges(myGraphUtils::makeDirect(graph), true);
    Graph<N,E> result;
    //Initialization
    for (Node<N,E> const& from : strippedg.getNodes())
    {
        result.addNode(from);
        std::vector<Edge<E>> const& neighbours = from.getAdjacentEdges();
        for (Node<N,E> const& to : strippedg.getNodes()) {
            min.insert_or_assign({from.getId(), to.getId()}, std::numeric_limits<E>().max());
            Edge<E> e(from.getId(), to.getId(), std::numeric_limits<E>().max(), false);
            prev.insert_or_assign({e.getFrom(), e.getTo()}, e);
        }
        for (Edge<E> const& e : neighbours)
        {
            prev.insert_or_assign({e.getFrom(), e.getTo()}, e);
            min.insert_or_assign({e.getFrom(), e.getTo()}, e.getCost());
        }
    }
    for (Node<N,E> const& h : strippedg.getNodes())
    {
        for (Node<N,E> const& i : strippedg.getNodes())
        {
            for (Node<N,E> const& j : strippedg.getNodes())
            {
                E const max = std::numeric_limits<E>().max();
                E const minIJ = min.at({i.getId(),j.getId()}), minIH = min.at({i.getId(),h.getId()}), minHJ = min.at({h.getId(),j.getId()});
                E const altMin = minIH == max || minHJ == max ? max : minIH + minHJ;
                if(minIJ > altMin) {
                    min.insert_or_assign({i.getId(), j.getId()}, altMin);
                    prev.insert_or_assign({i.getId(), j.getId()}, prev.at({h.getId(), j.getId()}));
                }
            }
        }    
    }
    for (auto const& pair : prev) {
        Edge<E> const& e = pair.second;
        if(e.getCost() < 0) {
            return {};
        }
        result.addEdge(e.getFrom(), e.getTo(), e.getCost(), e.isBidirectional());
    }
    return {result};

}

#pragma endregion



#endif