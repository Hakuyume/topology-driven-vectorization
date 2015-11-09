#pragma once

#include "point.hpp"

namespace extractCenterline
{
class Pixel
{
public:
  template <typename T>
  Pixel(const T &p)
      : p{p()}, t{p.thickness()} {}
  point::Vector operator()() const;
  double thickness() const;
  void setDelta(const std::vector<Pixel> &pixels);
  void move();

private:
  point::Vector p, delta;
  double t;
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
