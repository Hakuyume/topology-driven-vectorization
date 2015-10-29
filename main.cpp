#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "no file specified" << std::endl;
    return 1;
  }
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if (src.empty()) {
    std::cerr << "can't open file '" << argv[1] << "'" << std::endl;
    return 1;
  }

  cv::imshow("input", src);

  while (cv::waitKey(0) != 'q') {
  };

  return 0;
}
