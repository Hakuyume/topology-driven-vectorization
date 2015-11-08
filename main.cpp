#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "move_pixels.hpp"
#include "extract_topology.hpp"

constexpr double eps_coeff{0.1};
constexpr double delta_t{0.1};
constexpr double moving_limit{0.01};

void printSVG(const extractTopology::Graph &graph)
{
  std::cout << "<?xml version=\"1.0\"?>" << std::endl;
  std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  std::cout << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
  std::cout << "<g stroke=\"black\" stroke-width=\"0.1\">" << std::endl;

  const auto edges = boost::edges(graph);
  for (auto it = edges.first; it != edges.second; it++) {
    const auto &v = graph[boost::source(*it, graph)];
    const auto &u = graph[boost::target(*it, graph)];
    std::cout << "<line x1=\"" << v()(0) << "\" y1=\"" << v()(1) << "\" x2=\"" << u()(0) << "\" y2=\"" << u()(1) << "\"/>" << std::endl;
  }

  std::cout << "</g>" << std::endl;
  std::cout << "</svg>" << std::endl;
}

void printSVG(const extractTopology::Graph &graph, const std::vector<std::vector<extractTopology::VertexDescriptor>> &paths)
{
  std::cout << "<?xml version=\"1.0\"?>" << std::endl;
  std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
  std::cout << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
  std::cout << "<g stroke=\"black\" stroke-width=\"0.1\" fill=\"none\">" << std::endl;

  for (const auto &path : paths) {
    std::cout << "<path d=\"M";
    for (const auto &v_desc : path) {
      const auto &v = graph[v_desc];
      std::cout << " " << v()(0) << " " << v()(1);
    }
    std::cout << "\"/>" << std::endl;
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

  extractTopology::Graph graph;
  for (const auto &p : pixels)
    extractTopology::addVertex(graph, p(), p.traveledDistance());
  extractTopology::addEdges(graph);
  auto mst = extractTopology::getMST(graph);
  extractTopology::pruneBranches(mst);

  printSVG(mst, extractTopology::getPaths(mst));

  return 0;
}
