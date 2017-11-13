#include <stdio.h>
#include "CMapSolver.h"
#include "CFloodFill.h"
#include <iostream>

CMapSolver::CMapSolver()
{
	pAlgo = new CFloodFill();
}

CMapSolver::~CMapSolver()
{
	delete pAlgo;
}

void CMapSolver::init()
{
	pathQueue.empty();
}

std::string CMapSolver::translate_eMovCMd2String(eMovCmd mov)
{
	std::string str;

	switch (mov)
	{
    case eMovCmd_FORWARD:
		str = "forward";
		break;

    case eMovCmd_LEFT:
		str = "left";
		break;

    case eMovCmd_RIGHT:
		str = "right";
		break;

    case eMovCmd_SEARCH_SIGN:
        str = "sign";
		break;

    case eMovCmd_STOP:
		str = "stop";
		break;

    case eMovCmd_UTURN:
        str = "backward";
		break;

	default:
		str = "stop";
		break;
	}

	return str;
}

bool CMapSolver::checkCmdAvailable_up(eMapNode * map, int width, int cur_x, int cur_y)
{
	bool rval = true;

    if (map[(cur_y + 1)*width + cur_x] == eMapNode_WALL)
		rval = false;

	return rval;
}

bool CMapSolver::checkCmdAvailable_down(eMapNode * map, int width, int cur_x, int cur_y)
{
	bool rval = true;

    if (map[(cur_y - 1)*width + cur_x] == eMapNode_WALL)
		rval = false;

	return rval;
}
bool CMapSolver::checkCmdAvailable_left(eMapNode * map, int width, int cur_x, int cur_y)
{
	bool rval = true;

    if (map[cur_y*width + (cur_x - 1)] == eMapNode_WALL)
		rval = false;

	return rval;
}
bool CMapSolver::checkCmdAvailable_right(eMapNode * map, int width, int cur_x, int cur_y)
{
	bool rval = true;

    if (map[cur_y*width + (cur_x+1)] == eMapNode_WALL)
		rval = false;

	return rval;
}
bool CMapSolver::checkCmdAvailable(eMovCmd cmd, eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading)
{
	bool rval = true;

	if (cur_heading == 0)
	{
        if (cmd == eMovCmd_FORWARD)
			rval = checkCmdAvailable_right(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_LEFT)
			rval = checkCmdAvailable_up(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_RIGHT)
			rval = checkCmdAvailable_down(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_UTURN)
			rval = checkCmdAvailable_left(map, width, cur_x, cur_y);
	}
	else if (cur_heading == 90)
	{
        if (cmd == eMovCmd_FORWARD)
			rval = checkCmdAvailable_up(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_LEFT)
			rval = checkCmdAvailable_left(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_RIGHT)
			rval = checkCmdAvailable_right(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_UTURN)
			rval = checkCmdAvailable_down(map, width, cur_x, cur_y);
	}
	else if (cur_heading == -90)
	{
        if (cmd == eMovCmd_FORWARD)
			rval = checkCmdAvailable_down(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_LEFT)
			rval = checkCmdAvailable_right(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_RIGHT)
			rval = checkCmdAvailable_left(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_UTURN)
			rval = checkCmdAvailable_up(map, width, cur_x, cur_y);
	}
	else //if (cur_heading == 180)
	{
        if (cmd == eMovCmd_FORWARD)
			rval = checkCmdAvailable_left(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_LEFT)
			rval = checkCmdAvailable_down(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_RIGHT)
			rval = checkCmdAvailable_up(map, width, cur_x, cur_y);
        else if (cmd == eMovCmd_UTURN)
			rval = checkCmdAvailable_right(map, width, cur_x, cur_y);
	}

	return rval;
}

bool CMapSolver::CheckRedDot(eMapNode * map, int width, int height, int cur_x, int cur_y)
{
    bool rval = false;

    if(map[cur_y*width + cur_x] == eMapNode_REDDOT)
        rval = true;

    return rval;
}

eMovCmd CMapSolver::MapBuilder_1step(eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading)
{
    eMovCmd nextMov = eMovCmd_STOP;

	if (pathQueue.size() == 0)
	{
		// there is no path
		// path should be made from map

		std::vector<eMovCmd> path = FindPath_NearestUnknown(map, width, height, cur_x, cur_y, cur_heading);

		if (path.size() > 0)
		{
            // check current position RED DOT
            // if RED DOT, add search command
            if(CheckRedDot(map, width, height, cur_x, cur_y) == true)
                pathQueue.push(eMovCmd_SEARCH_SIGN);

			for (size_t i = 0; i < path.size(); i++)
				pathQueue.push(path[i]);
		}
		else
		{
            fprintf(stderr, "!!! There is NO path\r\n");
		}
	}

	if (pathQueue.size() > 0)
	{
		nextMov = pathQueue.front();
		pathQueue.pop();

        // check cannot move situation
		if (checkCmdAvailable(nextMov, map, width, height, cur_x, cur_y, cur_heading) == false)
		{
            fprintf(stderr, "Cannot Move!!! make new path !! \r\n");

			// pathQueue.clear()
			while (pathQueue.size() > 0) pathQueue.pop();

			std::vector<eMovCmd> path = FindPath_NearestUnknown(map, width, height, cur_x, cur_y, cur_heading);

			if (path.size() > 0)
			{
				for (size_t i = 0; i < path.size(); i++)
					pathQueue.push(path[i]);

                nextMov = pathQueue.front();
                pathQueue.pop();
			}
			else
			{
                fprintf(stderr, "!!! There is NO path\r\n");
			}
		}
	}

	return nextMov;
}

