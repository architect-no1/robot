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

    fprintf(stderr, "map info -> (%d, %d, %d, %d)\n", map_left, map_right, map_top, map_bottom);
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
    {
        if(map[cur_y*MAP_SIZE + cur_x] == eMapNode_UNKNOWN
              || map[cur_y*MAP_SIZE + cur_x] == eMapNode_BACK
              || map[cur_y*MAP_SIZE + cur_x] == eMapNode_CLEAR)
        {
            map[cur_y*MAP_SIZE + cur_x] = eMapNode_REDDOT;
        }
    }
    else
    {
        if(map[cur_y*MAP_SIZE + cur_x] == eMapNode_UNKNOWN
                || map[cur_y*MAP_SIZE + cur_x] == eMapNode_BACK
                || map[cur_y*MAP_SIZE + cur_x] == eMapNode_CLEAR)
        {
            map[cur_y*MAP_SIZE + cur_x] = eMapNode_CLEAR;
        }
    }

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

void MapMaker::Dir2SignIndex(int heading
            , eMapNode * front, eMapNode * left
            , eMapNode * right, eMapNode * back)
{
    if(heading == 0)
    {
        *front = eMapNode_D0;
        *left = eMapNode_W0;
        *right = eMapNode_S0;
        *back = eMapNode_A0;
    }
    else if(heading == 90)
    {
        *front = eMapNode_W0;
        *left = eMapNode_A0;
        *right = eMapNode_D0;
        *back = eMapNode_S0;
    }
    else if(heading == -90)
    {
        *front = eMapNode_S0;
        *left = eMapNode_D0;
        *right = eMapNode_A0;
        *back = eMapNode_W0;
    }
    else// if(heading == 180)
    {
        *front = eMapNode_A0;
        *left = eMapNode_S0;
        *right = eMapNode_W0;
        *back = eMapNode_D0;
    }
}


void MapMaker::update_sign(CEnvInfo envInfo
                           , eMapNode front_index, eMapNode left_index
                           , eMapNode right_index, eMapNode back_index)//, int x, int y)
{
    int index = cur_y*MAP_SIZE + cur_x;
    // current cell is REDDOT
    if(map[index] != eMapNode_REDDOT)
        fprintf(stderr, "!!!!!!!! RED DOT ERROR\n");

    fprintf(stderr, "%s, %s, %s\n", envInfo.leftSign.c_str(), envInfo.frontSign.c_str(), envInfo.leftSign.c_str());

    if((int)envInfo.frontSign[0] >= (int)'0')
    {
        int inc = (int)envInfo.frontSign[0] - (int)'0';

        map[index] = (eMapNode)(front_index + inc);

        fprintf(stderr, "front ---- %d -- %d\n" , inc, (int)map[index]);
    }
    else if((int)envInfo.leftSign[0] >= (int)'0')
    {
        int inc = (int)envInfo.leftSign[0] - (int)'0';
        map[index] = (eMapNode)(left_index + inc);

        fprintf(stderr, "left ---- %d -- %d\n" , inc, (int)map[index]);
    }
    else if((int)envInfo.rightSign[0] >= (int)'0')
    {
        int inc = (int)envInfo.rightSign[0] - (int)'0';
        map[index] = (eMapNode)(right_index + inc);

        fprintf(stderr, "right ---- %d -- %d\n" , inc, (int)map[index]);
    }
}

void MapMaker::updateCurCell_Sign(CEnvInfo envInfo)
{
    if(cur_x >= 0 && cur_x < MAP_SIZE && cur_y >= 0 && cur_y < MAP_SIZE)
    {
        eMapNode front_index, left_index, right_index, back_index;

        Dir2SignIndex(cur_heading
                  , &front_index, &left_index, &right_index, &back_index);

        update_sign(envInfo, front_index, left_index, right_index, back_index);
    }
    else
    {
        fprintf(stderr, "overflow : (%d, %d)\n", cur_x, cur_y);
        // throw overflow error !!!!
    }

}

void MapMaker::updateCurCell(CEnvInfo envInfo)
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
       fprintf(stderr, "overflow : (%d, %d)\n", cur_x, cur_y);
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

    fprintf(stderr, "cur pos -> (%d, %d, %d)\n", cur_x, cur_y, cur_heading);
    update_MapSize();
}

