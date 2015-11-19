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
  template <typename Container>
  Centerline(const Container &points)
      : pixels(points.begin(), points.end()) {}
  std::vector<Pixel>::const_iterator begin() const;
  std::vector<Pixel>::const_iterator end() const;
  void smooth();

private:
  std::vector<Pixel> pixels;
};
}