eMovCmd CMapSolver::makeCmd_up(int bef_heading, int * next_heading)
{
	eMovCmd cmd;

	switch (bef_heading)
	{
	case 0:
        cmd = eMovCmd_LEFT;
		*next_heading = bef_heading + 90;
		break;
	case 90:
        cmd = eMovCmd_FORWARD;
		*next_heading = bef_heading;
		break;
	case -90:
        cmd = eMovCmd_UTURN;
		*next_heading = bef_heading + 180;
		break;
	default:
        cmd = eMovCmd_RIGHT;
		*next_heading = bef_heading - 90;
		break;
	}

	while (*next_heading > 180)
		*next_heading -= 360;
	while (*next_heading < -180)
		*next_heading += 360;


	return cmd;
}

eMovCmd CMapSolver::makeCmd_left(int bef_heading, int * next_heading)
{
	eMovCmd cmd;

	switch (bef_heading)
	{
	case 0:
        cmd = eMovCmd_UTURN;
		*next_heading = bef_heading + 180;
		break;
	case 90:
        cmd = eMovCmd_LEFT;
		*next_heading = bef_heading + 90;
		break;
	case -90:
        cmd = eMovCmd_RIGHT;
		*next_heading = bef_heading - 90;
		break;
	default:
        cmd = eMovCmd_FORWARD;
		*next_heading = bef_heading;
		break;
	}

	while (*next_heading > 180)
		*next_heading -= 360;
	while (*next_heading < -180)
		*next_heading += 360;

	return cmd;
}

eMovCmd CMapSolver::makeCmd_right(int bef_heading, int * next_heading)
{
	eMovCmd cmd;

	switch (bef_heading)
	{
	case 0:
        cmd = eMovCmd_FORWARD;
		*next_heading = bef_heading;
		break;
	case 90:
        cmd = eMovCmd_RIGHT;
		*next_heading = bef_heading - 90;
		break;
	case -90:
        cmd = eMovCmd_LEFT;
		*next_heading = bef_heading + 90;
		break;
	default:
        cmd = eMovCmd_UTURN;
		*next_heading = bef_heading + 180;
		break;
	}

	while (*next_heading > 180)
		*next_heading -= 360;
	while (*next_heading < -180)
		*next_heading += 360;

	return cmd;
}

eMovCmd CMapSolver::makeCmd_down(int bef_heading, int * next_heading)
{
	eMovCmd cmd;

	switch (bef_heading)
	{
	case 0:
        cmd = eMovCmd_RIGHT;
		*next_heading = bef_heading - 90;
		break;
	case 90:
        cmd = eMovCmd_UTURN;
		*next_heading = bef_heading + 180;
		break;
	case -90:
        cmd = eMovCmd_FORWARD;
		*next_heading = bef_heading;
		break;
	default:
        cmd = eMovCmd_LEFT;
		*next_heading = bef_heading + 90;
		break;
	}

	while (*next_heading > 180)
		*next_heading -= 360;
	while (*next_heading < -180)
		*next_heading += 360;

	return cmd;
}


std::vector<eMovCmd> CMapSolver::path2dir(std::vector<cPoint> path, int start_heading)
{
	std::vector<eMovCmd> dirPath;
	int bef_heading = start_heading;
	int next_heading;

	if (path.size() > 0)
	{
		for (size_t i = 1; i < path.size(); i++)
		{
            eMovCmd mov = eMovCmd_STOP;

			int dx = path[i].x - path[i - 1].x;
			int dy = path[i].y - path[i - 1].y;

			if(dx == 0 && dy == 0)
                mov = eMovCmd_STOP;
			else if (dx == 0 && dy > 0)
				mov = makeCmd_up(bef_heading, &next_heading);
			else if(dx == 0 && dy < 0)
				mov = makeCmd_down(bef_heading, &next_heading);
			else if (dx < 0 && dy == 0)
				mov = makeCmd_left(bef_heading, &next_heading);
			else //if (dx == 0 && dy == -1)
				mov = makeCmd_right(bef_heading, &next_heading);

			dirPath.push_back(mov);

			bef_heading = next_heading;
		}
	}

	return dirPath;
}

std::vector<eMovCmd> CMapSolver::FindPath_NearestUnknown(eMapNode * map, int width, int height, int cur_x, int cur_y, int cur_heading)
{
	std::vector<eMovCmd> cmdPath;
    found_path = pAlgo->solve(map, width, height, cur_x, cur_y, cur_heading, eMapNode_UNKNOWN);
	cmdPath = path2dir(found_path, cur_heading);

	return cmdPath;
}

