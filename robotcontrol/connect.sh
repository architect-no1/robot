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

RTOPIC=${1}-request
WTOPIC=${1}-response

mosquitto_sub -h ${HOST} -t ${RTOPIC} | ${@:2} | \
while read line; do
  mosquitto_pub -h ${HOST} -t ${WTOPIC} -m "${line}"
done

