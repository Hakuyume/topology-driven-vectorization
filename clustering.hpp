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
  void move();
  void check(const point::Map<std::vector<Pixel>::iterator> &map);

private:
  point::Vector p, m;
  bool active;
};

std::vector<Pixel> clustering(const cv::Mat &src, const double &epsCoeff, const double &deltaT, const double &movingLimit);
}
