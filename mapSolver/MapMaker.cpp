#include "MapMaker.h"
#include <stdio.h>

#define MAP_SIZE 100

MapMaker::MapMaker()
{
   //ctor

   map = new eMapNode[MAP_SIZE*MAP_SIZE];
}

MapMaker::~MapMaker()
{
   //dtor
   delete[] map;
}

void MapMaker::init()
{
	map_size = MAP_SIZE;
   cur_x = cur_y = MAP_SIZE/2;
   cur_heading = 0;

   map_left = map_right = cur_x;
   map_bottom = map_top = cur_y;

   for(int i = 0; i < MAP_SIZE; i++)
   {
       for(int k = 0; k <MAP_SIZE; k++)
       {
           map[i*MAP_SIZE + k] = eMapNode_UNKNOWN;
       }
   }
}

void MapMaker::update_MapSize()
{
	if(map_left > cur_x-1) map_left = cur_x-1;
	if(map_right < cur_x+1) map_right = cur_x+1;
	if(map_top < cur_y+1) map_top = cur_y+1;
	if(map_bottom > cur_y-1) map_bottom = cur_y-1;

	fprintf(stderr, "map info -> (%d, %d, %d, %d)\r\n", map_left, map_right, map_top, map_bottom);
}

int MapMaker::Dir2Index(int heading, int x, int y, int w
                        , int * front, int * left, int * right, int * back)
{
    switch(heading)
    {
    case 0:
        *front = (y+0)*w+(x+1);
        *left = (y+1)*w+(x+0);
        *right = (y-1)*w+(x+0);
        *back = (y+0)*w+(x-1);
        break;
    case 90:
        *front = (y+1)*w+(x+0);
        *left = (y+0)*w+(x-1);
        *right = (y+0)*w+(x+1);
        *back = (y-1)*w+(x+0);
        break;
    case -90:
        *front = (y-1)*w+(x+0);
        *left = (y+0)*w+(x+1);
        *right = (y+0)*w+(x-1);
        *back = (y+1)*w+(x+0);
        break;
    default:
        *front = (y+0)*w+(x-1);
        *left = (y-1)*w+(x+0);
        *right = (y+1)*w+(x+0);
        *back = (y+0)*w+(x+1);
        break;
    }
}

void MapMaker::update_core(CEnvInfo envInfo, int front_index, int left_index, int right_index, int back_index)//, int x, int y)
{
    if(envInfo.center == 1) // start
        map[cur_y*MAP_SIZE + cur_x] = eMapNode_START;
    else if(envInfo.center == 2) // end
        map[cur_y*MAP_SIZE + cur_x] = eMapNode_GOAL;
    else if(envInfo.center == 3) // red dot
        map[cur_y*MAP_SIZE + cur_x] = eMapNode_REDDOT;
    else
        map[cur_y*MAP_SIZE + cur_x] = eMapNode_CLEAR;

	if (envInfo.front == 1)
        map[front_index] = eMapNode_WALL;
    else
        map[front_index] = eMapNode_CLEAR;

	if (envInfo.left == 1)
        map[left_index] = eMapNode_WALL;
    else
        map[left_index] = eMapNode_CLEAR;

	if (envInfo.right == 1)
        map[right_index] = eMapNode_WALL;
    else
        map[right_index] = eMapNode_CLEAR;


    if(map[back_index] == eMapNode_UNKNOWN)
        map[back_index] = eMapNode_BACK;
}

void MapMaker::upateCurCell(CEnvInfo envInfo)
{
   if(cur_x >= 0 && cur_x < MAP_SIZE && cur_y >= 0 && cur_y < MAP_SIZE)
   {
       int front_index, left_index, right_index, back_index;

       Dir2Index(cur_heading, cur_x, cur_y, MAP_SIZE
                 , &front_index, &left_index, &right_index, &back_index);

       update_core(envInfo, front_index, left_index, right_index, back_index);
   }
   else
   {
	   fprintf(stderr, "overflow : (%d, %d)\r\n", cur_x, cur_y);
       // throw overflow error !!!!
   }
}

// dir
// 0 : forward
// 1 : left
// 2 : right
// 3 : u-turn
void MapMaker::udatePos(eMovCmd dir)
{
    if (dir != eMovCmd_STOP)
    {

       switch(dir)
       {
       case eMovCmd_FORWARD: // forward
           break;
       case eMovCmd_LEFT: // left
           cur_heading += 90;
           break;
       case eMovCmd_RIGHT: // right
           cur_heading -= 90;
           break;
       case eMovCmd_UTURN: // u-turn
           cur_heading += 180;
           break;
       }

       while(cur_heading > 180) cur_heading -= 360;
       while(cur_heading < -180) cur_heading += 360;

       switch(cur_heading)
       {
       case 0:
           cur_x += 2;
           break;
       case 90:
           cur_y += 2;
           break;
       case -90:
           cur_y -= 2;
           break;
       default:
           cur_x -= 2;
           break;
       }

       if(cur_x < 0)
           cur_x = 0;
       if(cur_y < 0)
           cur_y = 0;
       if(cur_x >= MAP_SIZE)
           cur_x = MAP_SIZE - 1;
       if(cur_y >= MAP_SIZE)
           cur_y = MAP_SIZE - 1;
    }

	fprintf(stderr, "cur pos -> (%d, %d, %d)\r\n", cur_x, cur_y, cur_heading);
    update_MapSize();
}

