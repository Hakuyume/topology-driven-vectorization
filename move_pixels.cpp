#include <opencv2/imgproc.hpp>
#include "move_pixels.hpp"

using namespace movePixels;

Pixel::Pixel(const point::Vector &p, const point::Vector &m)
    : p{p}, m{m}, active{true}, step{0}
{
}

point::Vector Pixel::operator()() const
{
  return p;
}

double Pixel::thickness() const
{
  return m.norm() * step;
}

bool Pixel::isActive() const
{
  return active;
}

void Pixel::update(const point::Map<Pixel> &map)
{
  if (not active)
    return;

  for (const auto &q : map.find(p, 1))
    if (m.dot(q.m) < 0 and m.dot(q.p - p) < 0) {
      active = false;
      return;
    }

  p += m;
  step++;
}

PixelSet::PixelSet(const cv::Mat &src, const double &eps_coeff, const double &delta_t)
{
  cv::Mat grad_x, grad_y;
  cv::Sobel(src, grad_x, CV_64F, 1, 0, 3);
  cv::Sobel(src, grad_y, CV_64F, 0, 1, 3);

  cv::Mat grad_mag;
  cv::magnitude(grad_x, grad_y, grad_mag);
  double grad_max;
  cv::minMaxLoc(grad_mag, NULL, &grad_max, NULL, NULL);

  const auto eps = grad_max * eps_coeff;
  std::vector<cv::Point> moving_indexes;
  cv::findNonZero(grad_mag > eps, moving_indexes);

  for (const auto &index : moving_indexes)
    pixels.push_back(
        Pixel(
            point::Vector(index.x, index.y),
            point::Vector(grad_x.at<double>(index), grad_y.at<double>(index)) * delta_t));
}

size_t PixelSet::countActivePixels() const
{
  size_t count = 0;
  for (const auto &p : pixels)
    if (p.isActive())
      count++;
  return count;
}

void PixelSet::movePixels(const size_t &limit)
{
  while (countActivePixels() > limit) {
    const point::Map<Pixel> map{pixels};
    for (auto &p : pixels)
      p.update(map);
  }
}

std::vector<Pixel> PixelSet::getValidPixels() const
{
  const point::Map<Pixel> map{pixels};
  std::vector<Pixel> valid_pixels;

  for (const auto &p : pixels) {
    if (p.isActive())
      continue;
    if (map.find(p(), 1).size() < 3)
      continue;
    valid_pixels.push_back(p);
  }

  return valid_pixels;
}
