#include <boost/graph/copy.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include "extract_topology.hpp"

using namespace extractTopology;

void extractTopology::addVertex(Graph &graph, const point::Vector &p, const double &thickness)
{
  const auto desc = boost::add_vertex(graph);
  graph[desc].p = p;
  graph[desc].length = 0;
  graph[desc].thickness = thickness;
  graph[desc].desc = desc;
}

void extractTopology::addEdges(Graph &graph)
{
  point::Map<Vertex> map;
  const auto vertices = boost::vertices(graph);
  for (auto it = vertices.first; it != vertices.second; it++) {
    const auto &v = graph[*it];
    for (const auto &u : map.find(v(), 1)) {
      const auto edge = boost::add_edge(v.desc, u.desc, graph).first;
      boost::put(boost::edge_weight, graph, edge, (u() - v()).norm());
    }
    map.push(v);
  }
}

Graph extractTopology::getMST(const Graph &graph)
{
  std::vector<EdgeDescriptor> mst_edges;
  boost::kruskal_minimum_spanning_tree(graph, std::back_inserter(mst_edges));
  boost::filtered_graph<Graph, EdgeFilter> _mst{graph, mst_edges};
  Graph mst;
  boost::copy_graph(_mst, mst);
  return mst;
}

void pruneBranch(Graph &graph, const VertexDescriptor &v_desc)
{
  auto &v = graph[v_desc];
  if (v.length > v.thickness)
    return;
  if (boost::out_degree(v_desc, graph) != 1)
    return;
  const auto &edge = *(boost::out_edges(v_desc, graph).first);
  const auto u_desc = boost::source(edge, graph) != v_desc
                          ? boost::source(edge, graph)
                          : boost::target(edge, graph);
  auto &u = graph[u_desc];
  const auto r = (v() - u()).norm();
  if (v.length + r > u.length)
    u.length = v.length + r;
  boost::remove_edge(edge, graph);
  pruneBranch(graph, u_desc);
}

void extractTopology::pruneBranches(Graph &graph)
{
  const auto vertices = boost::vertices(graph);
  for (auto it = vertices.first; it != vertices.second; it++)
    pruneBranch(graph, *it);
}
