#pragma once

#include "point.hpp"

class Pixel
{
public:
  Pixel(const point::Vector &pos, const point::Vector &moving);
  point::Vector operator()() const;
  bool isActive() const;
  void move();
  void check(const point::Map<std::vector<Pixel>::iterator> &map);

private:
  point::Vector p,
      m;
  bool active;
};
