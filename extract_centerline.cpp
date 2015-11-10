#include "extract_centerline.hpp"

using namespace extractCenterline;

point::Vector Pixel::operator()() const
{
  return p;
}

double Pixel::thickness() const
{
  return t;
}

void Pixel::setDelta(const Pixel &prev, const Pixel &next)
{
  point::Vector p_sum{point::Vector::Zero()};
  double w_sum{0};

  for (const auto &q : {prev, *this, next}) {
    const double w = exp(-(q() - p).norm() / (2 * t * t));
    p_sum += w * q();
    w_sum += w;
  }

  const auto v = (next() - p).normalized() + (p - prev()).normalized();
  const auto normal = point::Vector(-v(1), v(0)).normalized();
  delta = (p_sum / w_sum - p).dot(normal) * normal;
}

void Pixel::move()
{
  p += delta;
}

std::vector<Pixel>::const_iterator Centerline::begin() const
{
  return pixels.begin();
}

std::vector<Pixel>::const_iterator Centerline::end() const
{
  return pixels.end();
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
