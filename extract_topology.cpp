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
    for (const auto &u : map.find(v(), 1))
      boost::add_edge(v.desc, u.desc, (u() - v()).norm(), graph);
    map.push(v);
  }
}

Graph extractTopology::getMST(const Graph &graph)
{
  struct EdgeFilter {
    EdgeFilter() {}
    EdgeFilter(const std::vector<EdgeDescriptor> &edges)
        : edges{edges.begin(), edges.end()} {}
    bool operator()(const EdgeDescriptor &edge) const
    {
      return edges.count(edge) > 0;
    }
    std::set<EdgeDescriptor> edges;
  };

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
  const auto u_desc = boost::target(edge, graph);
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

void extractTopology::skeletonize(Graph &graph)
{
  const auto vertices = boost::vertices(graph);
  for (auto it = vertices.first; it != vertices.second; it++) {
    if (boost::out_degree(*it, graph) != 2)
      continue;
    const auto &edge0 = *(boost::out_edges(*it, graph).first);
    const auto u0_desc = boost::target(edge0, graph);
    const auto &edge1 = *(boost::out_edges(*it, graph).first + 1);
    const auto u1_desc = boost::target(edge1, graph);
    boost::remove_edge(edge0, graph);
    boost::remove_edge(edge1, graph);
    boost::add_edge(u0_desc, u1_desc, graph);
  }
}
