CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

PROJECT(yolo)

SET(DEMOPATH ..)

ADD_DEFINITIONS(-std=c++11)
SET(SENSOR_SOURCES Sensor.cpp
  ${DEMOPATH}/HC-SR04/Sonar.cpp
)
SET(SENSOR_LIBS wiringPi vl53l0x pthread)

INCLUDE_DIRECTORIES(
  ${DEMOPATH}/VL53L0X_TCA9548A_rasp-master/
	${DEMOPATH}/VL53L0X_TCA9548A_rasp-master/Api/core/inc
	${DEMOPATH}/VL53L0X_TCA9548A_rasp-master/platform/inc
  ${DEMOPATH}/HC-SR04
)

ADD_LIBRARY(sensor ${SENSOR_SOURCES})
TARGET_LINK_LIBRARIES(sensor ${SENSOR_LIBS})

SET(ROBOT_SOURCES Robot.cpp ImageProcessing.cpp PID.cpp Signs.cpp Camera.cpp
  ${DEMOPATH}/Common/Servos.cpp
	${DEMOPATH}/Common/NetworkUDP.cpp
	${DEMOPATH}/Common/UdpSendRecvJpeg.cpp
)
SET(ROBOT_LIBS sensor)

#move to Camera
INCLUDE_DIRECTORIES(
  ${DEMOPATH}/Common/
	/home/pi/git/robidouille/raspicam_cv
)
LINK_DIRECTORIES(
  /home/pi/git/robidouille/raspicam_cv
)
SET(ROBOT_LIBS ${ROBOT_LIBS}
  raspicamcv_320_240
	opencv_core
	opencv_highgui
	opencv_imgcodecs
	opencv_imgproc
	opencv_video
)

ADD_LIBRARY(robot ${ROBOT_SOURCES})
TARGET_LINK_LIBRARIES(robot ${ROBOT_LIBS})

SET(SOURCES main.cpp)

ADD_EXECUTABLE(main ${SOURCES})
TARGET_LINK_LIBRARIES(main robot)
