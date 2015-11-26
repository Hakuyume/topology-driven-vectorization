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
using Path = std::vector<Vertex>;

class Vertex : public point::Point
{
public:
  Vertex();
  Vertex(const point::Point &p, const VertexDescriptor &desc);
  VertexDescriptor desc() const;
  bool isRemovable(const Graph &graph, const double &length = 0) const;
  bool updateLength(const Vertex &v);

private:
  double l;
  VertexDescriptor v_desc;
};

template <class T>
Graph createGraph(const std::vector<T> &points)
{
  Graph graph;
  point::Map<Vertex> map;

  for (const auto &p : points) {
    if (map.find(p(), 0).size() > 0)
      continue;
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
void pruneBranches(Graph &graph, const double &length = 0);
void skeletonize(Graph &graph);
std::vector<std::pair<Vertex, Vertex>> getEdges(Graph &graph);
std::vector<Path> getPaths(Graph &graph);
}
