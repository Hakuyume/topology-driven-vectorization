#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <boost/graph/adjacency_list.hpp>
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
  size_t id;
  point::Vector p;
  point::Vector operator()() const { return p; }
};

Graph pixels2Graph(const std::vector<movePixels::Pixel> &pixels)
{
  Graph graph;
  point::Map<Vertex> map;
  for (const auto &p : pixels) {
    const auto desc = boost::add_vertex(graph);
    graph[desc].id = desc;
    graph[desc].p = p();
    map.push(graph[desc]);
  }

  const auto range = boost::vertices(graph);
  for (auto it = range.first; it != range.second; it++) {
    const auto &v = graph[*it];
    for (const auto &w : map.find(v(), 1)) {
      if (v.id == w.id)
        continue;
      const auto res = boost::add_edge(v.id, w.id, graph);
      if (res.second)
        boost::put(boost::edge_weight, graph, res.first, (w() - v()).norm());
    }
  }

  return graph;
}

Graph genMST(const Graph &graph)
{
  std::vector<EdgeDescriptor> mst_edges;
  boost::kruskal_minimum_spanning_tree(graph, std::back_inserter(mst_edges));

  Graph mst;
  const auto range = boost::vertices(graph);
  for (auto it = range.first; it != range.second; it++) {
    const auto desc = boost::add_vertex(mst);
    mst[desc] = graph[*it];
  }

  for (const auto &desc : mst_edges) {
    const auto res = boost::add_edge(boost::source(desc, graph), boost::target(desc, graph), mst);
    boost::put(boost::edge_weight, mst, res.first, boost::get(boost::edge_weight, graph, desc));
  }

  return mst;
}

void printSVG(const Graph &graph)
{
  std::cout << "<?xml version=\"1.0\"?>" << std::endl;
  std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  std::cout << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
  std::cout << "<g stroke=\"black\" stroke-width=\"5\">" << std::endl;

  const auto range = boost::edges(graph);
  for (auto it = range.first; it != range.second; it++) {
    const auto v = graph[boost::source(*it, graph)]();
    const auto w = graph[boost::target(*it, graph)]();
    std::cout << "<line x1=\"" << v(0) << "\" y1=\"" << v(1) << "\" x2=\"" << w(0) << "\" y2=\"" << w(1) << "\"/>" << std::endl;
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
  auto mst = genMST(graph);
  printSVG(mst);

  return 0;
}
