#pragma once

#include <set>
#include <boost/graph/adjacency_list.hpp>
#include "point.hpp"

namespace extractTopology
{
struct Vertex;
using Graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    Vertex,
                                    boost::property<boost::edge_weight_t, double>>;
using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;

struct Vertex {
  point::Vector p;
  double length, thickness;
  VertexDescriptor desc;
  point::Vector operator()() const { return p; }
};

void addVertex(Graph &graph, const point::Vector &p, const double &thickness);
void addEdges(Graph &graph);
Graph getMST(const Graph &graph);
void pruneBranches(Graph &graph);
void skeletonize(Graph &graph);
std::vector<std::vector<VertexDescriptor>> getPaths(Graph &graph);
}
