
#include "Robot.h"

#include "Config.h"
#include <unistd.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "Servos.h"
#include "ImageProcessing.h"
#include "Signs.h"
#include "PID.h"

#include "debug.h"

#define log(...) fprintf(stderr, __VA_ARGS__)
#define ack(_b) \
  do {\
    fprintf(stdout, "ack " _b "\n" );\
    fflush(stdout);\
  } while (0)\

namespace yolo {

// Parameters for PID
#define KP        1.0               // defaut 1.0
#define KI        0.1               // defaut 0.1
#define KD        0.0               // defaut 0.0
#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25

Robot::Robot()
    : isPaused(false)
    , capture(NULL) {
  // pan tilt
  if (OpenServos() == -1) {
    log("log pan tilt servos init fail\n");
    exit(0);
  }
  ResetServos();
  sleep(1);
  c.setPan(TRK_LINE_CAM_PAN);
  c.setTilt(TRK_LINE_CAM_TILT);

  // camera
  capture = cvCreateCameraCapture(0);   // Open default Camera
  if(!capture) {
    log("log camera init fail\n");
    exit(0);
  }

  int width = 640;
  int height = 320;
  if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width) == 0) {
    log("log capture width init fail\n");
  }

  if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height) == 0) {
    log("log capture height init fail\n");
  }

  width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
  height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
}

Robot::~Robot() {
  if (capture) {
    cvReleaseCapture(&capture);
    capture = NULL;
  }
  CloseServos();
}

cv::Mat& Robot::getCamera() {
  return camera;
}

void Robot::hello() {
  ack("hello");
}

void Robot::updateCamera() {
  IplImage *iplCamera = cvQueryFrame(capture);
  camera = cv::cvarrToMat(iplCamera);
  flip(camera, camera,-1);
}

bool Robot::isWallFront() {
#define WALL_DISTANCE_MAX 10
  //return false;
  return sensor.front < WALL_DISTANCE_MAX;
}

bool Robot::isWallLeft() {
  //return false;
  return sensor.left/10 < WALL_DISTANCE_MAX;
}

bool Robot::isWallRight() {
  //return false;
  return sensor.right/10 < WALL_DISTANCE_MAX;
}

void Robot::pause() {
  setWheelSpeed(0, 0);
  isPaused = true;
  ack("pause");
}

void Robot::resume() {
  isPaused = false;
  ack("resume");
}

class Robot::Behavior {
public:
  Behavior(Robot *robot) : r(robot) {}
  virtual void tick() = 0;

protected:
  Robot *r;
};

void Robot::runOneLoop() {
  updateCamera();
  if (behavior && !isPaused) behavior->tick();
}

void Robot::setWheelSpeed(int left, int right) {
  SetWheelSpeed(left, right);
}

void Robot::stop() {
  setWheelSpeed(0, 0);
  behavior.reset();
}

static bool findRed(cv::Mat &camera) {
  cv::Rect RoiRec(10, 2*camera.rows/3, camera.cols-20, camera.rows/12);
  cv::Mat roi(camera, RoiRec);
  cv::Mat hsv;
  cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
  cv::Mat mask1, mask2;
  inRange(hsv, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mask1);
  inRange(hsv, cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255), mask2);
  cv::Mat mask = mask1 | mask2;

  // FIXME: check by average != black?
  double min, max;
  minMaxLoc(mask, &min, &max);
  return max != 0.0;
}

static bool findGreen(cv::Mat &camera) {
  cv::Rect RoiRec(10, 2*camera.rows/3, camera.cols-20, camera.rows/12);
  cv::Mat roi(camera, RoiRec);
  cv::Mat hsv;
  cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
  cv::Mat mask;
  inRange(hsv, cv::Scalar(35, 220, 145), cv::Scalar(45, 250, 180), mask);

  double min, max;
  minMaxLoc(mask, &min, &max);

  return max != 0.0;
}

static bool findBlue(cv::Mat &camera) {
  cv::Rect RoiRec(10, 2*camera.rows/3, camera.cols-20, camera.rows/12);
  cv::Mat roi(camera, RoiRec);
  cv::Mat hsv;
  cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
  cv::Mat mask;
  inRange(hsv, cv::Scalar(55, 10, 140), cv::Scalar(80, 30, 170), mask);

  double min, max;
  minMaxLoc(mask, &min, &max);

  return max != 0.0;
}

