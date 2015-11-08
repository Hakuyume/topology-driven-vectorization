#include <boost/graph/copy.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
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

std::vector<VertexDescriptor> getShortestPath(const Graph &graph, const VertexDescriptor &v_desc, const VertexDescriptor &u_desc)
{
  using IndexMap = boost::property_map<Graph, boost::vertex_index_t>::type;
  using PredecessorMap = boost::iterator_property_map<VertexDescriptor *, IndexMap, VertexDescriptor, VertexDescriptor &>;
  using DistanceMap = boost::iterator_property_map<double *, IndexMap, double, double &>;

  std::vector<VertexDescriptor> predecessors(boost::num_vertices(graph));
  std::vector<double> distances(boost::num_vertices(graph));

  IndexMap indexMap = boost::get(boost::vertex_index, graph);
  PredecessorMap predecessorMap{&predecessors[0], indexMap};
  DistanceMap distanceMap{&distances[0], indexMap};

  boost::dijkstra_shortest_paths(graph, u_desc, boost::distance_map(distanceMap).predecessor_map(predecessorMap));

  std::vector<VertexDescriptor> path;
  for (auto w_desc = v_desc; w_desc != u_desc; w_desc = predecessorMap[w_desc])
    path.push_back(w_desc);
  path.push_back(u_desc);

  return path;
}

std::vector<std::vector<VertexDescriptor>> extractTopology::getPaths(Graph &graph)
{
  Graph skeleton;
  boost::copy_graph(graph, skeleton);
  skeletonize(skeleton);

  std::vector<std::vector<VertexDescriptor>> paths;

  const auto edges = boost::edges(skeleton);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto v_desc = boost::source(*it, skeleton);
    const auto u_desc = boost::target(*it, skeleton);
    paths.push_back(getShortestPath(graph, v_desc, u_desc));
  }

  return paths;
}
