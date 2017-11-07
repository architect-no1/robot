// test_opencv_signs.cpp : Defines the entry point for the console application.
//
#if  defined(_WIN32) || defined(_WIN64)
#define IMAGE_DIR "signs\\"
#else
#define IMAGE_DIR "../TestOpenCvSigns/signs/"
#endif


#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <algorithm>    // std::max

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include "opencv2/video/tracking.hpp"
#include "PI3OpencvCompat.h"
#include "NetworkUDP.h"
#include "UdpSendRecvJpeg.h"

#define NUM_SIGNS 10

using namespace cv;
using namespace std;

extern TUdpLocalPort *UdpLocalPort;
extern TUdpDest      *UdpDest;


class Symbol
{

public:
	Mat img;
	string name;

};

static double median(cv::Mat channel);
static SOCKET_FD_TYPE SetupUDPsocket(const char *hostname, char * portno, struct sockaddr_in *remaddr);
static int SendImageUDP(SOCKET_FD_TYPE fd, struct sockaddr_in *remaddr, unsigned char *jpg, size_t length);

static double median(cv::Mat channel)
{
	double m = (channel.rows*channel.cols) / 2;
	int    bin = 0;
	double med = -1.0;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat hist;
	cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

	for (int i = 0; i < histSize && med < 0.0; ++i)
	{
		bin += cvRound(hist.at< float >(i));
		if (bin > m && med < 0.0)
			med = i;
	}

	return med;
}

static void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (unsigned int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}

