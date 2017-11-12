#pragma once

#include <vector>
#include "mapType.h"

class CAbstAlgo
{
public:
	CAbstAlgo();
	~CAbstAlgo();

	virtual std::vector<cPoint> solve(eMapNode * map, int width, int height
		, int start_x, int start_y, int start_heading, eMapNode tarType) = 0;
};

