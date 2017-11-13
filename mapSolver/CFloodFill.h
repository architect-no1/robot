#pragma once
#include "CAbstAlgo.h"
class CFloodFill : public CAbstAlgo
{
public:
	CFloodFill();
	~CFloodFill();

	std::vector<cPoint> solve(eMapNode * map, int width, int height
							, int start_x, int start_y, int start_heading, eMapNode tarType);

private:
	bool checkAlreadyChecked(int * dirmap, int w, int h, int x, int y);
	bool checkAvailable(eMapNode *map, int * dirmap, int w, int h, int x, int y);
	std::vector<cPoint> makePath(int * dirmap, int w, int gx, int gy);
    bool checkUncertainUTurn(eMapNode *map, int w, int x, int y);
};

