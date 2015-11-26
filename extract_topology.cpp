#include <boost/graph/copy.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "extract_topology.hpp"

using namespace extractTopology;

Vertex::Vertex()
{
}

Vertex::Vertex(const point::Point &p, const VertexDescriptor &desc)
    : point::Point{p}, l{0}, v_desc{desc}
{
}

VertexDescriptor Vertex::desc() const
{
  return v_desc;
}

bool Vertex::isRemovable(const Graph &graph, const double &length) const
{
  return l <= (length > 0 ? length : thick) and boost::out_degree(v_desc, graph) == 1;
}

bool Vertex::updateLength(const Vertex &v)
{
  const auto r = (v.pos - pos).norm();
  if (v.l + r <= l)
    return false;
  l = v.l + r;
  return true;
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

void pruneBranch(Graph &graph, const VertexDescriptor &v_desc, const double &length)
{
  auto &v = graph[v_desc];
  if (not v.isRemovable(graph, length))
    return;
  const auto &edge = *(boost::out_edges(v_desc, graph).first);
  const auto u_desc = boost::target(edge, graph);
  auto &u = graph[u_desc];
  u.updateLength(v);
  boost::remove_edge(edge, graph);
  pruneBranch(graph, u_desc, length);
}

void extractTopology::pruneBranches(Graph &graph, const double &length)
{
  const auto vertices = boost::vertices(graph);
  for (auto it = vertices.first; it != vertices.second; it++)
    pruneBranch(graph, *it, length);
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

Path getShortestPath(const Graph &graph, const VertexDescriptor &v_desc, const VertexDescriptor &u_desc)
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

  Path path;
  for (auto w_desc = v_desc; w_desc != u_desc; w_desc = predecessorMap[w_desc])
    path.push_back(graph[w_desc]);
  path.push_back(graph[u_desc]);

  return path;
}

std::vector<std::pair<Vertex, Vertex>> extractTopology::getEdges(Graph &graph)
{
  std::vector<std::pair<Vertex, Vertex>> edge_pairs;
  const auto edges = boost::edges(graph);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto v_desc = boost::source(*it, graph);
    const auto u_desc = boost::target(*it, graph);
    edge_pairs.push_back({graph[v_desc], graph[u_desc]});
  }
  return edge_pairs;
}

std::vector<Path> extractTopology::getPaths(Graph &graph)
{
  Graph skeleton;
  boost::copy_graph(graph, skeleton);
  skeletonize(skeleton);

  std::vector<Path> paths;

  const auto edges = boost::edges(skeleton);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto v_desc = boost::source(*it, skeleton);
    const auto u_desc = boost::target(*it, skeleton);
    paths.push_back(getShortestPath(graph, v_desc, u_desc));
  }

  return paths;
}
