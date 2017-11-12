#include "Camera.h"

#include <stdio.h>
#include <stdlib.h>
#include "Servos.h"

namespace yolo {

Camera::Camera() {
  if (OpenServos() == -1) {
    exit(0);
  }

  pan = SERVO_CENTER_OR_STOP; tilt = SERVO_CENTER_OR_STOP;
  ResetServos();
}

Camera::~Camera() {
  CloseServos();
}

int Camera::setPan(int p) {
  pan = p;
  SetServoPosition(CAMERA_PAN, pan);
  return pan;
}

int Camera::setTilt(int t) {
  tilt = t;
  SetServoPosition(CAMERA_TILT, tilt);
  return tilt;
}

void Camera::moveLeft() {
  pan += 3;
  SetServoPosition(CAMERA_PAN, pan);
  fprintf(stderr, "pan %d tilt %d\n", pan, tilt);
}

void Camera::moveRight() {
  pan -= 3;
  SetServoPosition(CAMERA_PAN, pan);
  fprintf(stderr, "pan %d tilt %d\n", pan, tilt);
}

void Camera::moveUp() {
  tilt -= 3;
  SetServoPosition(CAMERA_TILT, tilt);
  fprintf(stderr, "pan %d tilt %d\n", pan, tilt);
}

void Camera::moveDown() {
  tilt += 3;
  SetServoPosition(CAMERA_TILT, tilt);
  fprintf(stderr, "pan %d tilt %d\n", pan, tilt);
}

} // namespace yolo

