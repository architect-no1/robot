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

int main(int argc, char *argv[]) {
  yolo::Robot robot;

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

  do {
    std::string line;
    if (haveStdin()) std::getline(std::cin, line);
    // TODO: json parse
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
