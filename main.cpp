#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "move_pixels.hpp"
#include "extract_topology.hpp"
#include "extract_centerline.hpp"
#include "writer.hpp"

constexpr double eps_coeff{0.1};
constexpr double delta_t{0.1};
constexpr double moving_limit{0.01};

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

  auto graph = extractTopology::createGraph(pixels);
  auto mst = extractTopology::getMST(graph);
  extractTopology::pruneBranches(mst);
  const auto paths = extractTopology::getPaths(mst);

  std::vector<extractCenterline::Centerline> centerlines(paths.begin(), paths.end());

  for (auto &c : centerlines)
    c.smooth();

  writer::SVG(std::cout) << centerlines;

  return 0;
}
