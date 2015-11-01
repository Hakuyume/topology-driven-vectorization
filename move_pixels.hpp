#pragma once

#include <opencv2/core.hpp>
#include "point.hpp"

namespace movePixels
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

class PixelSet
{
public:
  PixelSet(const cv::Mat &src, const double &eps_coeff, const double &delta_t);
  size_t countActivePixels() const;
  void movePixels(const double &moving_limit);
  std::vector<Pixel> getValidPixels() const;

private:
  std::vector<Pixel> pixels;
};
}
