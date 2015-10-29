#pragma once

#include <vector>
#include <list>
#include <map>
#include <Eigen/Core>

namespace pixel
{
using Vector = Eigen::Vector2d;

class Pixel;
class PixelSet;

class Pixel
{
  friend PixelSet;

public:
  Pixel(const Vector &p, const Vector &m);
  bool isActive() const;
  void move();
  void check(const PixelSet &pixelSet);

private:
  Vector p, m;
  bool active;
};

class PixelSet
{
public:
  PixelSet(const std::vector<Pixel> &pixels);
  std::list<Pixel> findNeighbors(const Vector &p, const double &r) const;
  void move();
  size_t countActives() const;

private:
  struct CmpVector {
    bool operator()(const Vector &a, const Vector &b) const
    {
      return (static_cast<int>(a(0)) < static_cast<int>(b(0))) ||
             (static_cast<int>(a(1)) < static_cast<int>(b(1)));
    }
  };

  size_t active;
  std::vector<Pixel> pixels;
  std::map<Vector, std::list<Pixel>, CmpVector> pixelMap;
};
}
