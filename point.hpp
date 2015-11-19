#pragma once

#include <vector>
#include <map>
#include <Eigen/Core>

namespace point
{
using Vector = Eigen::Vector2d;

class Point
{
public:
  Point() {}
  Point(const Point &p)
      : pos{p.pos}, thick{p.thick} {}
  Point(const Vector &p, const double &t)
      : pos{p}, thick{t} {}
  point::Vector operator()() const { return pos; }
  double thickness() const { return thick; }
protected:
  Vector pos;
  double thick;
};

template <class T>
class Map
{
public:
  Map() {}
  Map(const std::vector<T> &points)
  {
    for (const auto &p : points)
      push(p);
  }
  size_t size() const { return map.size(); }
  void push(const T &p) { map.emplace(p(), p); }
  std::vector<T> find(const Vector &p, const double &r) const
  {
    std::vector<T> neighbors;

    for (int dx = -ceil(r); dx <= ceil(r); dx++)
      for (int dy = -ceil(r); dy <= ceil(r); dy++) {
        const auto key = p + Vector(dx, dy);
        const auto range = map.equal_range(key);
        for (auto it = range.first; it != range.second; it++)
          if ((it->second() - p).norm() <= r)
            neighbors.push_back(it->second);
      }

    return neighbors;
  }

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

  std::multimap<Vector, T, CmpVector> map;
};
}
