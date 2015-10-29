#include "pixel.hpp"

using namespace pixel;

Pixel::Pixel(const Vector &p, const Vector &m)
    : p{p}, m{m}, active{true}
{
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

void Pixel::check(const PixelSet &pixelSet)
{
  for (const auto &q : pixelSet.findNeighbors(p, 1))
    if (m.dot(q.m) < 0)
      active = false;
}

PixelSet::PixelSet(const std::vector<Pixel> &pixels)
    : pixels{pixels}
{
  active = 0;
  for (auto &p : pixels)
    if (p.isActive())
      active++;
}

std::vector<Pixel> PixelSet::findNeighbors(const Vector &p, const double &r) const
{
  std::vector<Pixel> neighbors;

  for (int dx = -ceil(r); dx <= ceil(r); dx++)
    for (int dy = -ceil(r); dy <= ceil(r); dy++) {
      const auto it = pixelMap.find(p + Vector(dx, dy));

      if (it == pixelMap.end())
        continue;

      for (const auto &n : it->second)
        if ((p - n.p).norm() < r)
          neighbors.push_back(n);
    }

  return neighbors;
}

void PixelSet::move()
{
  pixelMap.clear();
  for (const auto &p : pixels)
    pixelMap[p.p].push_back(p);

  active = 0;
  for (auto &p : pixels) {
    p.check(*this);
    if (p.isActive())
      active++;
  }

  for (auto &p : pixels)
    p.move();
}

size_t PixelSet::countActives() const
{
  return active;
}
