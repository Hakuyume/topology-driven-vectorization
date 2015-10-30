#pragma once

#include <vector>
#include <map>
#include <Eigen/Core>

namespace point
{
using Vector = Eigen::Vector2d;

template <class T>
class Map
{
public:
  Map()
  {
  }

  template <class C>
  Map(const C &points)
  {
    for (const auto &p : points)
      map.insert({p(), p});
  }

  void push(const T &p)
  {
    map.insert({p(), p});
  }

  std::vector<T> find(const Vector &p, const double &r) const
  {
    std::vector<T> neighbors;

    for (int dx = -ceil(r); dx <= ceil(r); dx++)
      for (int dy = -ceil(r); dy <= ceil(r); dy++) {
        const auto key = p + Vector(dx, dy);
        const auto begin = map.lower_bound(key);
        const auto end = map.upper_bound(key);
        for (auto it = begin; it != end; it++)
          if ((it->second() - p).norm() < r)
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
