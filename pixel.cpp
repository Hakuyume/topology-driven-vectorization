#include "pixel.hpp"

using namespace pixel;

Pixel::Pixel(const Vector &pos, const Vector &moving)
    : p{pos}, m{moving}, active{true}
{
}

Vector Pixel::pos() const
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

void Pixel::check(const PixelSet &pixelSet)
{
  if (p(0) < 0) {
    p(0) = 0;
    active = false;
  } else if (pixelSet.width() <= p(0)) {
    p(0) = pixelSet.width() - 1;
    active = false;
  }
  if (p(1) < 0) {
    p(1) = 0;
    active = false;
  } else if (pixelSet.height() <= p(1)) {
    p(1) = pixelSet.height() - 1;
    active = false;
  }

  if (not active)
    return;

  for (const auto &q : pixelSet.findNeighbors(p, 1))
    if (m.dot(q.m) < 0) {
      active = false;
      return;
    }
}

PixelSet::PixelSet(const size_t &width, const size_t &height, const std::vector<Pixel> &pixels)
    : w{width}, h{height}, pixels{pixels}
{
  actives = 0;
  for (auto &p : pixels)
    if (p.isActive())
      actives++;
}

std::list<Pixel> PixelSet::findNeighbors(const Vector &p, const double &r) const
{
  std::list<Pixel> neighbors;

  for (int dx = -ceil(r); dx <= ceil(r); dx++)
    for (int dy = -ceil(r); dy <= ceil(r); dy++) {
      const auto it = pixelMap.find(p + Vector(dx, dy));

      if (it == pixelMap.end())
        continue;

      for (const auto &n : it->second)
        if ((p - n.pos()).norm() < r)
          neighbors.push_back(n);
    }

  return neighbors;
}

void PixelSet::move()
{
  for (auto &p : pixels)
    p.move();

  pixelMap.clear();
  for (const auto &p : pixels) {
    const auto it = pixelMap.find(p.pos());
    if (it == pixelMap.end())
      pixelMap.insert({p.pos(), {p}});
    else
      it->second.push_back(p);
  }

  actives = 0;
  for (auto &p : pixels) {
    p.check(*this);
    if (p.isActive())
      actives++;
  }
}

size_t PixelSet::width() const
{
  return w;
}
size_t PixelSet::height() const
{
  return h;
}

size_t PixelSet::countActivePixels() const
{
  return actives;
}

std::vector<Pixel> PixelSet::allPixels() const
{
  return pixels;
}
