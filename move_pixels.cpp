#include <iostream>
#include <opencv2/imgproc.hpp>
#include "move_pixels.hpp"

using namespace movePixels;

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

void Pixel::update(const point::Map<Pixel> &map)
{
  if (not active)
    return;

  for (const auto &q : map.find(p, 1))
    if (m.dot(q.m) < 0) {
      active = false;
      return;
    }

  p += m;
}

size_t countActivePixels(const std::vector<Pixel> &pixels)
{
  size_t count = 0;
  for (const auto &p : pixels)
    if (p.isActive())
      count++;
  return count;
}

std::vector<Pixel> movePixels::movePixels(const cv::Mat &src, const double &eps_coeff, const double &delta_t, const double &moving_limit)
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

  std::vector<Pixel> pixels;
  for (const auto &index : moving_indexes)
    pixels.push_back(
        Pixel(
            point::Vector(index.x, index.y),
            point::Vector(grad_x.at<double>(index), grad_y.at<double>(index)) * delta_t));

  const auto initial_actives = countActivePixels(pixels);
  auto actives = initial_actives;

  while (actives > initial_actives * moving_limit) {
    std::cerr << "moving: " << actives << " pixels" << std::endl;

    const point::Map<Pixel> map{pixels};
    for (auto &p : pixels)
      p.update(map);

    actives = countActivePixels(pixels);
  }

  const point::Map<Pixel> map{pixels};
  std::vector<Pixel> inactive_pixels;

  for (const auto &p : pixels) {
    if (p.isActive())
      continue;
    if (map.find(p(), 1).size() < 3)
      continue;
    inactive_pixels.push_back(p);
  }

  return inactive_pixels;
}
