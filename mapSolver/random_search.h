#ifndef CLEFTHAND_H
#define CLEFTHAND_H

#include "CAbstAlgo.h"

class CRanSearch : public CAbstAlgo
{
public:
    CRanSearch();

    std::vector<cPoint> solve(eMapNode * map, int width, int height
                              , int start_x, int start_y, int start_heading, eMapNode tarType);

};

#endif // CLEFTHAND_H
