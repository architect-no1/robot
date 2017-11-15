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
    bool checkAlreadyChecked(float *dirmap, int w, int h, int x, int y);
    bool checkAvailable(eMapNode *map, int w, int h, int x, int y);
    std::vector<cPoint> makePath(eMapNode *map, float *s_cost, int w, int gx, int gy);
    int checkMinDir(eMapNode *map, float *s_cost, int w, int x, int y);
    bool checkUncertainUTurn(eMapNode *map, int w, int x, int y);
    float calcCost_down(int bef_heading);
    float calcCost_left(int bef_heading);
    float calcCost_right(int bef_heading);
    float calcCost_up(int bef_heading);
};

#endif // CASTAR_H
