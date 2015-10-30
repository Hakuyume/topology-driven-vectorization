#include "point.hpp"
#include "pixel.hpp"

Pixel::Pixel(const point::Vector &p, const point::Vector &m)
    : p{p}, m{m}, active{true}
{
}

point::Vector Pixel::operator()() const
{
  return p;
}

bool Pixel::isActive() const
{
  return active;
}

void Pixel::move()
{
  if (active)
    p += m;
}

void Pixel::check(const point::Map<std::vector<Pixel>::iterator> &map)
{
  if (not active)
    return;

  for (const auto &q : map.find(p, 1))
    if (m.dot(q->m) < 0) {
      active = false;
      return;
    }
}
