#include <stdio.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>
#include <iostream>
#include <string>

#include "Robot.h"

static bool haveStdin() {
  fd_set fds;
  struct timeval tv;
  int ret;

  FD_ZERO(&fds);
  FD_SET(0, &fds);

  tv.tv_sec = 0;
  tv.tv_usec = 1;

  ret = select(1, &fds, NULL, NULL, &tv);

  return 0 < ret;
}

class EventHandler : public yolo::Robot::Listener {
private:
  yolo::Robot *r;

public:
  EventHandler(yolo::Robot *robot) : r(robot) { }

  std::string wallsToMapString() {
    std::string ret;
    ret = r->isWallFront() ? ". X . " : ". O . ";
    ret += r->isWallLeft() ? "X O "   : "O O ";
    ret += r->isWallRight()?     "X " :     "O ";
    ret += ". . .";
    return ret;
  }

  void onMoveComplete(const char *move) {
    printf("ack %s %s\n", move, wallsToMapString().c_str());
    fflush(stdout);
  }

  virtual void onForwardComplete() {
    onMoveComplete("forward");
  }

  virtual void onLeftComplete() {
    onMoveComplete("left");
  }

  virtual void onRightComplete() {
    onMoveComplete("right");
  }

  virtual void onBackwardComplete() {
    onMoveComplete("backward");
  }

  virtual void onCheckSignComplete(std::string forward,
                                   std::string left,
                                   std::string right) {
    std::string signmap = "";
    if (0 < forward.length()) signmap += "dw" + forward + " ";
    if (0 < left.length()) signmap += "da" + left + " ";
    if (0 < right.length()) signmap += "dd" + right + " ";

    if (0 == signmap.length()) {
      // TODO: error
    }
  }

};

int main(int argc, char *argv[]) {
  // TODO: move robot inside EventHandler
  yolo::Robot robot;

  EventHandler e(&robot);
  robot.setListener(&e);

  const char *ip = "172.20.10.2", *port = "3000";
  if (3 <= argc) {
    ip = argv[1];
    port = argv[2];
  }

  TUdpLocalPort *UdpLocalPort;
  TUdpDest *UdpDest;

  if ((UdpLocalPort = OpenUdpPort(0)) == NULL) {
    printf("OpenUdpPort Failed\n");
    return 0;
  }

  if ((UdpDest = GetUdpDest(ip, port)) == NULL) {
    printf("GetUdpDest Failed\n");
    return 0;
  }

  // TODO: move this loop to EventHandler
  do {
    std::string line;
    if (haveStdin()) std::getline(std::cin, line);
    if (line.find("q") != std::string::npos) {
      break;
    } else if (line.find("hello") != std::string::npos) {
      robot.hello();
    } else if (line.find("init") != std::string::npos) {
      robot.init();
    } else if (line.find("forward") != std::string::npos) {
      robot.goForward();
    } else if (line.find("left") != std::string::npos) {
      robot.goLeft();
    } else if (line.find("right") != std::string::npos) {
      robot.goRight();
    } else if (line.find("backward") != std::string::npos) {
      robot.goBackward();
    } else if (line.find("check") != std::string::npos) {
      robot.checkSigns();
    } else if (line.find("suspend") != std::string::npos) {
      robot.suspend();
    } else if (line.find("resume") != std::string::npos) {
      robot.resume();
    } else if (line == "i") {
      robot.c.moveUp();
    } else if (line == "m") {
      robot.c.moveDown();
    } else if (line == "j") {
      robot.c.moveLeft();
    } else if (line == "l") {
      robot.c.moveRight();
    }

    robot.runOneLoop();
    UdpSendImageAsJpeg(UdpLocalPort, UdpDest, robot.getCamera());

    // TODO: write sensor update
  } while (1);

  return 0;
}
