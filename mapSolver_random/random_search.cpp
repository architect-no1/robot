#include "random_search.h"
#include "Astar.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

CRanSearch::CRanSearch()
{
    srand (time(NULL));
}

std::vector<cPoint> CRanSearch::solve(eMapNode * map, int width, int height
                        , int start_x, int start_y, int start_heading, eMapNode tarType)
{
    std::vector<cPoint> rval;
    cPoint nextPt;

    int forward = 0;
    int left = 0;
    int right = 0;
    int uturn = 0;
    int forward_index, left_index, right_index;

    if(start_heading == 0)
    {
        forward_index = (start_y+0)*width + (start_x+1);
        left_index = (start_y+1)*width + (start_x+0);
        right_index = (start_y-1)*width + (start_x+0);
    }
    else if(start_heading == 90)
    {
        forward_index = (start_y+1)*width + (start_x+0);
        left_index = (start_y+0)*width + (start_x-1);
        right_index = (start_y+0)*width + (start_x+1);
    }
    else if(start_heading == -90)
    {
        forward_index = (start_y-1)*width + (start_x+0);
        left_index = (start_y+0)*width + (start_x+1);
        right_index = (start_y+0)*width + (start_x-1);
    }
    else
    {
        forward_index = (start_y+0)*width + (start_x-1);
        left_index = (start_y-1)*width + (start_x+0);
        right_index = (start_y+1)*width + (start_x+0);
    }

    if(map[forward_index] == eMapNode_WALL)
        forward = 1;
    if(map[left_index] == eMapNode_WALL)
        left = 1;
    if(map[right_index] == eMapNode_WALL)
        right = 1;

    if(forward == 0 || left == 0 || right == 0)
        uturn = 1;

    fprintf(stderr, "--- %d, %d, %d\n" , left, forward, right);

    int final_dir = -1;

    int iSecret = rand() % 50 + 1;
    int state = 0;
    fprintf(stderr, "random : %d\n", iSecret);

    while(iSecret > 0)
    {
        if(left == 0)
        {
            iSecret--;
            if(iSecret == 0) final_dir = 0;
        }
        if(forward == 0)
        {
            iSecret--;
            if(iSecret == 0) final_dir = 1;
        }
        if(right == 0)
        {
            iSecret--;
            if(iSecret == 0) final_dir = 2;
        }
        if(uturn == 0)
        {
            iSecret--;
            if(iSecret == 0) final_dir = 3;
        }
    }

    if(final_dir == 0)
    {
        if(start_heading == 0)
            nextPt = cPoint(start_x, start_y+2);
        else if(start_heading == 90)
            nextPt = cPoint(start_x-2, start_y);
        else if(start_heading == -90)
            nextPt = cPoint(start_x+2, start_y);
        else
            nextPt = cPoint(start_x, start_y-2);
    }
    else if(final_dir == 1)
    {
        if(start_heading == 0)
            nextPt = cPoint(start_x+2, start_y);
        else if(start_heading == 90)
            nextPt = cPoint(start_x, start_y+2);
        else if(start_heading == -90)
            nextPt = cPoint(start_x, start_y-2);
        else
            nextPt = cPoint(start_x-2, start_y);
    }
    else if(final_dir == 2)
    {
        if(start_heading == 0)
            nextPt = cPoint(start_x, start_y-2);
        else if(start_heading == 90)
            nextPt = cPoint(start_x+2, start_y);
        else if(start_heading == -90)
            nextPt = cPoint(start_x-2, start_y);
        else
            nextPt = cPoint(start_x, start_y+2);
    }
    else
    {
        if(start_heading == 0)
            nextPt = cPoint(start_x-2, start_y);
        else if(start_heading == 90)
            nextPt = cPoint(start_x, start_y-2);
        else if(start_heading == -90)
            nextPt = cPoint(start_x, start_y+2);
        else
            nextPt = cPoint(start_x+2, start_y);
    }

    fprintf(stderr, "(%d, %d) - (%d, %d)", start_x, start_y, nextPt.x, nextPt.y);

    CAstar astar;

    std::vector<cPoint> found_path;
    found_path = astar.solve(map, width, height
                             , start_x, start_y, start_heading, eMapNode_UNKNOWN);

    fprintf(stderr, "astar result - %d", (int)found_path.size());

    if(found_path.size() > 0)
    {
        rval.push_back(cPoint(start_x, start_y));
        rval.push_back(nextPt);
    }

    return rval;
}
