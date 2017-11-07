//------------------------------------------------------------------------------------------------
// File: ImageProcessing.h
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Processes images from the Camera to locate various attributes
//------------------------------------------------------------------------------------------------
#ifndef ImageProcessingH
#define ImageProcessingH
#include <opencv2/core/core.hpp>

float FindLineInImageAndComputeOffset(cv::Mat& CameraImage);
// mode
// -1 left
// 0 center
// 1 right
float FindLineInImageAndComputeOffset2(cv::Mat& CameraImage, bool &foundCrossing);

#endif
//------------------------------------------------------------------------------------------------
//END of Include
//------------------------------------------------------------------------------------------------
