#! /bin/bash
### BEGIN INIT INFO
# Provides:          maze.sh
# Required-Start:    $mosquitto
# Required-Stop:     
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: CMU Robot Project by YOLO
### END INIT INFO

if (( $# < 1 )); then
  echo "Usage: ${0} { start | stop | restart }"
  exit 1
fi

HOME=/home/pi/robot/bin

function start {
  /etc/init.d/mosquitto start
  sleep 1
  ./connect.sh robot ./main &
  sleep 1
  ./connect.sh algorithm ./MapSolver &
  sleep 1
  python ./robotui/app.py &    
}

function stop {
    /etc/init.d/mosquitto stop
    for p in `ps ax|grep algorithm|awk { print $1; } '`; do
      kill -9 $p >/dev/null 2>&1
    done
    for p in `ps ax|grep MapSolver|awk { print $1; } '`; do
      kill -9 $p >/dev/null 2>&1
    done
    for p in `ps ax|grep robot|awk { print $1; } '`; do
      kill -9 $p >/dev/null 2>&1
    done
    for p in `ps ax|grep main|awk { print $1; } '`; do
      kill -9 $p >/dev/null 2>&1
    done
    for p in `ps ax|grep app.py|awk { print $1; } '`; do
      kill -9 $p >/dev/null 2>&1
    done
}

pushd ${HOME} > /dev/null

case "$1" in
  start)
    start
	;;
  stop)
    stop
	;;
  restart)
    stop
    sleep 1
    start
	;;
  *)
	  exit 1
esac

popd > /dev/null

exit 0
