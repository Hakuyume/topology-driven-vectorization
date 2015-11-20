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
  void push(const T &p) { map.emplace(genKey(p()), p); }
  std::vector<T> find(const Vector &p, const double &r) const
  {
    std::vector<T> neighbors;
    Vector d;
    for (d(0) = -ceil(r); d(0) <= ceil(r); d(0)++)
      for (d(1) = -ceil(r); d(1) <= ceil(r); d(1)++) {
        const auto range = map.equal_range(genKey(p + d));
        for (auto it = range.first; it != range.second; it++)
          if ((it->second() - p).norm() <= r)
            neighbors.push_back(it->second);
      }
    return neighbors;
  }

private:
  using Key = Eigen::Vector2i;
  struct CmpVector {
    bool operator()(const Key &a, const Key &b) const
    {
      if (a(0) != b(0))
        return a(0) < b(0);
      else
        return a(1) < b(1);
    }
  };
  std::multimap<Eigen::Vector2i, T, CmpVector> map;
  static Key genKey(const Vector &p) { return {floor(p(0)), floor(p(1))}; }
};
}
