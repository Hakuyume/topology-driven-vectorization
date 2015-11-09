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

std::vector<Pixel>::const_iterator Centerline::begin() const
{
  return pixels.begin();
}

std::vector<Pixel>::const_iterator Centerline::end() const
{
  return pixels.end();
}
