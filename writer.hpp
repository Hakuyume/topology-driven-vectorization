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

  template <typename T>
  void write(const std::vector<T> &vector)
  {
    os << "[";
    for (auto it = vector.begin(); it != vector.end(); it++) {
      if (it != vector.begin())
        os << ",";
      write(*it);
    }
    os << "]";
  }

  void write(const extractCenterline::Centerline &line)
  {
    write<extractCenterline::Pixel>(line);
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
