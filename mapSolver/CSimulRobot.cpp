#include "CSimulRobot.h"
#include "mapType.h"
#include <stdio.h>

int start_robo_x = 1;
int start_robo_y = 1;
int start_robo_heading = 0;
int test_map_size = 11;
int test_map[] = // 11 X 11 121-58 = 
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0,	1, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

void CSimulRobot::DrawMap()
{
    for(int i = 0; i < 11; i++)
    {
        for(int k = 0; k < 11; k++)
        {
            int fPath = false;

            for(int n = 0; n < robotPath.size(); n++)
            {
                if(robotPath[n].x == i && robotPath[n].y == k)
                    fPath = true;
            }


            if(cur_x == i && cur_y == k)
                fprintf(stderr, "R ");
            else if(fPath == true)
                fprintf(stderr, "* ");
            else
                fprintf(stderr, "%d ", test_map[k*test_map_size + i]);
        }
        fprintf(stderr, "\r\n");
    }
    fprintf(stderr, "\r\n");
}

CSimulRobot::CSimulRobot()
{
	cur_x = start_robo_x;
	cur_y = start_robo_y;
	cur_heading = start_robo_heading;

    robotPath.push_back(cPoint(cur_x, cur_y));
}


CSimulRobot::~CSimulRobot()
{
}

void CSimulRobot::forward(int heading, int * next_x, int *next_y, int *next_heading)
{
	if (heading == 0)
		*next_x += 2;
	else if (heading == 90)
		*next_y += 2;
	else if (heading == -90)
		*next_y -= 2;
	else
		*next_x -= 2;
	
}
void CSimulRobot::left(int heading, int * next_x, int *next_y, int *next_heading)
{
	if (heading == 0)
		*next_y += 2;
	else if (heading == 90)
		*next_x -= 2;
	else if (heading == -90)
		*next_x += 2;
	else
		*next_y -= 2;

	*next_heading += 90;

	while (*next_heading > 180)
		*next_heading -= 360;

}
void CSimulRobot::right(int heading, int * next_x, int *next_y, int *next_heading)
{
	if (heading == 0)
		*next_y -= 2;
	else if (heading == 90)
		*next_x += 2;
	else if (heading == -90)
		*next_x -= 2;
	else
		*next_y += 2;

	*next_heading -= 90;

	while (*next_heading < -180)
		*next_heading += 360;
}
void CSimulRobot::uturn(int heading, int * next_x, int *next_y, int *next_heading)
{
	if (heading == 0)
		*next_x -= 2;
	else if (heading == 90)
		*next_y -= 2;
	else if (heading == -90)
		*next_y += 2;
	else
		*next_x += 2;

	*next_heading += 180;

	while (*next_heading > 180)
		*next_heading -= 360;
}

int MapNode2EnvInfo(int mapinfo)
{
	int rval = 0;

	switch (mapinfo)
	{
	case 1: // wall
		rval = 1; // wall
		break;
	default:
		rval = 0;
		break;

	}

	return rval;
}

void CSimulRobot::makeEnvInfo_dir(CEnvInfo * pEnvinfo, int heading, int up, int down, int left, int right)
{
	pEnvinfo->clear();

	switch (heading)
	{
	case 0:
		pEnvinfo->front = MapNode2EnvInfo(right);
		pEnvinfo->left = MapNode2EnvInfo(up);
		pEnvinfo->right = MapNode2EnvInfo(down);
		break;
	case 90:
		pEnvinfo->front = MapNode2EnvInfo(up);
		pEnvinfo->left = MapNode2EnvInfo(left);
		pEnvinfo->right = MapNode2EnvInfo(right);
		break;
	case -90:
		pEnvinfo->front = MapNode2EnvInfo(down);
		pEnvinfo->left = MapNode2EnvInfo(right);
		pEnvinfo->right = MapNode2EnvInfo(left);
		break;
	default:
		pEnvinfo->front = MapNode2EnvInfo(left);
		pEnvinfo->left = MapNode2EnvInfo(down);
		pEnvinfo->right = MapNode2EnvInfo(up);
		break;
	}
}

void CSimulRobot::makeEnvInfo(CEnvInfo * pEnvInfo)
{
	pEnvInfo->clear();

	if (cur_x > 0 && cur_x < test_map_size && cur_y > 0 && cur_y < test_map_size)
	{
		int up = test_map[(cur_y+1)*test_map_size + cur_x];
		int down = test_map[(cur_y-1)*test_map_size + cur_x];
		int left = test_map[cur_y*test_map_size + (cur_x-1)];
		int right = test_map[cur_y*test_map_size + (cur_x+1)];

		makeEnvInfo_dir(pEnvInfo, cur_heading, up, down, left, right);
	}
}

void CSimulRobot::move(eMovCmd mov)
{
	int next_x = cur_x;
	int next_y = cur_y;
	int next_heading = cur_heading;

	switch (mov)
	{
    case eMovCmd_FORWARD:
		forward(cur_heading, &next_x, &next_y, &next_heading);
		break;

    case eMovCmd_LEFT:
		left(cur_heading, &next_x, &next_y, &next_heading);
		break;

    case eMovCmd_RIGHT:
		right(cur_heading, &next_x, &next_y, &next_heading);
		break;

    case eMovCmd_UTURN:
		uturn(cur_heading, &next_x, &next_y, &next_heading);
		break;

	default:
		break;
	}
	
	cur_x = next_x;
	cur_y = next_y;
	cur_heading = next_heading;

    robotPath.push_back(cPoint(cur_x, cur_y));
}
