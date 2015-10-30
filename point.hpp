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
  Map(const T &begin, const T &end)
  {
    for (auto it = begin; it != end; it++)
      map.insert({ (*it)(), it});
  }

  std::vector<T> find(const Vector &p, const double &r) const
  {
    std::vector<T> neighbors;

    for (int dx = -ceil(r); dx <= ceil(r); dx++)
      for (int dy = -ceil(r); dy <= ceil(r); dy++) {
        const auto range = map.equal_range(p + Vector(dx, dy));
        for (auto it = range.first; it != std::next(range.second, 1); it++)
          if (((*it->second)() - p).norm() < r)
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
