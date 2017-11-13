#pragma once

#include "CAbstAlgo.h"
#include <queue>
#include <string>

class CMapSolver
{
public:
    CMapSolver();
	~CMapSolver();

	void init();
	eMovCmd MapBuilder_1step(eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading);
	std::string translate_eMovCMd2String(eMovCmd mov);
	std::vector<cPoint> found_path;

private:
	CAbstAlgo * pAlgo;
	std::queue<eMovCmd> pathQueue;
	std::vector<eMovCmd> path2dir(std::vector<cPoint> path, int start_heading);
	eMovCmd makeCmd_up(int bef_heading, int * next_heading);
	eMovCmd makeCmd_down(int bef_heading, int * next_heading);
	eMovCmd makeCmd_left(int bef_heading, int * next_heading);
	eMovCmd makeCmd_right(int bef_heading, int * next_heading);
	bool checkCmdAvailable(eMovCmd cmd, eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading);

	bool checkCmdAvailable_up(eMapNode * map, int width, int cur_x, int cur_y);
	bool checkCmdAvailable_down(eMapNode * map, int width, int cur_x, int cur_y);
	bool checkCmdAvailable_left(eMapNode * map, int width, int cur_x, int cur_y);
	bool checkCmdAvailable_right(eMapNode * map, int width, int cur_x, int cur_y);
	std::vector<eMovCmd> FindPath_NearestUnknown(eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading);
    bool CheckRedDot(eMapNode * map, int width, int height, int cur_x, int cur_y);

};

