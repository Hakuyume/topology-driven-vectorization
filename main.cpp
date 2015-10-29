#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Eigen/Core>

class MovingPixel
{
public:
  MovingPixel(const cv::Point &pos, const double &gradX, const double &gradY)
      : p{pos.x, pos.y}, m{gradX * deltaT, gradY * deltaT}, active{true} {}
  bool isActive() const { return active; }
private:
  static constexpr double deltaT = 0.1;
  Eigen::Vector2d p, m;
  bool active;
};

class PixelSet
{
public:
  PixelSet(const std::vector<MovingPixel> &pixels)
      : pixels{pixels} {}
private:
  std::vector<MovingPixel> pixels;
};

PixelSet genPixelSet(const cv::Mat &src)
{
  cv::Mat gradX, gradY;
  cv::Sobel(src, gradX, CV_64F, 1, 0, 3);
  cv::Sobel(src, gradY, CV_64F, 0, 1, 3);

  cv::Mat gradMag;
  cv::magnitude(gradX, gradY, gradMag);
  double gradMax;
  cv::minMaxLoc(gradMag, NULL, &gradMax, NULL, NULL);

  const auto eps = gradMax * 0.1;
  std::vector<cv::Point> movingIndexes;
  cv::findNonZero(gradMag > eps, movingIndexes);

  std::vector<MovingPixel> pixels;
  for (auto &index : movingIndexes)
    pixels.push_back(MovingPixel(index, gradX.at<double>(index), gradY.at<double>(index)));

  return PixelSet(pixels);
}

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
  raw.convertTo(src, CV_64F, 1.0 / 256);

  cv::imshow("input", src);

  auto pixelSet = genPixelSet(src);

  while (cv::waitKey(0) != 'q') {
  };

  return 0;
}