static int readRefImages(Symbol *symbols)
{

	symbols[0].img = imread(IMAGE_DIR"arrowL.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[0].img.data)
		return -1;
	threshold(symbols[0].img, symbols[0].img, 100, 255, 0);
	symbols[0].name = "Left 90";

	symbols[1].img = imread(IMAGE_DIR"arrowR.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[1].img.data)
		return -1;
	threshold(symbols[1].img, symbols[1].img, 100, 255, 0);
	symbols[1].name = "Right 90";

	symbols[2].img = imread(IMAGE_DIR"arrowT.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[2].img.data)
		return -1;
	threshold(symbols[2].img, symbols[2].img, 100, 255, 0);
	symbols[2].name = "Turn Around";

	symbols[3].img = imread(IMAGE_DIR"arrowS.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[3].img.data)
		return -1;
	threshold(symbols[3].img, symbols[3].img, 100, 255, 0);
	symbols[3].name = "Straight";

	symbols[4].img = imread(IMAGE_DIR"arrowPush.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[4].img.data)
		return -1;
	threshold(symbols[4].img, symbols[4].img, 100, 255, 0);
	symbols[4].name = "Push Can";

	symbols[5].img = imread(IMAGE_DIR"arrowStop.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[5].img.data)
		return -1;
	threshold(symbols[5].img, symbols[5].img, 100, 255, 0);
	symbols[5].name = "Stop";

	symbols[6].img = imread(IMAGE_DIR"arrowB.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[6].img.data)
		return -1;
	threshold(symbols[6].img, symbols[6].img, 100, 255, 0);
	symbols[6].name = "Ball";

	symbols[7].img = imread(IMAGE_DIR"arrowL45.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[7].img.data)
		return -1;
	threshold(symbols[7].img, symbols[7].img, 100, 255, 0);
	symbols[7].name = "Left 45";

	symbols[8].img = imread(IMAGE_DIR"arrowR45.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[8].img.data)
		return -1;
	threshold(symbols[8].img, symbols[8].img, 100, 255, 0);
	symbols[8].name = "Right 45";

	symbols[9].img = imread(IMAGE_DIR"arrowGo.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols[9].img.data)
		return -1;
	threshold(symbols[9].img, symbols[9].img, 100, 255, 0);
	symbols[9].name = "Go";

	return 0;
}






Symbol             symbols[NUM_SIGNS];
int initSign() {
  static bool isInit = false;
  if (!isInit && readRefImages(symbols) == -1)
  {
    fprintf(stderr, "Error reading reference symbols\n");
    return -1;
  }
  isInit = true;
  return 0;
}

//const char *getSign(cv::Mat& camera) {
std::string getSign(cv::Mat& camera) {
	Mat greyImg, new_image;
  cvtColor(camera, greyImg, CV_RGB2GRAY);

  //threshold(greyImg, greyImg, 100, 255, 0);

  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);


  double med;
  double sigma = 0.33;
  int lower, upper;

  /// Detect edges using canny
  //med = median(greyImg);
  med = 120.0;
  lower = int(max(0, (int)((1.0 - sigma) * med)));
  upper = int(min(255, (int)((1.0 + sigma) * med)));
  Canny(greyImg, canny_output, lower, upper);


#if 0
              if (!IsPi3) imshow("B", canny_output);
#endif
  /// Find contours
  findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
    CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<Point> approxRect;

  int maxsizeIndex = -1;
  float maxsize = 0;
  size_t i;
  for (i = 0; i < contours.size(); i++) {

    approxPolyDP(contours[i], approxRect,
      arcLength(Mat(contours[i]), true) * 0.01, true);

    if (approxRect.size() == 4) {
      double area = contourArea(contours[i]);
      //printf("area[%d] %f\n", i, area);
      if (maxsize < area) {
	maxsizeIndex = i;
	maxsize = area;
      }

      // area threshold 2000 for 640*480
      if (area > 300)

      {
        std::vector<cv::Point2f> corners;

        vector<Point>::iterator vertex;
        vertex = approxRect.begin();
        //vertex++;
        circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
        corners.push_back(*vertex);
        vertex++;
        circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
        corners.push_back(*vertex);
        vertex++;
        circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
        corners.push_back(*vertex);
        vertex++;
        circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
        corners.push_back(*vertex);

        Moments mu;
        mu = moments(contours[i], false);
        Point2f center((float)(mu.m10 / mu.m00), (float)(mu.m01 / mu.m00));

        sortCorners(corners, center);

        // Define the destination image
        Mat correctedImg = ::Mat::zeros(195, 271, CV_8UC3);

        // Corners of the destination image
        std::vector<cv::Point2f> quad_pts;
        quad_pts.push_back(Point2f(0, 0));
        quad_pts.push_back(Point2f((float)correctedImg.cols, 0.0f));
        quad_pts.push_back(Point2f((float)correctedImg.cols, (float)correctedImg.rows));
        quad_pts.push_back(Point2f(0.0f, (float)correctedImg.rows));

        // Get transformation matrix
        Mat transmtx = getPerspectiveTransform(corners, quad_pts);

        // Apply perspective transformation
        warpPerspective(camera, correctedImg, transmtx,
          correctedImg.size());

        Mat correctedImgBin;

        cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);
        //equalizeHist(correctedImgBin, correctedImgBin);
        correctedImgBin.copyTo(new_image);

        threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

        if (!IsPi3) imshow("B", correctedImgBin);


        double minVal, maxVal, medVal;


        minMaxLoc(new_image, &minVal, &maxVal);

        medVal = (maxVal - minVal) / 2;

        threshold(new_image, new_image, medVal, 255, 0);

        if (!IsPi3) imshow("C", new_image);

        Mat diffImg;

        int match, minDiff, diff;
        minDiff = 12000;
        match = -1;

        for (int i = 0; i < NUM_SIGNS; i++) {

          bitwise_xor(new_image, symbols[i].img, diffImg,
            noArray());

          diff = countNonZero(diffImg);
          if (diff < minDiff) {
            minDiff = diff;
            match = i;
          }

          if (i == 0) {

                //if (!IsPi3)   imshow("B",diffImg);


          }
        }

        //if (!IsPi3) imshow("B", correctedImg);


        if (match != -1) {
          putText(camera, symbols[match].name, Point(320, 30), 1,
            2, Scalar(0, 255, 0), 2);
          printf("Match %s\n", symbols[match].name.c_str());
					return symbols[match].name;
					//return symbols[match].name.c_str();
        } else {
					printf("No Match!\n");
				}
      }
    }
  }
  return "";
  /*
  if (0 <= maxsizeIndex) {
      printf("area[%d] %f\n", maxsizeIndex, maxsize);
  }
  */
}
/*
int main(int argc, char** argv)
{

	IplImage *         iplCameraImage;
	CvCapture *        capture = NULL;
	Mat                camera, greyImg, new_image;



   if ((argc != 3) && (argc != 1))
   {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
  }
   if (argc == 3)
   {
       if  ((UdpLocalPort=OpenUdpPort(0))==NULL)  // Open UDP Network port
         {
          printf("OpenUdpPort Failed\n");
          return(-1); 
         }

       if  ((UdpDest=GetUdpDest(argv[1],argv[2]))==NULL)  // Setup remote network destination to send images
         {
          printf("GetUdpDest Failed\n");
          return(-1); 
         }
   }

	capture = cvCreateCameraCapture(0);
	if (!capture)
	{
		fprintf(stderr, "Camera Failed to Initialize\n");
		return 0;
	}

        if (!IsPi3)
          {
	   namedWindow("A", CV_WINDOW_AUTOSIZE);
	   namedWindow("B", CV_WINDOW_AUTOSIZE);
	   namedWindow("C", CV_WINDOW_AUTOSIZE);
          }



	while (1) {


		iplCameraImage = cvQueryFrame(capture);

		camera = cv::cvarrToMat(iplCameraImage);
           if (IsPi3) flip(camera, camera,-1);    //flip(-1)=180


		cvtColor(camera, greyImg, CV_RGB2GRAY);

		//threshold(greyImg, greyImg, 100, 255, 0);

		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);


		double med;
		double sigma = 0.33;
		int lower, upper;

		/// Detect edges using canny
		med = median(greyImg);
		lower = int(max(0, (int)((1.0 - sigma) * med)));
		upper = int(min(255, (int)((1.0 + sigma) * med)));
		Canny(greyImg, canny_output, lower, upper);


#if 0
                if (!IsPi3) imshow("B", canny_output);
#endif
		/// Find contours
		findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<Point> approxRect;

		for (size_t i = 0; i < contours.size(); i++) {

			approxPolyDP(contours[i], approxRect,
				arcLength(Mat(contours[i]), true) * 0.01, true);

			if (approxRect.size() == 4) {
				double area = contourArea(contours[i]);


				if (area > 2000)

				{
					std::vector<cv::Point2f> corners;

					vector<Point>::iterator vertex;
					vertex = approxRect.begin();
					//vertex++;
					circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
					corners.push_back(*vertex);
					vertex++;
					circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
					corners.push_back(*vertex);
					vertex++;
					circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
					corners.push_back(*vertex);
					vertex++;
					circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
					corners.push_back(*vertex);

					Moments mu;
					mu = moments(contours[i], false);
					Point2f center((float)(mu.m10 / mu.m00), (float)(mu.m01 / mu.m00));

					sortCorners(corners, center);

					// Define the destination image
					Mat correctedImg = ::Mat::zeros(195, 271, CV_8UC3);

					// Corners of the destination image
					std::vector<cv::Point2f> quad_pts;
					quad_pts.push_back(Point2f(0, 0));
					quad_pts.push_back(Point2f((float)correctedImg.cols, 0.0f));
					quad_pts.push_back(Point2f((float)correctedImg.cols, (float)correctedImg.rows));
					quad_pts.push_back(Point2f(0.0f, (float)correctedImg.rows));

					// Get transformation matrix
					Mat transmtx = getPerspectiveTransform(corners, quad_pts);

					// Apply perspective transformation
					warpPerspective(camera, correctedImg, transmtx,
						correctedImg.size());

					Mat correctedImgBin;

					cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);

					//equalizeHist(correctedImgBin, correctedImgBin);

					correctedImgBin.copyTo(new_image);

					threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

					if (!IsPi3) imshow("B", correctedImgBin);


					double minVal, maxVal, medVal;


					minMaxLoc(new_image, &minVal, &maxVal);

					medVal = (maxVal - minVal) / 2;

					threshold(new_image, new_image, medVal, 255, 0);

					if (!IsPi3) imshow("C", new_image);

					Mat diffImg;

					int match, minDiff, diff;
					minDiff = 12000;
					match = -1;

					for (int i = 0; i < NUM_SIGNS; i++) {

						bitwise_xor(new_image, symbols[i].img, diffImg,
							noArray());

						diff = countNonZero(diffImg);
						if (diff < minDiff) {
							minDiff = diff;
							match = i;
						}

						if (i == 0) {

					        //if (!IsPi3) 	imshow("B",diffImg);


						}
					}

					//if (!IsPi3) imshow("B", correctedImg);


					if (match != -1) {
						putText(camera, symbols[match].name, Point(320, 30), 1,
							2, Scalar(0, 255, 0), 2);
						printf("Match %s\n", symbols[match].name.c_str());

					}
					else printf("No Match\n");


				}

			}
		}
                UdpSendImageAsJpeg(UdpLocalPort,UdpDest,camera);   // Send processed UDP image to detination

		if (!IsPi3) imshow("A", camera);

		waitKey(20);
	}
}
*/





