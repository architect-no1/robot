#ifndef MAPSOLVER_MAIN_H
#define MAPSOLVER_MAIN_H

using namespace std;

#include "mapType.h"
#include "CMapSolver.h"
#include <iostream>
#include "CSimulRobot.h"
#include "MapMaker.h"

#define MAX_MAP_SIZE 50

enum CMD_TYPE
{
    CMD_NOTHING,
    CMD_MAZE_START,
    CMD_MAP_START,
    CMD_MAP_CLEAR,
    CMD_STOP,
    CMD_ACK,
    CMD_ACK_CANNOT,
    CMD_ACK_SIGN,
    CMD_ACK_SIGN_CANNOT,

    CMD_QUIT
};

enum STATE
{
    STATE_READY,
    STATE_MAZE_SOLVE,
    STATE_MAP_DRAW
};

class MapSolver_main
{
public:
    MapSolver_main(int algo_mode);
    ~MapSolver_main();

    void init();
    std::vector<std::string> process(std::string msg);

private:
    std::vector<std::string> split(const std::string& s, char seperator);
    void Str2Dir(std::string str, eMovCmd *robotMov);
    void Str2Env(std::vector<std::string> str_list, CEnvInfo *env);
    CMD_TYPE parsing(std::string line, eMovCmd *robotMov, CEnvInfo *env);
    void Str2Sign(std::string str_list, CEnvInfo *env);
    bool checkEndCondition(eMapNode * map, int width, int height, string *errMsg);


    eMovCmd befMovCmd;
    CSimulRobot simul_robo;

    CMapSolver * pMapSol;
    MapMaker mapMak;

    int step;

    STATE state;
};

#endif // MAPSOLVER_MAIN_H
