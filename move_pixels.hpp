#pragma once

#include <opencv2/core.hpp>
#include "point.hpp"

namespace movePixels
{
class Pixel : public point::Point
{
public:
  Pixel(const point::Vector &p, const point::Vector &m);
  bool isActive() const;
  void update(const point::Map<Pixel> &map, const size_t &width, const size_t &height);

private:
  point::Vector m;
  bool active;
  unsigned int step;
};

class PixelSet
{
public:
  PixelSet(const cv::Mat &src, const double &eps_coeff, const double &delta_t);
  size_t countActivePixels() const;
  void movePixels();
  void movePixels(const size_t &limit);
  std::vector<Pixel> getValidPixels() const;

private:
  size_t width, height;
  size_t actives;
  std::vector<Pixel> pixels;
};
}
