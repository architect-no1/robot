#ifndef YOLO_CAMERA_H_
#define YOLO_CAMERA_H_

namespace yolo {

class Camera {
public:
  Camera();
  ~Camera();

  int pan;
  int tilt;

  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();

  int setPan(int p);
  int setTilt(int t);
};

} // namespace yolo

#endif // YOLO_CAMERA_H_
