#include <iostream>
#include <set>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include "move_pixels.hpp"

constexpr double eps_coeff{0.1};
constexpr double delta_t{0.1};
constexpr double moving_limit{0.01};

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

class EdgeFilter
{
public:
  EdgeFilter() {}
  template <typename Container>
  EdgeFilter(const Container &edges)
      : edges{edges.begin(), edges.end()} {}
  bool operator()(const EdgeDescriptor &edge) const
  {
    return edges.count(edge) > 0;
  }

private:
  std::set<EdgeDescriptor> edges;
};

Graph pixels2Graph(const std::vector<movePixels::Pixel> &pixels)
{
  Graph graph;
  point::Map<Vertex> map;
  for (const auto &p : pixels) {
    const auto desc = boost::add_vertex(graph);
    graph[desc].p = p();
    graph[desc].length = 0;
    graph[desc].thickness = p.traveledDistance();
    graph[desc].desc = desc;
    map.push(graph[desc]);
  }

  const auto vertices = boost::vertices(graph);
  for (auto it = vertices.first; it != vertices.second; it++) {
    const auto &v = graph[*it];
    for (const auto &u : map.find(v(), 1)) {
      if (v.desc == u.desc)
        continue;
      const auto res = boost::add_edge(v.desc, u.desc, graph);
      if (res.second)
        boost::put(boost::edge_weight, graph, res.first, (u() - v()).norm());
    }
  }

  return graph;
}

void printSVG(const Graph &graph)
{
  std::cout << "<?xml version=\"1.0\"?>" << std::endl;
  std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  std::cout << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
  std::cout << "<g stroke=\"black\">" << std::endl;

  const auto edges = boost::edges(graph);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto &v = graph[boost::source(*it, graph)];
    const auto &u = graph[boost::target(*it, graph)];
    std::cout << "<line x1=\"" << v()(0) << "\" y1=\"" << v()(1) << "\" x2=\"" << u()(0) << "\" y2=\"" << u()(1) << "\"/>" << std::endl;
  }

  std::cout << "</g>" << std::endl;
  std::cout << "</svg>" << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "no file specified" << std::endl;
    return 1;
  }
  cv::Mat raw = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if (raw.empty()) {
    std::cerr << "can't open file '" << argv[1] << "'" << std::endl;
    return 1;
  }
  cv::Mat src;
  raw.convertTo(src, CV_64F, -1.0 / 256, 1.0);

  movePixels::PixelSet pixelSet{src, eps_coeff, delta_t};
  std::cerr << "extract " << pixelSet.countActivePixels() << " pixels" << std::endl;
  std::cerr << "moving pixels ... ";
  pixelSet.movePixels(pixelSet.countActivePixels() * moving_limit);
  std::cerr << "done" << std::endl;
  const auto pixels = pixelSet.getValidPixels();

  auto graph = pixels2Graph(pixels);

  std::vector<EdgeDescriptor> mst_edges;
  boost::kruskal_minimum_spanning_tree(graph, std::back_inserter(mst_edges));

  boost::filtered_graph<Graph, EdgeFilter> _mst{graph, mst_edges};
  Graph mst;
  boost::copy_graph(_mst, mst);

  const auto vertices = boost::vertices(mst);
  bool update;
  do {
    update = false;
    for (auto it = vertices.first; it != vertices.second; it++) {
      if (boost::out_degree(*it, mst) != 1)
        continue;
      if (mst[*it].length > mst[*it].thickness)
        continue;

      const auto &edge = *(boost::out_edges(*it, mst).first);

      auto &v = mst[*it];
      auto &u = mst[boost::source(edge, mst) != *it ? boost::source(edge, mst) : boost::target(edge, mst)];
      const auto r = (v() - u()).norm();

      if (v.length + r > u.length)
        u.length = v.length + r;

      boost::remove_edge(edge, mst);
      update = true;
    }
  } while (update);

  printSVG(mst);

  return 0;
}
