#include "Astar.h"
#include <stdio.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <algorithm>

CAstar::CAstar()
{

}

bool CAstar::checkAvailable(eMapNode *map, int w, int h, int x, int y)
{
    bool rval = false;

    if (x >= 0 && x < w && y >= 0 && y < h
        && map[y*w + x] != eMapNode_WALL)
    {
        rval = true;
    }

    return rval;
}

bool CAstar::checkAlreadyChecked(int * scost_map, int w, int h, int x, int y)
{
    bool rval = false;

    if (x >= 0 && x < w && y >= 0 && y < h)
    {
        if(scost_map[y*w + x] == -1)
            rval = true;
    }
    else
        rval = true;

    return rval;
}

int CAstar::checkMinDir(eMapNode *map, int * s_cost, int w, int x, int y)
{
    int rval;

    int dir1;
    int dir2;
    int dir;

    int value_ori = s_cost[y*w + x];

    int value[4];

    value[0] = s_cost[(y+0)*w + x-2];
    value[1] = s_cost[(y+2)*w + x+0];
    value[2] = s_cost[(y+0)*w + x+2];
    value[3] = s_cost[(y-2)*w + x+0];

    if(value[0] == -1 || map[(y+0)*w+(x-1)] == eMapNode_WALL)
        value[0] = 9999;

    if(value[1] == -1 || map[(y+1)*w+(x+0)] == eMapNode_WALL)
        value[1] = 9999;

    if(value[2] == -1 || map[(y+0)*w+(x+1)] == eMapNode_WALL)
        value[2] = 9999;

    if(value[3] == -1 || map[(y-1)*w+(x+0)] == eMapNode_WALL)
        value[3] = 9999;


    if(value[0] < value[1])
        dir1 = 0;
    else
        dir1 = 1;

    if(value[2] < value[3])
        dir2 = 2;
    else
        dir2 = 3;

    if(value[dir1] < value[dir2])
        dir = dir1;
    else
        dir = dir2;

    if(value[dir] > value_ori)
        rval = 9;
    else
        rval = dir+1;

    return rval;
}

std::vector<cPoint> CAstar::makePath(eMapNode *map, int * s_cost, int w, int gx, int gy)
{
    std::vector<cPoint> path;
    std::vector<cPoint> path_inv;
    int cx = gx;
    int cy = gy;
    bool looplife = true;

    while (looplife)
    {
        path_inv.push_back(cPoint(cx, cy));

        switch (checkMinDir(map, s_cost, w, cx, cy))
        {
        case 1: // left
            cx -= 2;
            break;
        case 2: // up
            cy += 2;
            break;
        case 3: // right
            cx += 2;
            break;
        case 4: // down
            cy -= 2;
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

struct stAstarEle
{
    cPoint pt;
    int t_cost;
    stAstarEle(cPoint point, int cost)
    {
        pt = point;
        t_cost = cost;
    }
};

bool wayToSort(stAstarEle i, stAstarEle j)
{
    return (i.t_cost > j.t_cost);
}

bool CAstar::checkUncertainUTurn(eMapNode *map, int w, int x, int y)
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

std::vector<cPoint> CAstar::solve(eMapNode * map, int width, int height
                        , int start_x, int start_y, int start_heading
                        , eMapNode tarType)
{
    float firstFlag = true;
    std::vector<cPoint> path;
    bool looplife = true;
    cPoint goalpt;
    bool found_goal = false;

    int * s_cost = new int[width*height];
    memset(s_cost, -1, sizeof(int) * width*height);

    int * headTable = new int[width*height];
    memset(headTable, 0, sizeof(int) * width*height);


    std::vector<stAstarEle> ptList;

    s_cost[start_y*width + start_x] = 0;
    headTable[start_y*width + start_x] = start_heading;
    ptList.push_back(stAstarEle(cPoint(start_x, start_y), 0));

    while (looplife)
    {
        if (ptList.size() == 0)
            looplife = false;
        else
        {
            // sorting high -> low
            std::sort(ptList.begin(), ptList.end(), wayToSort);

            int tx = ptList.back().pt.x;
            int ty = ptList.back().pt.y;
            ptList.pop_back();

            if (map[ty*width + tx] == tarType)
            {
                // found target
                found_goal = true;
                looplife = false;
                goalpt = cPoint(tx, ty);
            }
            else
            {
                int parent_sCost = s_cost[ty*width+tx];

                if (checkAvailable(map, width, height, tx + 1, ty) == true)
                {
                    if (checkAlreadyChecked(s_cost, width, height, tx + 2, ty) == true)
                    {
                        if(firstFlag == true
                                &&  (start_heading == 180 || start_heading == -180)
                                && checkUncertainUTurn(map, width, tx+2, ty) == true)
                        {

                        }
                        else // uturn check
                        {
                            s_cost[ty*width + (tx+2)] = parent_sCost+2;
                            ptList.push_back(stAstarEle(cPoint(tx+2, ty), parent_sCost+2));
                        }
                    }
                }

                if (checkAvailable(map, width, height, tx - 1, ty) == true)
                {
                    if (checkAlreadyChecked(s_cost, width, height, tx - 2, ty) == true)
                    {
                        if(firstFlag == true
                                && start_heading == 0
                                && checkUncertainUTurn(map, width, tx-2, ty) == true)
                        {

                        }
                        else
                        {
                            s_cost[ty*width + (tx-2)] = parent_sCost+2;
                            ptList.push_back(stAstarEle(cPoint(tx-2, ty), parent_sCost+2));
                        }
                    }
                }

                if (checkAvailable(map, width, height, tx, ty + 1) == true)
                {
                    if (checkAlreadyChecked(s_cost, width, height, tx, ty + 2) == true)
                    {
                        if(firstFlag == true
                                && start_heading == -90
                                && checkUncertainUTurn(map, width, tx, ty + 2) == true)
                        {

                        }
                        else
                        {
                            s_cost[(ty+2)*width + tx] = parent_sCost+2;
                            ptList.push_back(stAstarEle(cPoint(tx, ty+2), parent_sCost+2));
                        }
                    }
                }

                if (checkAvailable(map, width, height, tx, ty - 1) == true)
                {
                    if (checkAlreadyChecked(s_cost, width, height, tx, ty - 2) == true)
                    {
                        if(firstFlag == true
                                && start_heading == 90
                                && checkUncertainUTurn(map, width, tx, ty - 2) == true)
                        {

                        }
                        else
                        {
                            s_cost[(ty-2)*width + tx] = parent_sCost+2;
                            ptList.push_back(stAstarEle(cPoint(tx, ty-2), parent_sCost+2));
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

        path = makePath(map, s_cost, width, goalpt.x, goalpt.y);
    }
    else
    {
        fprintf(stderr, "no path\n");
    }

    delete[] headTable;
    delete[] s_cost;
    return path;
}