void Robot::followLineForwardInit() {
  InitPID(PID,KP,KI,KD,BASESPEED,BASESPEEDFUDGEFACTOR);
}

void Robot::followLineForward() {
  float offset = FindLineInImageAndComputeOffset(getCamera());
  SetError(PID,offset);
  double correction, left, right;
  correction = RunPID(PID);
  left = BASESPEED - correction;
  right = BASESPEED + correction;
  setWheelSpeed(left, right);
}

void Robot::followLineForward(bool &foundCrossing) {
  float offset = FindLineInImageAndComputeOffset2(getCamera(), foundCrossing);
  SetError(PID,offset);
  double correction, left, right;
  correction = RunPID(PID);
  left = BASESPEED - correction;
  right = BASESPEED + correction;
  setWheelSpeed(left, right);
}

class ForwardBehavior : public Robot::Behavior {
public:
  ForwardBehavior(Robot* r) : Behavior(r),
      ticks(0), state(BEFORE_CROSSING) {
    //FIXME: may cause problem on how command is queued. create init state?
    r->followLineForwardInit();

    timestart();
  }

  ~ForwardBehavior() {
    timeend();
    timediff();
    ack("forward");
  }

  virtual void tick() {
    ticks++;
    switch (state) {
      case BEFORE_CROSSING: {
        bool foundRed = findRed(r->getCamera());
        bool foundCrossing = false;
        r->followLineForward(foundCrossing);
        if (foundRed || foundCrossing) {
          state = AFTER_CROSSING;
          ticks = 0;
        }
        if (TICKS_BEFORE_CROSSING_GUARD <= ticks) {
          r->stop();
        }
      }
      break;
      case AFTER_CROSSING:
        r->followLineForward();
        if (TICKS_AFTER_CROSSING <= ticks) {
          r->stop();
        }
      break;
      default:
        // r->stopError();
      break;
    }
  }

private:
  int ticks;
  enum {
    BEFORE_CROSSING,
    AFTER_CROSSING,
  } state;
};

// NOTE: goForwardInternal?
//
// TODO: repeat updateCamera - checksensor - sendimage?
void Robot::goForward() {
  if (behavior) return;
  behavior = std::make_shared<ForwardBehavior>(this);
}

class TurnBehavior : public Robot::Behavior {
public:
  TurnBehavior(Robot* r, bool _isLeft) : Behavior(r),
      ticks(0), state(TURN), isLeft(_isLeft) {
    r->followLineForwardInit();
  }

  ~TurnBehavior() {
    if (isLeft) ack("left");
    else ack("right");
  }
  virtual void tick() {
    ticks++;
    switch (state) {
      case TURN:
        if (isLeft)
          r->setWheelSpeed(0, BASESPEED);
        else
          r->setWheelSpeed(BASESPEED, 0);
        if (TICKS_TURN <= ticks) {
          ticks = 0;
          state = BEFORE_CROSSING;
        }
        break;
      case BEFORE_CROSSING: {
        bool foundRed = findRed(r->getCamera());
        bool foundCrossing = false;
        r->followLineForward(foundCrossing);
        if (foundRed || foundCrossing) {
          state = AFTER_CROSSING;
          ticks = 0;
        }
        if (TICKS_BEFORE_CROSSING_GUARD <= ticks) {
          r->stop();
        }
      }
        break;
      case AFTER_CROSSING:
        r->followLineForward();
        if (TICKS_AFTER_CROSSING <= ticks) {
          r->stop();
        }
        break;
      default:
        // r->stopError();
        break;
    }
  }

private:
  int ticks;
  enum {
    TURN,
    BEFORE_CROSSING,
    AFTER_CROSSING,
  } state;
  bool isLeft;
};

void Robot::turnLeft() {
  if (behavior) return;
  behavior = std::make_shared<TurnBehavior>(this, true);
}

void Robot::turnRight() {
  if (behavior) return;
  behavior = std::make_shared<TurnBehavior>(this, false);
}

class UturnBehavior : public Robot::Behavior {
public:
  UturnBehavior(Robot* r) : Behavior(r),
      ticks(0), state(UTURN1) { }

  ~UturnBehavior() {
    ack("uturn");
  }

