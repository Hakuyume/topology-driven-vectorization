#include "extract_centerline.hpp"

using namespace extractCenterline;

Pixel::Pixel(const point::Point &p)
    : point::Point{p}, delta{0, 0}
{
}

void Pixel::setDelta(const Pixel &prev, const Pixel &next)
{
  point::Vector p_sum{0, 0};
  double w_sum{0};

  for (const auto &q : {prev, *this, next}) {
    const double w = exp(-(q.pos - pos).norm() / (2 * thick * thick));
    p_sum += w * q.pos;
    w_sum += w;
  }

  const point::Vector v = (next.pos - pos).normalized() + (pos - prev.pos).normalized();
  const point::Vector normal = point::Vector(-v(1), v(0)).normalized();
  delta = (p_sum / w_sum - pos).dot(normal) * normal;
}

void Pixel::move()
{
  pos += delta;
}

Centerline::operator const std::vector<Pixel> &() const
{
  return pixels;
}

void Centerline::smooth()
{
  if (pixels.size() < 3)
    return;
  for (auto it = std::next(pixels.begin()); std::next(it) != pixels.end(); it++)
    it->setDelta(*std::prev(it), *std::next(it));
  for (auto &p : pixels)
    p.move();
}
