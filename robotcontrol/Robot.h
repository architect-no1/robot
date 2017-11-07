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

  void init();

  void goForward();
  void goLeft();
  void goRight();
  void goBackward();

  void suspend();
  void resume();

  void checkSigns();

  bool isWallFront();
  bool isWallLeft();
  bool isWallRight();

  void runOneLoop();

  void forwardComplete();
  void leftComplete();
  void rightComplete();
  void backwardComplete();

  class Listener {
  public:
    virtual void onForwardComplete() = 0;
    virtual void onLeftComplete() = 0;
    virtual void onRightComplete() = 0;
    virtual void onBackwardComplete() = 0;
  };

  void setListener(Listener *l);

  void setWheelSpeed(int left, int right);

  void followLineForwardInit();
  void followLineForward();
  void followLineForward(bool &foundCrossing);

  void stop();

  Camera c;

  class Behavior;

private:
  Listener *listener;
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
