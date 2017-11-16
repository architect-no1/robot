#pragma once

#include "CAbstAlgo.h"
#include "mapType.h"

class CSimulRobot
{
public:
	CSimulRobot();
	~CSimulRobot();

	void move(eMovCmd mov);
	void makeEnvInfo(CEnvInfo * pEnvInfo);
    void DrawMap();

	int cur_x;
	int cur_y;
	int cur_heading;

private:
	void forward(int heading, int * next_x, int *next_y, int *next_heading);
	void left(int heading, int * next_x, int *next_y, int *next_heading);
	void right(int heading, int * next_x, int *next_y, int *next_heading);
	void uturn(int heading, int * next_x, int *next_y, int *next_heading);

	void makeEnvInfo_dir(CEnvInfo * pEnvinfo, int heading, int up, int down, int left, int right);

    std::vector<cPoint> robotPath;

};

