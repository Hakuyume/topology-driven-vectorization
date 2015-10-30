#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "pixel.hpp"

constexpr double epsCoeff{0.1};
constexpr double deltaT{0.1};
constexpr double movingLimit{0.01};

std::vector<Pixel> extractPixels(const cv::Mat &src);
size_t countActivePixels(const std::vector<Pixel> &pixels);

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "no file specified" << std::endl;
    return 1;
  }
  cv::Mat raw = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if (raw.empty()) {
    std::cerr << "can't open file '" << argv[1] << "'" << std::endl;
    return 1;
  }
  cv::Mat src;
  raw.convertTo(src, CV_64F, -1.0 / 256, 1.0);

  auto pixels = extractPixels(src);
  const auto initialActives = countActivePixels(pixels);
  auto actives = initialActives;

  while (actives > initialActives * movingLimit) {
    std::cerr << "moving: " << actives << " pixels" << std::endl;

    const point::Map<std::vector<Pixel>::iterator> map{pixels.begin(), pixels.end()};
    for (auto &p : pixels)
      p.check(map);
    for (auto &p : pixels)
      p.move();

    actives = countActivePixels(pixels);
  }

  cv::Mat result{src.size(), CV_8U, cv::Scalar(0)};
  for (const auto &p : pixels)
    if (not p.isActive())
      result.at<uchar>(p()(1), p()(0)) = 255;

  cv::imshow("result", result);
  cv::waitKey(0);

  return 0;
}

std::vector<Pixel> extractPixels(const cv::Mat &src)
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

  return pixels;
}

size_t countActivePixels(const std::vector<Pixel> &pixels)
{
  size_t count = 0;
  for (const auto &p : pixels)
    if (p.isActive())
      count++;
  return count;
}
