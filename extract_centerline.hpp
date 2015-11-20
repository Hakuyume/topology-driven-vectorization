#pragma once

#include "point.hpp"

namespace extractCenterline
{
class Pixel : public point::Point
{
public:
  Pixel(const point::Point &p);
  void setDelta(const Pixel &prev, const Pixel &next);
  void move();

private:
  point::Vector delta;
};

class Centerline
{
public:
  template <class T>
  Centerline(const std::vector<T> &points)
      : pixels(points.begin(), points.end()) {}
  operator const std::vector<Pixel> &() const;
  void smooth();

private:
  std::vector<Pixel> pixels;
};
}