  virtual void tick() {
    ticks++;
    switch (state) {
      case UTURN1:
        r->setWheelSpeed(-BASESPEED, 0);
        if (TICKS_TURN <= ticks) {
          ticks = 0;
          state = UTURN2;
        }
        break;
      case UTURN2:
        r->setWheelSpeed(0, BASESPEED);
        if (TICKS_TURN <= ticks) {
          r->stop();
        }
        break;
      default:
        // r->stopError();
        break;
    }
  }

private:
  int ticks;
  enum {
    UTURN1,
    UTURN2,
  } state;
};

void Robot::uturn() {
  if (behavior) return;
  behavior = std::make_shared<UturnBehavior>(this);
}

class CheckSignBehavior : public Robot::Behavior {
public:
  CheckSignBehavior(Robot* r) : Behavior(r),
      ticks(0), state(MOVE_BACK) {
    r->followLineForwardInit();
  }
  ~CheckSignBehavior() {
    printf("%s, %s, %s\n",
        signForward.c_str(),
        signLeft.c_str(),
        signRight.c_str());
    ack("check");
  }

  virtual void tick() {
    ticks++;
    switch (state) {
      case MOVE_BACK:
        r->setWheelSpeed(-BASESPEED, -BASESPEED);
        if (TICKS_CHECKSIGN_MOVE <= ticks) {
          r->setWheelSpeed(0, 0);
          state = LOOK_FORWARD;
          ticks = 0;
        }
        break;
      case LOOK_FORWARD:
        r->c.setPan(CAMERA_CHECKSIGN_FORWARD_PAN);
        r->c.setTilt(CAMERA_CHECKSIGN_FORWARD_TILT);
        if (TICKS_CHECKSIGN_TURNSERVOS <= ticks) {
          state = CHECK_SIGN_FORWARD;
          ticks = 0;
        }
        break;
      case CHECK_SIGN_FORWARD:
        signForward = getSign(r->getCamera());
        if (0 < signForward.length() || TICKS_CHECKSIGN_CHECKSIGN <= ticks) {
          state = LOOK_LEFT;
          ticks = 0;
        }
        break;
      case LOOK_LEFT:
        r->c.setPan(CAMERA_CHECKSIGN_LEFT_PAN);
        r->c.setTilt(CAMERA_CHECKSIGN_LEFT_TILT);
        if (TICKS_CHECKSIGN_TURNSERVOS <= ticks) {
          state = CHECK_SIGN_LEFT;
          ticks = 0;
        }
        break;
      case CHECK_SIGN_LEFT:
        signLeft = getSign(r->getCamera());
        if (0 < signLeft.length() || TICKS_CHECKSIGN_CHECKSIGN <= ticks) {
          state = LOOK_RIGHT;
          ticks = 0;
        }
        break;
      case LOOK_RIGHT:
        r->c.setPan(CAMERA_CHECKSIGN_RIGHT_PAN);
        r->c.setTilt(CAMERA_CHECKSIGN_RIGHT_TILT);
        if (TICKS_CHECKSIGN_TURNSERVOS <= ticks) {
          state = CHECK_SIGN_RIGHT;
          ticks = 0;
        }
        break;
      case CHECK_SIGN_RIGHT:
        signRight = getSign(r->getCamera());
        if (0 < signRight.length() || TICKS_CHECKSIGN_CHECKSIGN <= ticks) {
          state = LOOK_RESET;
          ticks = 0;
        }
        break;
      case LOOK_RESET:
        r->c.setPan(TRK_LINE_CAM_PAN);
        r->c.setTilt(TRK_LINE_CAM_TILT);
        if (TICKS_CHECKSIGN_TURNSERVOS <= ticks) {
          state = MOVE_FRONT;
          ticks = 0;
        }
        break;
      case MOVE_FRONT:
        //r->setWheelSpeed(BASESPEED, BASESPEED);
        r->followLineForward();
        if (TICKS_CHECKSIGN_MOVE <= ticks) {
          r->stop();
        }
        break;
      default:
        break;
    }
  }

private:
  int ticks;
  enum {
    MOVE_BACK,
    LOOK_FORWARD,
    CHECK_SIGN_FORWARD,
    LOOK_LEFT,
    CHECK_SIGN_LEFT,
    LOOK_RIGHT,
    CHECK_SIGN_RIGHT,
    LOOK_RESET,
    MOVE_FRONT,
  } state;

  std::string signForward;
  std::string signLeft;
  std::string signRight;
};

void Robot::checkSigns() {
  if (behavior) return;
  behavior = std::make_shared<CheckSignBehavior>(this);
}

} // namespace yolo
