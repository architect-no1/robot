QT += core
QT -= gui

CONFIG += c++11

TARGET = mapSolver
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    Astar.cpp \
    CAbstAlgo.cpp \
    CFloodFill.cpp \
    CMapSolver.cpp \
    CSimulRobot.cpp \
    MapMaker.cpp \
    mapsolver_main.cpp \
    random_search.cpp

HEADERS += \
    Astar.h \
    CAbstAlgo.h \
    CFloodFill.h \
    CMapSolver.h \
    CSimulRobot.h \
    MapMaker.h \
    mapsolver_main.h \
    mapType.h \
    random_search.h
