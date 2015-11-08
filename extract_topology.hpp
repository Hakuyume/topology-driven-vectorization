#pragma once

#include <set>
#include <boost/graph/adjacency_list.hpp>
#include "point.hpp"

namespace extractTopology
{
class Vertex;
using Graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    Vertex,
                                    boost::property<boost::edge_weight_t, double>>;
using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;

class Vertex
{
public:
  Vertex();
  template <typename T>
  Vertex(const T &p, const VertexDescriptor &desc)
      : p{p()}, t{p.thickness()}, l{0}, v_desc{desc} {}
  point::Vector operator()() const;
  double thickness() const;
  VertexDescriptor desc() const;
  bool isRemovable(const Graph &graph) const;
  bool updateLength(const Vertex &v);

private:
  point::Vector p;
  double t, l;
  VertexDescriptor v_desc;
};

template <typename Container>
Graph createGraph(const Container &points)
{
  Graph graph;
  point::Map<Vertex> map;

  for (const auto &p : points) {
    const auto desc = boost::add_vertex(graph);
    auto &v = graph[desc];
    v = Vertex(p, desc);
    for (const auto &u : map.find(v(), 1))
      boost::add_edge(v.desc(), u.desc(), (u() - v()).norm(), graph);
    map.push(v);
  }

  return graph;
}

Graph getMST(const Graph &graph);
void pruneBranches(Graph &graph);
void skeletonize(Graph &graph);
std::vector<std::vector<Vertex>> getPaths(Graph &graph);
}
