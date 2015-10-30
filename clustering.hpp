#pragma once

#include <opencv2/core.hpp>
#include "point.hpp"

namespace clustering
{
class Pixel
{
public:
  Pixel(const point::Vector &pos, const point::Vector &moving);
  point::Vector operator()() const;
  bool isActive() const;
  void update(const point::Map<Pixel> &map);

private:
  point::Vector p, m;
  bool active;
};

std::vector<Pixel> clustering(const cv::Mat &src, const double &eps_coeff, const double &delta_t, const double &moving_limit);
}
