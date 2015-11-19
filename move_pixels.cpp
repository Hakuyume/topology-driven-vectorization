#include <opencv2/imgproc.hpp>
#include "move_pixels.hpp"

using namespace movePixels;

bool inRect(const point::Vector &p, const double &width, const double &height)
{
  return (0 <= p(0) and p(0) < width and 0 <= p(1) and p(1) < height);
}

Pixel::Pixel(const point::Vector &p, const point::Vector &m)
    : point::Point{p, 0}, m{m}, active{true}, step{0}
{
}

bool Pixel::isActive() const
{
  return active;
}

void Pixel::update(const point::Map<Pixel> &map, const size_t &width, const size_t &height)
{
  if (not active)
    return;

  pos += m;
  thick = m.norm() * ++step;

  if (not inRect(pos, width, height)) {
    active = false;
    return;
  }

  for (const auto &q : map.find(pos, 1))
    if (m.dot(q.m) < 0 and m.dot(q.pos - pos) < 0) {
      active = false;
      return;
    }
}

PixelSet::PixelSet(const cv::Mat &src, const double &eps_coeff, const double &delta_t)
    : width{static_cast<size_t>(src.size().width)}, height{static_cast<size_t>(src.size().height)}
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
  actives = pixels.size();
}

size_t PixelSet::countActivePixels() const
{
  return actives;
}

void PixelSet::movePixels()
{
  const point::Map<Pixel> map{pixels};
  actives = 0;
  for (auto &p : pixels) {
    p.update(map, width, height);
    if (p.isActive())
      actives++;
  }
}

void PixelSet::movePixels(const size_t &limit)
{
  while (countActivePixels() > limit)
    movePixels();
}

std::vector<Pixel> PixelSet::getValidPixels() const
{
  const point::Map<Pixel> map{pixels};
  std::vector<Pixel> valid_pixels;

  for (const auto &p : pixels) {
    if (p.isActive())
      continue;
    if (not inRect(p(), width, height))
      continue;
    if (map.find(p(), 1).size() < 3)
      continue;
    valid_pixels.push_back(p);
  }

  return valid_pixels;
}
