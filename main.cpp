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

struct Vertex {
  size_t id;
  point::Vector p;
  Graph::vertex_descriptor desc;
  point::Vector operator()() const { return p; }
};

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
  pixelSet.movePixels(moving_limit);
  const auto pixels = pixelSet.getValidPixels();
  Graph graph;
  point::Map<Vertex> map;
  for (const auto &p : pixels) {
    const auto desc = boost::add_vertex(graph);
    static size_t id = 0;
    graph[desc].id = id++;
    graph[desc].p = p();
    graph[desc].desc = desc;
    map.push(graph[desc]);
  }

  const auto range = boost::vertices(graph);
  for (auto it = range.first; it != range.second; it++) {
    const auto &v = graph[*it];
    for (const auto &w : map.find(v.p, 1)) {
      const auto res = boost::add_edge(v.desc, w.desc, graph);
      if (res.second)
        boost::put(boost::edge_weight, graph, res.first, (w.p - v.p).norm());
    }
  }

  std::cerr << boost::num_edges(graph) << std::endl;

  std::vector<boost::graph_traits<Graph>::edge_descriptor> spanning_tree;
  boost::kruskal_minimum_spanning_tree(graph, std::back_inserter(spanning_tree));

  std::cout << "graph topology {" << std::endl;
  std::cout << "node [ shape=circle ];" << std::endl;
  for (auto it = range.first; it != range.second; it++) {
    const auto &v = graph[*it];
    std::cout << v.id << " [ pos=\"" << v()(0) * 100 << "," << -v()(1) * 100 << "!\", label=\"\" ];" << std::endl;
  }
  for (const auto &desc : spanning_tree) {
    const auto &v = graph[boost::source(desc, graph)];
    const auto &w = graph[boost::target(desc, graph)];
    std::cout << v.id << " -- " << w.id << ";" << std::endl;
  }

  std::cout << "}" << std::endl;

  return 0;
}
