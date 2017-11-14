#ifndef CASTAR_H
#define CASTAR_H

#include "CAbstAlgo.h"

class CAstar : public CAbstAlgo
{
public:
    CAstar();

    std::vector<cPoint> solve(eMapNode * map, int width, int height
                            , int start_x, int start_y, int start_heading, eMapNode tarType);

private:
    bool checkAlreadyChecked(int * dirmap, int w, int h, int x, int y);
    bool checkAvailable(eMapNode *map, int w, int h, int x, int y);
    std::vector<cPoint> makePath(eMapNode *map, int * s_cost, int w, int gx, int gy);
    int checkMinDir(eMapNode *map, int * s_cost, int w, int x, int y);
    bool checkUncertainUTurn(eMapNode *map, int w, int x, int y);
};

#endif // CASTAR_H
