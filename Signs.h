#ifndef YOLO_SIGNS_H_
#define YOLO_SIGNS_H_

#include <string>
#include <opencv2/core/core.hpp>

int initSign();
std::string getSign(cv::Mat& camera);

#endif // YOLO_SIGNS_H_
