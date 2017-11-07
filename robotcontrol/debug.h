#ifndef YOLO_DEBUG_H_
#define YOLO_DEBUG_H_

#include <stdio.h>
#include <time.h>

static struct timespec _start, _end;

static void timestart() {
  clock_gettime(CLOCK_REALTIME, &_start);
}

static void timeend() {
  clock_gettime(CLOCK_REALTIME, &_end);
}

static void timediff() {
  struct timespec diff;
  diff.tv_nsec = _end.tv_nsec - _start.tv_nsec;
  diff.tv_sec = _end.tv_sec - _start.tv_sec;
  if (diff.tv_nsec < 0) {
    diff.tv_nsec += 1000000000;
    diff.tv_sec--;
  }
  printf("time %d.%09d\n", diff.tv_sec, diff.tv_nsec);
}

#endif // YOLO_DEBUG_H_
