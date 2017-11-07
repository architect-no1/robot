#ifndef YOLO_CONFIG_H_
#define YOLO_CONFIG_H_

namespace yolo {

int BASESPEED = 6;

int TICKS_BEFORE_CROSSING_GUARD = 100;
int TICKS_AFTER_CROSSING = 20;

int TICKS_TURN = 56;

int TICKS_CHECKSIGN_MOVE = 35;
int TICKS_CHECKSIGN_TURNSERVOS = 20;
int TICKS_CHECKSIGN_CHECKSIGN = 20;

int CAMERA_CHECKSIGN_FORWARD_PAN = 150;
int CAMERA_CHECKSIGN_FORWARD_TILT = 156;

int CAMERA_CHECKSIGN_LEFT_PAN = 200;
int CAMERA_CHECKSIGN_LEFT_TILT = 150;

int CAMERA_CHECKSIGN_RIGHT_PAN = 93;
int CAMERA_CHECKSIGN_RIGHT_TILT = 162;

void readConfig(const char *filename);

} // namespace yolo

#endif // YOLO_CONFIG_H_