bool MapMaker::CheckSignNode(eMapNode grid_info, std::string * str)
{
    bool rval = false;

    str->clear();

    if(grid_info == eMapNode_REDDOT) *str = "j ";

    else if(grid_info == eMapNode_A0) *str = "ja0 ";
    else if(grid_info == eMapNode_A1) *str = "ja1 ";
    else if(grid_info == eMapNode_A2) *str = "ja2 ";
    else if(grid_info == eMapNode_A3) *str = "ja3 ";
    else if(grid_info == eMapNode_A4) *str = "ja4 ";
    else if(grid_info == eMapNode_A5) *str = "ja5 ";
    else if(grid_info == eMapNode_A6) *str = "ja6 ";
    else if(grid_info == eMapNode_A7) *str = "ja7 ";
    else if(grid_info == eMapNode_A8) *str = "ja8 ";
    else if(grid_info == eMapNode_A9) *str = "ja9 ";
    else if(grid_info == eMapNode_A10) *str = "ja10 ";

    else if(grid_info == eMapNode_S0) *str = "js0 ";
    else if(grid_info == eMapNode_S1) *str = "js1 ";
    else if(grid_info == eMapNode_S2) *str = "js2 ";
    else if(grid_info == eMapNode_S3) *str = "js3 ";
    else if(grid_info == eMapNode_S4) *str = "js4 ";
    else if(grid_info == eMapNode_S5) *str = "js5 ";
    else if(grid_info == eMapNode_S6) *str = "js6 ";
    else if(grid_info == eMapNode_S7) *str = "js7 ";
    else if(grid_info == eMapNode_S8) *str = "js8 ";
    else if(grid_info == eMapNode_S9) *str = "js9 ";
    else if(grid_info == eMapNode_S10) *str = "js10 ";

    else if(grid_info == eMapNode_D0) *str = "jd0 ";
    else if(grid_info == eMapNode_D1) *str = "jd1 ";
    else if(grid_info == eMapNode_D2) *str = "jd2 ";
    else if(grid_info == eMapNode_D3) *str = "jd3 ";
    else if(grid_info == eMapNode_D4) *str = "jd4 ";
    else if(grid_info == eMapNode_D5) *str = "jd5 ";
    else if(grid_info == eMapNode_D6) *str = "jd6 ";
    else if(grid_info == eMapNode_D7) *str = "jd7 ";
    else if(grid_info == eMapNode_D8) *str = "jd8 ";
    else if(grid_info == eMapNode_D9) *str = "jd9 ";
    else if(grid_info == eMapNode_D10) *str = "jd10 ";

    else if(grid_info == eMapNode_W0) *str = "jw0 ";
    else if(grid_info == eMapNode_W1) *str = "jw1 ";
    else if(grid_info == eMapNode_W2) *str = "jw2 ";
    else if(grid_info == eMapNode_W3) *str = "jw3 ";
    else if(grid_info == eMapNode_W4) *str = "jw4 ";
    else if(grid_info == eMapNode_W5) *str = "jw5 ";
    else if(grid_info == eMapNode_W6) *str = "jw6 ";
    else if(grid_info == eMapNode_W7) *str = "jw7 ";
    else if(grid_info == eMapNode_W8) *str = "jw8 ";
    else if(grid_info == eMapNode_W9) *str = "jw9 ";
    else if(grid_info == eMapNode_W10) *str = "jw10 ";

    if(str->size() > 0)
        rval = true;

    return rval;
}

std::string MapMaker::send_map()
{
    std::string outmsg;

    int sign_x = -1;
    int sign_y = -1;
    std::string sign_str;

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
            if(sign_str.size() == 0 && CheckSignNode(grid_info, &sign_str) == true)
            {
                sign_x = i - map_left;
                sign_y = k - map_bottom;

                tmpStr[tmpStr_pos] = 'j';

                fprintf(stderr, "sign str : %s\n" , sign_str.c_str());
            }
            else if(grid_info == eMapNode_UNKNOWN) // unknown
                tmpStr[tmpStr_pos] = '-';
            else if(grid_info == eMapNode_CLEAR) // opened
                tmpStr[tmpStr_pos] = 'o';
            else if(grid_info == eMapNode_WALL) // opened
                tmpStr[tmpStr_pos] = 'x';
            else if(grid_info == eMapNode_BACK) // opened
                tmpStr[tmpStr_pos] = '-';
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
                if(k == sign_x && i == sign_y)
                {
                    fprintf(stderr, "!!!! robot + sign : %s\n", sign_str.c_str());
                    outmsg += robo_str + sign_str;
                }
                else
                {
                    fprintf(stderr, "!!!! robot + normal : %s, (%d, %d), (%d, %d)\n", sign_str.c_str(), robo_x, robo_y, sign_x, sign_y);
                    outmsg += robo_str + format("%c ", tmpStr[i*map_width + k]);
                }
            }
            else if(k == sign_x && i == sign_y)
            {
                outmsg += sign_str;
            }
            else
                outmsg += format("%c ", tmpStr[i*map_width + k]);
        }
    }

    fprintf(stderr, ">> send map : %s\n", outmsg.c_str());

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

   fprintf(stderr, "map %d %d \n", map_width, map_height);
   for(int i = map_height -1; i >= 0; i--)
   {
       for(int k = 0; k < map_width; k++)
       {
           if(k == robo_x && i == robo_y)
               fprintf(stderr,"R");
           else
                fprintf(stderr,"%c", tmpStr[i*map_width + k]);
       }
       fprintf(stderr,"\n");
   }
   fprintf(stderr, "\n");
}
