#include <stdio.h>
#include "CFloodFill.h"
#include <iostream>
#include <queue>
#include <string.h>

CFloodFill::CFloodFill()
{
}


CFloodFill::~CFloodFill()
{
}

bool CFloodFill::checkAvailable(eMapNode *map, int * dirmap, int w, int h, int x, int y)
{
	bool rval = false;

	if (x >= 0 && x < w && y >= 0 && y < h
        && map[y*w + x] != eMapNode_WALL
		&& dirmap[y*w + x] == 0)
	{
		rval = true;
	}

	return rval;
}

bool CFloodFill::checkAlreadyChecked(int * dirmap, int w, int h, int x, int y)
{
	bool rval = false;

	if (x >= 0 && x < w && y >= 0 && y < h)
	{
		if(dirmap[y*w + x] == 0)
			rval = true;
	}
	else
		rval = true;

	return rval;
}

std::vector<cPoint> CFloodFill::makePath(int * dirmap, int w, int gx, int gy)
{
	std::vector<cPoint> path;
	std::vector<cPoint> path_inv;
	int cx = gx;
	int cy = gy;
	bool looplife = true;

	while (looplife)
	{
		path_inv.push_back(cPoint(cx, cy));

		switch (dirmap[cy*w + cx])
		{
		case 1: // left
			cx += 2;
			break;
		case 2: // up
			cy -= 2;
			break;
		case 3: // right
			cx -= 2;
			break;
		case 4: // down
			cy += 2;
			break;
		case 9:
			looplife = false;
			break;
		}
	}

	for (int i = path_inv.size()-1; i >= 0; i--)
		path.push_back(path_inv[i]);

	return path;
}

bool CFloodFill::checkUncertainUTurn(eMapNode *map, int w, int x, int y)
{
    bool rval = false;

    if(map[y*w+x] == eMapNode_UNKNOWN
        || map[y*w+x] == eMapNode_BACK
        || map[y*w+x] == eMapNode_WALL)
    {
        rval = true;
    }

    return rval;

}

std::vector<cPoint> CFloodFill::solve(eMapNode * map, int width, int height
									, int start_x, int start_y, int start_heading, eMapNode tarType)
{
    float firstFlag = true;
	std::vector<cPoint> path;
	bool looplife = true;
	cPoint goalpt;
	bool found_goal = false;

	int * dirmap = new int[width*height]; // 1:left, 2:up, 3:right, 4:down, 9:start
	memset(dirmap, 0, sizeof(int) * width*height);

	std::queue<cPoint> que;

	que.push(cPoint(start_x, start_y));

	dirmap[start_y*width + start_x] = 9; // start

	while (looplife)
	{
		if (que.size() == 0)
			looplife = false;
		else
		{
            int tx = que.front().x;
			int ty = que.front().y;
            que.pop();

			if (map[ty*width + tx] == tarType)
			{
				// found target
				found_goal = true;
				looplife = false;
				goalpt = cPoint(tx, ty);
			}
			else
			{
				if (checkAvailable(map, dirmap, width, height, tx + 1, ty) == true)
				{
					if (checkAlreadyChecked(dirmap, width, height, tx + 2, ty) == true)
					{
                        if(firstFlag == true
                                &&  (start_heading == 180 || start_heading == -180)
                                && checkUncertainUTurn(map, width, tx+2, ty) == true)
                        {
                        }

                        if(map[ty*width + (tx+2)] != eMapNode_WALL
                                || firstFlag != true || (start_heading != 180 && start_heading != 180)) // uturn check
                        {
                            que.push(cPoint(tx + 2, ty));
                            dirmap[ty*width + (tx + 2)] = 3; // right
                        }
					}
				}

				if (checkAvailable(map, dirmap, width, height, tx - 1, ty) == true)
				{
                    if (checkAlreadyChecked(dirmap, width, height, tx-2, ty) == true)
					{
                        if(firstFlag == true
                                && start_heading == 0
                                && checkUncertainUTurn(map, width, tx-2, ty) == true)
                        {

                        }
                        else
                        {
                            que.push(cPoint(tx - 2, ty));
                            dirmap[ty*width + (tx - 2)] = 1; // left
                        }
					}
				}

				if (checkAvailable(map, dirmap, width, height, tx, ty + 1) == true)
				{
                    if (checkAlreadyChecked(dirmap, width, height, tx, ty+2) == true)
					{
                        if(firstFlag == true
                                && start_heading == -90
                                && checkUncertainUTurn(map, width, tx, ty+2) == true)
                        {

                        }
                        else
                        {
                            que.push(cPoint(tx, ty + 2));
                            dirmap[(ty + 2)*width + tx] = 2; // up
                        }
					}
				}

				if (checkAvailable(map, dirmap, width, height, tx, ty - 1) == true)
				{
                    if (checkAlreadyChecked(dirmap, width, height, tx, ty-2) == true)
					{
                        if(firstFlag == true
                                && start_heading == 90
                                && checkUncertainUTurn(map, width, tx, ty-2) == true)
                        {

                        }
                        else
                        {
                            que.push(cPoint(tx, ty - 2));
                            dirmap[(ty - 2)*width + tx] = 4; // down
                        }
					}
				}

                if(firstFlag == true)
                    firstFlag = false;
			}


		}
	}

	if (found_goal == true)
	{
        fprintf(stderr, "found goal (%d,%d)-(%d, %d)\n", start_x, start_y, goalpt.x, goalpt.y);

		path = makePath(dirmap, width, goalpt.x, goalpt.y);
	}
	else
	{
        fprintf(stderr, "no path\n");
	}

	delete[] dirmap;
	return path;
}
