#ifndef _YOLO_ROBOT_H_
#define _YOLO_ROBOT_H_

#include <string>
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PI3OpencvCompat.h"
#include "UdpSendRecvJpeg.h"
#include "Sensor.h"
#include "Camera.h"
#include "PID.h"

using namespace std;

namespace yolo {

// TODO: singleton?
class Robot {
public:
  Robot();
  ~Robot();

  cv::Mat& getCamera();

  void hello();

  void goForward();
  void turnLeft();
  void turnRight();
  void uturn();

  void pause();
  void resume();

  void stop();

  void checkSigns();

  bool isWallFront();
  bool isWallLeft();
  bool isWallRight();

  void runOneLoop();

  void setWheelSpeed(int left, int right);

  void followLineForwardInit();
  void followLineForward();
  void followLineForward(bool &foundCrossing);

  Camera c;

  class Behavior;

private:
  std::shared_ptr<Behavior> behavior;

  bool isPaused;

  cv::Mat camera;
  CvCapture* capture;

  void updateCamera();

  Sensor sensor;

  TPID PID;
};

}

#endif // _YOLO_ROBOT_H_
