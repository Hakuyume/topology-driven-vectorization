#pragma once

#include <Eigen/Core>

namespace pixel
{
using Vector = Eigen::Vector2d;

class Pixel;
class PixelSet;

class Pixel
{
public:
  Pixel(const Vector &p, const Vector &m);
  bool isActive() const;
  void move();
  void check(const PixelSet &pixelSet);

private:
  Eigen::Vector2d p, m;
  bool active;
};

class PixelSet
{
public:
  PixelSet(const std::vector<Pixel> &pixels);
  std::vector<Pixel> findNeighbors(const Vector &p, const double &r) const;

private:
  std::vector<Pixel> pixels;
};
}
