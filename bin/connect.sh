#!/bin/bash

RTOPIC=request
WTOPIC=response
HOST=localhost

if (( $# < 2 )); then
  echo "Usage: ${0} role executable [...]"
  echo "       role : robot | algorithm"
  echo "       executable : executable file + arguments"
  exit 1
fi

if [[ ${1} != "robot" && ${1} != "algorithm" ]]; then
  echo "Error: Unknown role name. Use robot or algorithm"
  exit 1
fi

if [[ ${1} == "robot" ]]; then
  mosquitto_sub -h ${HOST} -t robot-request | ${@:2} | \
  while read line; do
    mosquitto_pub -h ${HOST} -t robot-response -m "${line}"
  done
else
  mosquitto_sub -h ${HOST} -t algorithm-request -t robot-response | ${@:2} | \
  while read line; do
    if [[ ${line} == "current" || ${line} == "forward" || ${line} == "backword" || ${line} == "left" || ${line} == "right" || ${line} == "sign" ]]; then
      mosquitto_pub -h ${HOST} -t robot-request -m "${line}"
    else
      mosquitto_pub -h ${HOST} -t algorithm-response -m "${line}"
    fi
  done
fi

