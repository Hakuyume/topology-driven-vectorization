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
  for (const auto &q : pixelSet.findNeighbors(p, 1)) {
  }
}

PixelSet::PixelSet(const std::vector<Pixel> &pixels)
    : pixels{pixels}
{
}

std::vector<Pixel> PixelSet::findNeighbors(const Vector &p, const double &r) const
{
  std::vector<Pixel> neighbors;
  return neighbors;
}
