#include <iostream>
#include <opencv2/imgproc.hpp>
#include "clustering.hpp"

using namespace clustering;

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

std::vector<Pixel> clustering::clustering(const cv::Mat &src, const double &epsCoeff, const double &deltaT, const double &movingLimit)
{
  cv::Mat gradX, gradY;
  cv::Sobel(src, gradX, CV_64F, 1, 0, 3);
  cv::Sobel(src, gradY, CV_64F, 0, 1, 3);

  cv::Mat gradMag;
  cv::magnitude(gradX, gradY, gradMag);
  double gradMax;
  cv::minMaxLoc(gradMag, NULL, &gradMax, NULL, NULL);

  const auto eps = gradMax * epsCoeff;
  std::vector<cv::Point> movingIndexes;
  cv::findNonZero(gradMag > eps, movingIndexes);

  std::vector<Pixel> pixels;
  for (const auto &index : movingIndexes)
    pixels.push_back(
        Pixel(
            point::Vector(index.x, index.y),
            point::Vector(gradX.at<double>(index), gradY.at<double>(index)) * deltaT));

  const auto initialActives = countActivePixels(pixels);
  auto actives = initialActives;

  while (actives > initialActives * movingLimit) {
    std::cerr << "moving: " << actives << " pixels" << std::endl;

    const point::Map<Pixel> map{pixels};
    for (auto &p : pixels)
      p.update(map);

    actives = countActivePixels(pixels);
  }

  std::vector<Pixel> inactivePixels;
  for (const auto &p : pixels)
    if (not p.isActive())
      inactivePixels.push_back(p);

  return inactivePixels;
}