std::string MapMaker::send_map()
{
    std::string outmsg;

    int map_width = map_right - map_left + 1;
    int map_height = map_top - map_bottom + 1;

    char *tmpStr = new char[map_height * map_width];

     for(int i = 0; i < map_height; i++)
         for(int k = 0; k < map_width; k++)
             tmpStr[i*map_width + k] = '.';

    // .X.X.X.
    // Xo-o-oX
    // .I.X.I.
    // Xo-oXoX
    // .X.X.X.

    for(int i = map_left; i <= map_right; i++)
    {
        for(int k = map_bottom; k <= map_top; k++)
        {
            int tmpStr_pos_y = (k-map_bottom);
            int tmpStr_pos_x = (i-map_left);

            int tmpStr_pos = tmpStr_pos_y * map_width + tmpStr_pos_x;
            eMapNode grid_info = map[k*MAP_SIZE+i];

            // draw grid info
            if(grid_info == eMapNode_UNKNOWN) // unknown
                tmpStr[tmpStr_pos] = '-';
            else if(grid_info == eMapNode_CLEAR) // opened
                tmpStr[tmpStr_pos] = 'o';
            else if(grid_info == eMapNode_WALL) // opened
                tmpStr[tmpStr_pos] = 'x';
            else if(grid_info == eMapNode_BACK) // opened
                tmpStr[tmpStr_pos] = '-';
            else if(grid_info == eMapNode_REDDOT) // opened
                tmpStr[tmpStr_pos] = 'j';
            else if(grid_info == eMapNode_START) // opened
                tmpStr[tmpStr_pos] = 'b';
            else if(grid_info == eMapNode_GOAL) // opened
                tmpStr[tmpStr_pos] = 'f';
            else
                tmpStr[tmpStr_pos] = '?';

        }

    }

    // draw screen

    std::string robo_str;
    int robo_x = cur_x - map_left;
    int robo_y = cur_y - map_bottom;

    if(cur_heading == 0) robo_str = "rd";
    else if(cur_heading == 90) robo_str = "rw";
    else if(cur_heading == -90) robo_str = "rs";
    else  robo_str = "ra";

    outmsg += format("map %d %d ", (map_width-1)/2, (map_height-1)/2);
    for(int i = map_height -1; i >= 0; i--)
    {
        for(int k = 0; k < map_width; k++)
        {
            if(k == robo_x && i == robo_y)
            {
                outmsg += robo_str + format("%c ", tmpStr[i*map_width + k]);
            }
            else
                outmsg += format("%c ", tmpStr[i*map_width + k]);
        }
    }

    fprintf(stderr, ">> send map : %s\r\n", outmsg.c_str());

    return outmsg;
}

void MapMaker::printf_curMap()
{
   int map_width = map_right - map_left + 1;
   int map_height = map_top - map_bottom + 1;

   char *tmpStr = new char[map_height * map_width];

	for(int i = 0; i < map_height; i++)
		for(int k = 0; k < map_width; k++)
			tmpStr[i*map_width + k] = '.';

   // .X.X.X.
   // Xo-o-oX
   // .I.X.I.
   // Xo-oXoX
   // .X.X.X.

   for(int i = map_left; i <= map_right; i++)
   {
       for(int k = map_bottom; k <= map_top; k++)
       {
	       int tmpStr_pos_y = (k-map_bottom);
	       int tmpStr_pos_x = (i-map_left);

           int tmpStr_pos = tmpStr_pos_y * map_width + tmpStr_pos_x;
           eMapNode grid_info = map[k*MAP_SIZE+i];

           // draw grid info
           if(grid_info == eMapNode_UNKNOWN) // unknown
               tmpStr[tmpStr_pos] = '.';
           else if(grid_info == eMapNode_CLEAR) // opened
               tmpStr[tmpStr_pos] = 'o';
           else if(grid_info == eMapNode_WALL) // opened
			   tmpStr[tmpStr_pos] = 'X';
		   else
			   tmpStr[tmpStr_pos] = '?';

       }

   }

   // draw screen

   int robo_x = cur_x - map_left;
   int robo_y = cur_y - map_bottom;

   fprintf(stderr, "map %d %d \r\n", map_width, map_height);
   for(int i = map_height -1; i >= 0; i--)
   {
       for(int k = 0; k < map_width; k++)
       {
           if(k == robo_x && i == robo_y)
               fprintf(stderr,"R");
           else
                fprintf(stderr,"%c", tmpStr[i*map_width + k]);
       }
       fprintf(stderr,"\r\n");
   }
   fprintf(stderr, "\r\n");
}
