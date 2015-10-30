#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "clustering.hpp"

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

  const auto pixels = movePixels::movePixels(src, eps_coeff, delta_t, moving_limit);
  cv::Mat result{src.size(), CV_8U, cv::Scalar(0)};
  for (const auto &p : pixels)
    result.at<uchar>(p()(1), p()(0)) = 255;

  cv::imshow("result", result);
  cv::waitKey(0);

  Graph graph;
  point::Map<Vertex> map;
  for (const auto &p : pixels) {
    const auto desc = boost::add_vertex(graph);
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
        graph[res.first].weight = (w.p - v.p).norm();
    }
  }

  std::cerr << boost::num_edges(graph) << std::endl;

  return 0;
}
