#include "Sensor.h"

#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "Sonar.h"
#include "user_api/vl53l0x.h"

#define ObjectNum_0 0
#define ObjectNum_1 1

#define VL53L0X_GOOD_ACCURACY_MODE       0  // Good Accuracy mode
#define VL53L0X_BETTER_ACCURACY_MODE     1  // Better Accuracy mode
#define VL53L0X_BEST_ACCURACY_MODE       2  // Best Accuracy mode
#define VL53L0X_LONG_RANGE_MODE          3  // Longe Range mode
#define VL53L0X_HIGH_SPEED_MODE          4  // High Speed mode

namespace yolo {

Sensor::Sensor()
    : front(-1)
    , left(-1)
    , right(-1) {
  // front sensor
  if (wiringPiSetup() == -1) {
    fprintf(stderr, "front sensor init fail\n");
    exit(0);
  }
  int trigger = 28;
  int echo = 29;
  SonarInit(trigger, echo);

  // left/right sensor
  if(VL53L0X_i2c_init("/dev/i2c-1") == -1) {
    printf("VL53L0X_i2c_init failed\n");
    exit(0);
  }
  // Start ranging on TCA9548A bus 1	
  startRanging(ObjectNum_0, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 1, 0x70);
  // Start ranging on TCA9548A bus 2
  startRanging(ObjectNum_1, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 2, 0x70);

  startThread();
}

Sensor::~Sensor() {
  stopThread();
}

void Sensor::print() {
  printf("front %d left %d right %d\n", front, left, right);
}

void Sensor::update() {
  int sensor = SonarDistance(30000);
  if (sensor != -1)
    front = sensor;

  // by millimeters
  // sensor work by timer thus affect fps. consider doing in another thread?
  left = getDistance(ObjectNum_1) / 10;
  right = getDistance(ObjectNum_0) / 10;
}

void Sensor::startThread() {
  run = true;
  updateThread = std::thread(&Sensor::_update, this);
}

void Sensor::stopThread() {
  run = false;
  updateThread.join();
}

void Sensor::_update() {
  while (run) {
    update();
#define SENSOR_TIMEOUT_MILLISECONDS 50
    usleep(SENSOR_TIMEOUT_MILLISECONDS * 1000);
  }
}

} // namespace yolo

