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
public:
  Pixel(const Vector &pos, const Vector &moving);
  Vector pos() const;
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
  PixelSet(const size_t &width, const size_t &height, const std::vector<Pixel> &pixels);
  std::list<Pixel> findNeighbors(const Vector &p, const double &r) const;
  void move();
  size_t countActivePixels() const;
  std::vector<Pixel> allPixels() const;
  size_t width() const;
  size_t height() const;

private:
  struct CmpVector {
    bool operator()(const Vector &a, const Vector &b) const
    {
      if (floor(a(0)) != floor(b(0)))
        return floor(a(0)) < floor(b(0));
      else
        return floor(a(1)) < floor(b(1));
    }
  };

  size_t w, h;
  size_t actives;
  std::vector<Pixel> pixels;
  std::map<Vector, std::list<Pixel>, CmpVector> pixelMap;
};
}
