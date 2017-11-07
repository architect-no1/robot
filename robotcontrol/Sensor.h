#ifndef YOLO_SENSOR_H_
#define YOLO_SENSOR_H_

#include <thread>

namespace yolo {

class Sensor {
public:
  Sensor();
  ~Sensor();

  void print();

  int front;
  int left;
  int right;

private:
  void update();

  std::thread updateThread;
  bool run;
  void startThread();
  void _update();
  void stopThread();
};

} // namespace yolo

#endif // YOLO_SENSOR_H_
