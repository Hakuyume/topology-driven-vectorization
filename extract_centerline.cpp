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

void Pixel::setDelta(const std::vector<Pixel> &pixels)
{
  if (pixels.size() < 3)
    return;

  if (p == (*pixels.begin())() or p == (*pixels.rbegin())()) {
    delta = point::Vector::Zero();
    return;
  }

  point::Vector normal;
  point::Vector p_sum{point::Vector::Zero()};
  double w_sum{0};

  for (auto it = pixels.begin(); it != pixels.end(); it++) {
    const double w = exp(-((*it)() - p).squaredNorm() / (2 * t * t));
    p_sum += w * (*it)();
    w_sum += w;

    if ((*it)() != p)
      continue;

    const auto &prev = *std::prev(it);
    const auto &next = *std::next(it);

    const auto v = next() - prev();
    normal = point::Vector(-v(1), v(0)).normalized();
  }

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
  for (auto &p : pixels)
    p.setDelta(pixels);
  for (auto &p : pixels)
    p.move();
}
