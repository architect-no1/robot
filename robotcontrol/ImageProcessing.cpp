//------------------------------------------------------------------------------------------------
// File: ImageProcessing.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Processes images from the Camera to locate various attributes
//----------------------------------------------------------------------------------------------
#include "ImageProcessing.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define FUDGE_BIAS 0.01 // Bias robot to the left or right


static const cv::Scalar ROI_COLOR   (  0.0, 255.0,   0.0);
static const cv::Scalar TRACK_COLOR (  0.0,   0.0, 255.0);
static const cv::Scalar SELECT_COLOR(255.0, 255.0, 255.0);
static const cv::Scalar NAV_COLOR   (  0.0, 255.0, 255.0);
//----------------------------------------------------------------
// FindLineInImageAndComputeOffset - This rountine finds the line 
// in a image and computes the the line offseyt from the center of 
// the image as the retuen value. If no line if found the center 
// value is returned
//-----------------------------------------------------------------
float FindLineInImageAndComputeOffset(cv::Mat& CameraImage)
{
  bool foundCrossing = false;
  return FindLineInImageAndComputeOffset2(CameraImage, foundCrossing);
}

float FindLineInImageAndComputeOffset2(cv::Mat& CameraImage, bool &foundCrossing)
{
   char text[1024];
   float offsetfromcenter;
   cv::Scalar mean,stddev;
   vector<vector<Point> > contours;
   vector<Vec4i> hierarchy;
   Mat mono,blur,thresh,erodeImg,dilateImg;
   Rect RoiRec(10, 2*CameraImage.rows/3, CameraImage.cols-20, CameraImage.rows/12); //Define region of interest rectangle

   Mat roi(CameraImage,RoiRec); // clip image to region of interest 
   cvtColor(roi, mono, COLOR_BGR2GRAY);  // coovert image to grayscale 
   meanStdDev(mono,mean,stddev);         // Comppute image mean and stddev -- be to used later if needed

   //std::cout<<"Variance: "<<stddev.val[0]<<std::endl;
   GaussianBlur(mono, blur,Size(9, 9), 2, 2); // blur image to remove small irregularities
   threshold(blur, thresh, 0, 255, THRESH_BINARY_INV|THRESH_OTSU); //Color thresholding makes image more blacka nd white
   cv::Mat hsv;
   cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
   cv::Mat mask1, mask2;
   inRange(hsv, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mask1);
   inRange(hsv, cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255), mask2);
   cv::Mat mask = mask1 | mask2;
   thresh |= mask;

   cv::Mat green;
   inRange(roi, cv::Scalar(0, 160, 100), cv::Scalar(255, 255, 255), green);
   thresh &= ~green;

   Mat erodeElmt = getStructuringElement(MORPH_RECT, Size(3,3));
   Mat dilateElmt = getStructuringElement(MORPH_RECT, Size(5,5));
   erode(thresh, erodeImg, erodeElmt);       // reduces noise Extract edges 
   dilate(erodeImg, dilateImg, dilateElmt);

  findContours(dilateImg, contours, hierarchy, RETR_LIST,CHAIN_APPROX_SIMPLE);// Find the contours of the frame

  rectangle(CameraImage, RoiRec, ROI_COLOR,3); // draw region of interest on camera image

  double minMaxCx = DBL_MAX;
  Rect selected_edge(0,0,0,0); //Edge beging followed
  Rect nav_point(0,0,0,0);
  double distanceMin = DBL_MAX;
  for(unsigned int i = 0; i<contours.size();i++)  //Find the biggest contour 
     {
       Moments mu = moments(contours[i]);
       if (mu.m00 > 100.0) // area threadhold
         {
           Rect r = boundingRect(contours[i]);
           Rect show(r.x+RoiRec.x,r.y+RoiRec.y,r.width,r.height);
           rectangle(CameraImage, show, TRACK_COLOR,3); // Draw contours found

           selected_edge.width=20;
           nav_point.width=20;
           double cx;
	   cx = r.x + r.width/2;
	   double distance = fabs(r.x + r.width/2 - (RoiRec.x + RoiRec.width / 2));
           //line( CameraImage,Point(cx+10,0),Point(cx+10,CameraImage.rows),Scalar( 0, 255, 0 ),2,8); 
	   if (RoiRec.width - r.width < 40) {
       foundCrossing = true;
       distance = 0;
     }
           if (distance < distanceMin)
             {
               distanceMin = distance;
               minMaxCx = cx;
               selected_edge=show;
               selected_edge.x=show.x+r.width-10;
               selected_edge.width=20;
               nav_point=show;
               nav_point.x=10+minMaxCx-10;
               nav_point.width=20;
             }
         }
     }

  if (fabs(minMaxCx)==DBL_MAX) minMaxCx = roi.cols/2;

  rectangle(CameraImage, selected_edge, SELECT_COLOR,3);
  rectangle(CameraImage, nav_point, NAV_COLOR,3);
  line( CameraImage,Point(CameraImage.cols/2,0),Point(CameraImage.cols/2,CameraImage.rows),Scalar( 0, 0, 0 ),2,8);
  //if (!IsPi3) imshow( "processed", dilateImg );
  offsetfromcenter=1.0f - 2.0f*(float)minMaxCx/roi.cols;
  //sprintf(text,"Nav Value %f",offsetfromcenter);
  //cv::putText(CameraImage, text,cv::Point(10,50),  CV_FONT_HERSHEY_PLAIN, 2,CV_RGB(0,0,0),3);

  /* find red thing in roi
  Mat hsv;
  cvtColor(roi, hsv, COLOR_BGR2HSV);
  Mat mask1, mask2;
  inRange(hsv, Scalar(0, 70, 50), Scalar(10, 255, 255), mask1);
  inRange(hsv, Scalar(170, 70, 50), Scalar(180, 255, 255), mask2);
  Mat mask = mask1 | mask2;
  vector<vector<Point> > contoursRed;
  vector<Vec4i> hierarchyRed;
  findContours(mask, contoursRed, hierarchyRed, RETR_LIST,CHAIN_APPROX_SIMPLE);// Find the contours of the frame
  if (0 < contoursRed.size()) foundCrossing = true;
  */

  return offsetfromcenter+FUDGE_BIAS;
}
//-----------------------------------------------------------------
// END FindLineInImageAndComputeOffset
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
