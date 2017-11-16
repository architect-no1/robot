#include "Config.h"

#include <fstream>
#include <map>
#include <string>

namespace yolo {

int BASESPEED = 6;

int TICKS_FORWARD_BEFORE_CROSSING_GUARD = 60;
int TICKS_AFTER_CROSSING = 21;

int TICKS_TURN_TURN = 52;
int TICKS_TURN_BEFORE_CROSSING_GUARD = 24;

int TICKS_BACKWARD_TURN = 52;
int TICKS_BACKWARD_FORWARD = 5;

int TICKS_CHECKSIGN_MOVE = 35;
int TICKS_CHECKSIGN_TURNSERVOS = 20;
int TICKS_CHECKSIGN_CHECKSIGN = 20;

int CAMERA_CHECKSIGN_FORWARD_PAN = 150;
int CAMERA_CHECKSIGN_FORWARD_TILT = 156;

int CAMERA_CHECKSIGN_LEFT_PAN = 200;
int CAMERA_CHECKSIGN_LEFT_TILT = 150;

int CAMERA_CHECKSIGN_RIGHT_PAN = 93;
int CAMERA_CHECKSIGN_RIGHT_TILT = 162;


static std::map<std::string, int> tag_map;

int loadConfig() {
  std::ifstream file("config.txt");
  std::string str;
  while (std::getline(file, str)) {
    std::string key;

    int positionOfEquals = str.find("=");

    key = str.substr(0, positionOfEquals);

    if (positionOfEquals != std::string::npos) {
      int value = std::stoi(str.substr(positionOfEquals + 1));
      tag_map[key] = value;

#define ASSIGN_CONFIG(x) \
      do { \
        if (key.compare(#x) == 0) x = value; \
      } while (0)
      ASSIGN_CONFIG(BASESPEED);
      ASSIGN_CONFIG(TICKS_FORWARD_BEFORE_CROSSING_GUARD);
      ASSIGN_CONFIG(TICKS_AFTER_CROSSING);
      ASSIGN_CONFIG(TICKS_TURN_TURN);
      ASSIGN_CONFIG(TICKS_TURN_BEFORE_CROSSING_GUARD);
      ASSIGN_CONFIG(TICKS_BACKWARD_TURN);
      ASSIGN_CONFIG(TICKS_BACKWARD_FORWARD);
      ASSIGN_CONFIG(TICKS_CHECKSIGN_MOVE);
    }
  }
}

} // namespace yolo
