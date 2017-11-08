CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

PROJECT(yolo)

ADD_DEFINITIONS(-std=c++11)
SET(SENSOR_SOURCES Sensor.cpp
  demo/HC-SR04/Sonar.cpp
)
SET(SENSOR_LIBS wiringPi vl53l0x pthread)

INCLUDE_DIRECTORIES(
  demo/VL53L0X_TCA9548A_rasp-master/
	demo/VL53L0X_TCA9548A_rasp-master/Api/core/inc
	demo/VL53L0X_TCA9548A_rasp-master/platform/inc
  demo/HC-SR04
)

ADD_LIBRARY(sensor ${SENSOR_SOURCES})
TARGET_LINK_LIBRARIES(sensor ${SENSOR_LIBS})

SET(ROBOT_SOURCES Robot.cpp ImageProcessing.cpp PID.cpp Signs.cpp Camera.cpp
  demo/Common/Servos.cpp
  demo/Common/NetworkUDP.cpp
  demo/Common/UdpSendRecvJpeg.cpp
)
SET(ROBOT_LIBS sensor)

#move to Camera
INCLUDE_DIRECTORIES(
  demo/Common/
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

SET(CMAKE_INSTALL_PREFIX /home/pi/robot)

INSTALL(TARGETS main DESTINATION bin)
INSTALL(PROGRAMS connect.sh DESTINATION bin)
INSTALL(DIRECTORY signs DESTINATION bin)
