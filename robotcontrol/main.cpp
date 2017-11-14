#include <stdio.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>
#include <iostream>
#include <string>

#include "Robot.h"
#include "Config.h"

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
  yolo::Robot r;

  TUdpLocalPort *UdpLocalPort;
  TUdpDest *UdpDest;

public:
  EventHandler(const char *ip, const char *port) {
    r.setListener(this);
    yolo::loadConfig();

    if ((UdpLocalPort = OpenUdpPort(0)) == NULL) {
      printf("OpenUdpPort Failed\n");
      exit(0);
    }

    if ((UdpDest = GetUdpDest(ip, port)) == NULL) {
      printf("GetUdpDest Failed\n");
      exit(0);
    }
  }

  std::string wallsToMapString(bool isRedDot, bool isStart, bool isEnd) {
    char rp = 'o';
    if (isRedDot) rp = 'j';
    else if (isStart) rp = 'b';
    else if (isEnd) rp = 'f';

    std::string ret;
    ret = r.isWallFront() ? ". x . " : ". o . ";
    ret += r.isWallLeft() ? "x "     : "o ";
    ret += rp;
    ret += r.isWallRight()?    " x " :    " o ";
    ret += ". o .";
    return ret;
  }

  void onMoveComplete(const char *move,
                      bool isRedDot, bool isStart, bool isEnd) {
    printf("ack %s %s\n",
           move, wallsToMapString(isRedDot, isStart, isEnd).c_str());
    fflush(stdout);
  }

  virtual void onForwardComplete(bool isRedDot, bool isStart, bool isEnd) {
    onMoveComplete("forward", isRedDot, isStart, isEnd);
  }

  virtual void onLeftComplete(bool isRedDot, bool isStart, bool isEnd) {
    onMoveComplete("left", isRedDot, isStart, isEnd);
  }

  virtual void onRightComplete(bool isRedDot, bool isStart, bool isEnd) {
    onMoveComplete("right", isRedDot, isStart, isEnd);
  }

  virtual void onBackwardComplete() {
    onMoveComplete("backward", false, false, false);
  }

  void capture(cv::Mat &mat) {
    static int id = 0;
    std::string filename = "capture/";
    filename += id;
    filename += ".jpg";
    imwrite(filename.c_str(), mat);
  }

  virtual void onCheckSignComplete(std::string forward,
                                   std::string left,
                                   std::string right) {
    std::string signmap = "";
    if (0 < forward.length()) signmap += "w" + forward;
    else if (0 < left.length()) signmap += "a" + left;
    else if (0 < right.length()) signmap += "d" + right;

    if (0 == signmap.length()) {
      // TODO: error
      printf("ack sign cannot\n");
    } else {
      printf("ack sign %s\n", signmap.c_str());
    }
    fflush(stdout);
  }

  void processCommand(std::string cmd) {
    if (cmd.find("hello") != std::string::npos) {
      r.hello();
      printf("ack hello\n");
      fflush(stdout);
    } else if (cmd == "servoup") {
      r.c.moveUp();
      printf("ack servoup\n");
      fflush(stdout);
    } else if (cmd == "servodown") {
      r.c.moveDown();
      printf("ack servodown\n");
      fflush(stdout);
    } else if (cmd == "servoleft") {
      r.c.moveLeft();
      printf("ack servoleft\n");
      fflush(stdout);
    } else if (cmd == "servoright") {
      r.c.moveRight();
      printf("ack servoright\n");
      fflush(stdout);
    } else if (cmd.find("current") != std::string::npos) {
      char rp = 'o';
      if (yolo::findGreen(r.getCamera())) rp = 'b';
      std::string ret;
      ret = r.isWallFront() ? ". x . " : ". o . ";
      ret += r.isWallLeft() ? "x "     : "o ";
      ret += rp;
      ret += r.isWallRight()?    " x " :    " o ";
      ret += ". - .";
      printf("ack %s %s\n", "current", ret.c_str());
      fflush(stdout);
    } else if (cmd.find("init") != std::string::npos) {
      r.init();
    } else if (cmd.find("forward") != std::string::npos) {
      if (r.isWallFront()) {
        printf("ack forward cannot\n");
        fflush(stdout);
      } else {
        r.goForward();
      }
    } else if (cmd.find("left") != std::string::npos) {
      if (r.isWallLeft()) {
        printf("ack left cannot\n");
        fflush(stdout);
      } else {
        r.goLeft();
      }
    } else if (cmd.find("right") != std::string::npos) {
      if (r.isWallRight()) {
        printf("ack right cannot\n");
        fflush(stdout);
      } else {
        r.goRight();
      }
    } else if (cmd.find("backward") != std::string::npos) {
      r.goBackward();
    } else if (cmd.find("sign") != std::string::npos) {
      r.checkSigns();
    } else if (cmd.find("pause") != std::string::npos) {
      r.pause();
      printf("ack pause\n");
      fflush(stdout);
    } else if (cmd.find("resume") != std::string::npos) {
      r.resume();
    } else if (cmd.find("stop") != std::string::npos) {
      r.stop();
      printf("ack stop\n");
      fflush(stdout);
    } else if (cmd.find("capture") != std::string::npos) {
      capture(r.getCamera());
    }
  }

  void run() {
    do {
      if (haveStdin()) {
        std::string line;
        std::getline(std::cin, line);
        processCommand(line);
      }

      r.runOneLoop();
      UdpSendImageAsJpeg(UdpLocalPort, UdpDest, r.getCamera());

      // TODO: write sensor update
    } while (1);
  }

};

int main(int argc, char *argv[]) {
  const char *ip = "172.20.10.2", *port = "3000";
  if (3 <= argc) {
    ip = argv[1];
    port = argv[2];
  }

  EventHandler e(ip, port);;
  e.run();

  return 0;
}
