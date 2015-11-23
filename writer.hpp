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

  template <class T>
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

  template <class T1, class T2>
  void write(const std::pair<T1, T2> &pair)
  {
    os << "[";
    write(pair.first);
    os << ",";
    write(pair.second);
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
