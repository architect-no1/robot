#pragma once

#include <string>
#include "mapType.h"

class MapMaker
{
   public:
       MapMaker();
       virtual ~MapMaker();

       void init();
	   void upateCurCell(CEnvInfo envInfo);// , int x, int y, int heading);
       void udatePos(eMovCmd dir);

       void printf_curMap();
        std::string send_map();

       int cur_x;
       int cur_y;
       int cur_heading;

	   eMapNode * map;
	   int map_size;
		   
       int map_bottom;
       int map_top;
       int map_left;
       int map_right;
   protected:

   private:
       void update_core(CEnvInfo envInfo, int front_index, int left_index, int right_index, int back_index);//, int x, int y);
       void update_MapSize();
       int Dir2Index(int heading, int x, int y, int w
                   , int * front, int * left, int * right, int * back);
};
