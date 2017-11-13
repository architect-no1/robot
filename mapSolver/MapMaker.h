#pragma once

#include <string>
#include "mapType.h"

class MapMaker
{
   public:
       MapMaker();
       virtual ~MapMaker();

       void init();
       void updateCurCell(CEnvInfo envInfo);// , int x, int y, int heading);
       void updateCurCell_Sign(CEnvInfo envInfo);// , int x, int y, int heading);
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
       void update_sign(CEnvInfo envInfo
                      , eMapNode front_index, eMapNode left_index
                      , eMapNode right_index, eMapNode back_index);
       void update_MapSize();
       int Dir2Index(int heading, int x, int y, int w
                   , int * front, int * left, int * right, int * back);
       void Dir2SignIndex(int heading
                   , eMapNode * front, eMapNode * left, eMapNode * right, eMapNode * back);

       bool CheckSignNode(eMapNode grid_info, std::string * str);
};
