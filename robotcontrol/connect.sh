#!/bin/bash

RTOPIC=request
WTOPIC=response

if (( $# < 2 )); then
  echo "Usage: ${0} broker_address executable [...]"
fi

mosquitto_sub -h ${1} -t ${RTOPIC} | ${@:2} | \
while read line; do
  mosquitto_pub -h ${1} -t ${WTOPIC} -m "${line}"
done

