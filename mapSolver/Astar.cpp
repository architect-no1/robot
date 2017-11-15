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

bool CAstar::checkAlreadyChecked(float * scost_map, int w, int h, int x, int y)
{
    bool rval = false;

    if (x >= 0 && x < w && y >= 0 && y < h)
    {
        if(scost_map[y*w + x] < 0)
            rval = true;
    }
    else
        rval = true;

    return rval;
}

int CAstar::checkMinDir(eMapNode *map, float * s_cost, int w, int x, int y)
{
    int rval;

    int dir1;
    int dir2;
    int dir;

    float value_ori = s_cost[y*w + x];

    float value[4];

    value[0] = s_cost[(y+0)*w + x-2];
    value[1] = s_cost[(y+2)*w + x+0];
    value[2] = s_cost[(y+0)*w + x+2];
    value[3] = s_cost[(y-2)*w + x+0];

    if(value[0] < 0 || map[(y+0)*w+(x-1)] == eMapNode_WALL)
        value[0] = 9999;

    if(value[1] < 0 || map[(y+1)*w+(x+0)] == eMapNode_WALL)
        value[1] = 9999;

    if(value[2] < 0 || map[(y+0)*w+(x+1)] == eMapNode_WALL)
        value[2] = 9999;

    if(value[3] < 0 || map[(y-1)*w+(x+0)] == eMapNode_WALL)
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

std::vector<cPoint> CAstar::makePath(eMapNode *map, float * s_cost, int w, int gx, int gy)
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
    float t_cost;
    stAstarEle(cPoint point, float cost)
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

float CAstar::calcCost_left(int bef_heading)
{
    float cost = 0;

    if(bef_heading == 0)
        cost = 3.62f;
    else if(bef_heading == 90)
        cost = 3.09;
    else if(bef_heading == -90)
        cost = 3.09f;
    else
        cost = 2.6f;

    return cost;
}

float CAstar::calcCost_right(int bef_heading)
{
    float cost = 0;

    if(bef_heading == 0)
        cost = 2.6f;
    else if(bef_heading == 90)
        cost = 3.09;
    else if(bef_heading == -90)
        cost = 3.09f;
    else
        cost = 3.62f;

    return cost;
}

float CAstar::calcCost_up(int bef_heading)
{
    float cost = 0;

    if(bef_heading == 0)
        cost = 3.09f;
    else if(bef_heading == 90)
        cost = 2.6f;
    else if(bef_heading == -90)
        cost = 3.62f;
    else
        cost = 3.09;

    return cost;
}

float CAstar::calcCost_down(int bef_heading)
{
    float cost = 0;

    if(bef_heading == 0)
        cost = 3.09f;
    else if(bef_heading == 90)
        cost = 3.62f;
    else if(bef_heading == -90)
        cost = 2.6f;
    else
        cost = 3.09f;

    return cost;
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

    float * s_cost = new float[width*height];
    for(int i = 0; i < width*height; i++)
        s_cost[i] = -100;

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
                int parent_heading = headTable[ty*width+tx];
                float parent_sCost = s_cost[ty*width+tx];

                bool ThereIsWay = false;

                // right
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
                            ThereIsWay = true;
                            headTable[ty*width + (tx+2)] = 0;
                            float new_cost = parent_sCost + calcCost_right(parent_heading);
                            s_cost[ty*width + (tx+2)] = new_cost;
                            ptList.push_back(stAstarEle(cPoint(tx+2, ty), new_cost));
                        }
                    }
                }

                // left
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
                            ThereIsWay = true;
                            headTable[ty*width + (tx-2)] = 180;
                            float new_cost = parent_sCost + calcCost_left(parent_heading);
                            s_cost[ty*width + (tx-2)] = new_cost;
                            ptList.push_back(stAstarEle(cPoint(tx-2, ty), new_cost));
                        }
                    }
                }

                // up
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
                            ThereIsWay = true;
                            headTable[(ty+2)*width + tx] = 90;
                            float new_cost = parent_sCost + calcCost_up(parent_heading);
                            s_cost[(ty+2)*width + tx] = new_cost;
                            ptList.push_back(stAstarEle(cPoint(tx, ty+2), new_cost));
                        }
                    }
                }

                // down
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
                            ThereIsWay = true;
                            headTable[(ty-2)*width + tx] = -90;
                            float new_cost = parent_sCost + calcCost_down(parent_heading);
                            s_cost[(ty-2)*width + tx] = new_cost;
                            ptList.push_back(stAstarEle(cPoint(tx, ty-2), new_cost));
                        }
                    }
                }

                if(firstFlag == true)
                {
                    firstFlag = false;

                    if(ThereIsWay == false)
                    {
                        int uturn_x, uturn_y;

                        if(start_heading == 0)
                        {
                            uturn_x = start_x - 2;
                            uturn_y = start_y;
                        }
                        else if(start_heading == 90)
                        {
                            uturn_x = start_x;
                            uturn_y = start_y-2;
                        }
                        else if(start_heading == -90)
                        {
                            uturn_x = start_x;
                            uturn_y = start_y+2;
                        }
                        else
                        {
                            uturn_x = start_x + 2;
                            uturn_y = start_y;
                        }

                        headTable[uturn_y*width + uturn_x] = -90;
                        float new_cost = parent_sCost + 3.62f;
                        s_cost[uturn_y*width + uturn_x] = new_cost;
                        ptList.push_back(stAstarEle(cPoint(uturn_x, uturn_y), new_cost));
                    }
                }
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
