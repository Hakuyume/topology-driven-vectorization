#pragma once

#include <ostream>
#include "point.hpp"
#include "extract_centerline.hpp"

namespace writer
{
class JSON
{
public:
  JSON(std::ostream &os)
      : os{os} {};

  template <typename Iterator>
  void write_iterators(const Iterator &begin, const Iterator &end)
  {
    os << "[";
    for (auto it = begin; it != end; it++) {
      if (it != begin)
        os << ",";
      write(*it);
    }
    os << "]";
  }

  template <typename T>
  void write(const std::vector<T, std::allocator<T>> &vector)
  {
    write_iterators(vector.begin(), vector.end());
  }

  void write(const extractCenterline::Centerline &line)
  {
    write_iterators(line.begin(), line.end());
  }

  void write(const point::Point &p)
  {
    os << "{"
       << "\"x\":" << p()(0) << ","
       << "\"y\":" << p()(1) << ","
       << "\"thickness\":" << p.thickness()
       << "}";
  }

private:
  std::ostream &os;
};
}
