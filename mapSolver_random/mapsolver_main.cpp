#include <stdio.h>
#include "mapsolver_main.h"

MapSolver_main::MapSolver_main(int algo_mode)
{
    pMapSol = new CMapSolver(algo_mode);
}

MapSolver_main::~MapSolver_main()
{
    delete pMapSol;
}

void MapSolver_main::init()
{
    state = STATE_READY;
    befMovCmd = eMovCmd_STOP;

    mapMak.init();
    pMapSol->init();

    step = 0;
}

bool MapSolver_main::checkEndCondition(eMapNode * map, int width, int height, std::string * errMsg)
{
    bool rval = true;

    int cnt_goal = 0;
    int cnt_start = 0;
    int cnt_reddot = 0;

    for(int i = 0; i < height; i++)
    {
        for(int k = 0; k < width; k++)
        {
            if(map[i*width+k] == eMapNode_START)
                cnt_start++;
            if(map[i*width+k] == eMapNode_GOAL)
                cnt_goal++;
            if(map[i*width+k] >= eMapNode_REDDOT)
                cnt_reddot++;
        }
    }

    if(cnt_goal == 0)
    {
        rval = false;
        *errMsg = "Cannot detect GOAL\n";
    }
    if(cnt_start == 0)
    {
        rval = false;
        *errMsg = "Cannot detect START\n";
    }
    if(cnt_reddot == 0)
    {
        rval = false;
        *errMsg = "Cannot detect RED-DOT\n";
    }

    return rval;
}

std::vector<std::string> MapSolver_main::process(std::string msg)
{
    std::vector<std::string> out_msg;

    CEnvInfo envInfo;
    eMovCmd cmd_mov;
    envInfo.clear();

    CMD_TYPE cmd = parsing(msg, &cmd_mov, &envInfo) ;

    if(cmd == CMD_MAP_CLEAR)
    {
        step = 0;
        mapMak.init();

        out_msg.push_back("algorithm-response\n");
        out_msg.push_back("ack map clear\n");
    }
    else if(cmd == CMD_MAP_START)
    {
        step = 0;

        state = STATE_MAP_DRAW;

        mapMak.udatePos(eMovCmd_STOP);

        out_msg.push_back("algorithm-response\n");
        out_msg.push_back("ack map start\n");
    }
    else if(cmd == CMD_MAZE_START)
    {
        step = 0;
        state = STATE_MAZE_SOLVE;

        out_msg.push_back("algorithm-response\n");
        out_msg.push_back("ack maze start\n");
        out_msg.push_back("robot-control\n");
        out_msg.push_back("current\n");
    }
    else if(cmd == CMD_ACK_SIGN_CANNOT)
    {
        step = 0;
        mapMak.init();
        state = STATE_READY;

        out_msg.push_back("algorithm-response\n");
        out_msg.push_back("maze fail Cannot find SIGN\n");
    }
    else
    {
        if(state == STATE_MAP_DRAW || state == STATE_MAZE_SOLVE)
        {
            if(cmd == CMD_STOP)
            {
                state = STATE_READY;
            }
            else if(cmd == CMD_ACK || cmd == CMD_ACK_SIGN || cmd == CMD_ACK_CANNOT)
            {
                step++;

                // update map using robot local map

                // simulation mode
    #ifdef SIMUL_MODE
                simul_robo.makeEnvInfo(&envInfo);
    #endif

    #ifndef SIMUL_MODE
                befMovCmd = cmd_mov;
    #endif

                if(cmd == CMD_ACK)
                {
                    mapMak.udatePos(befMovCmd);
                    mapMak.updateCurCell(envInfo);
                }
                else if(cmd == CMD_ACK_SIGN)
                {
                    fprintf(stderr, "!!!!!!! ack sign \n");
                    mapMak.updateCurCell_Sign(envInfo);
                }
                else if(cmd == CMD_ACK_SIGN_CANNOT)
                {
                    fprintf(stderr, "!!!!!!! ack sign CANNOT \n");
                }

                fprintf(stderr, "cur map, robot(%d, %d)\n", mapMak.cur_x, mapMak.cur_y);
                mapMak.printf_curMap();

                // SEND CURRENT MAP
    #ifndef SIMUL_MODE
                out_msg.push_back("algorithm-response\n");
                out_msg.push_back(mapMak.send_map());
    #endif

                fprintf(stderr, "env info\n");
                fprintf(stderr, ".%d.\n", envInfo.front);
                fprintf(stderr, "%do%d\n", envInfo.left, envInfo.right);

                if(state == STATE_MAZE_SOLVE) // only works maze solving mode
                {
                    // make path to go to unknown, and store in queue, and pop 1 item
                    eMovCmd movCmd = pMapSol->MapBuilder_1step(mapMak.map
                                            , mapMak.map_size, mapMak.map_size
                                            , mapMak.cur_x, mapMak.cur_y, mapMak.cur_heading);

                    if(movCmd == eMovCmd_STOP)
                    {
                        out_msg.push_back("algorithm-response\n");

                        std::string errMsg;
                        bool conditionEnd = checkEndCondition(mapMak.map, mapMak.map_size, mapMak.map_size, &errMsg);
                        if(conditionEnd == true)
                            out_msg.push_back(format("maze end %d\n", step));
                        else
                            out_msg.push_back("maze fail " + errMsg);
                    }
                    else
                    {
                        // SEND MOV COMMAND TO ROBOT
                        out_msg.push_back("robot-control");
                        out_msg.push_back(pMapSol->translate_eMovCMd2String(movCmd));
                        fprintf(stderr, ">> robot command : %s\n\n"
                                , pMapSol->translate_eMovCMd2String(movCmd).c_str());
                    }
                }

                // simulation mode
    #ifdef SIMUL_MODE
                simul_robo.move(movCmd);
                fprintf(stderr, "result cur robot (%d, %d, %d)\n"
                        , simul_robo.cur_x, simul_robo.cur_y, simul_robo.cur_heading);
                simul_robo.DrawMap();

                befMovCmd = movCmd;
    #endif
            }
        }
    }

    return out_msg;
}

std::vector<std::string> MapSolver_main::split(const std::string& s, char seperator)
{
   std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}

void MapSolver_main::Str2Dir(std::string str, eMovCmd *robotMov)
{
    eMovCmd dir = eMovCmd_STOP;

    if(str.find("forward") != std::string::npos)
        dir = eMovCmd_FORWARD;
    else if(str.find("left") != std::string::npos)
        dir = eMovCmd_LEFT;
    else if(str.find("right") != std::string::npos)
        dir = eMovCmd_RIGHT;
    else if(str.find("backward") != std::string::npos)
        dir = eMovCmd_UTURN;
    else if(str.find("sign") != std::string::npos)
        dir = eMovCmd_STOP;

    *robotMov = dir;
}

void MapSolver_main::Str2Env(std::vector<std::string> str_list, CEnvInfo *env)
{
    env->clear();

    if(str_list[1][0] == 'x')
        env->front = 1;
    if(str_list[3][0] == 'x')
        env->left = 1;
    if(str_list[5][0] == 'x')
        env->right = 1;

    if(str_list[4][0] == 'b')
        env->center = 1; // start
    else if(str_list[4][0] == 'f')
        env->center = 2; // end
    else if(str_list[4][0] == 'j')
        env->center = 3; // red dot
}

void MapSolver_main::Str2Sign(std::string str_list, CEnvInfo *env)
{
    env->clear();

    if(str_list[0] == 'w')
        env->frontSign = format("%c", str_list[1]);
    if(str_list[0] == 'a')
        env->leftSign = format("%c", str_list[1]);
    if(str_list[0] == 'd')
        env->rightSign = format("%c", str_list[1]);

//    fprintf(stderr, "--- char : (%c) received sign : %s -> (%s, %s, %s)"
//            , str_list[0]
//            , str_list.c_str()
//            , env->leftSign.c_str()
//            , env->frontSign.c_str()
//            , env->rightSign.c_str());
}

CMD_TYPE MapSolver_main::parsing(std::string line, eMovCmd *robotMov, CEnvInfo *env)
{
    CMD_TYPE rval = CMD_NOTHING;

    std::vector<std::string> strs = split(line, ' ');

    if(strs.size() == 1) // 1 word command
    {
        if(strs[0].find("q") != std::string::npos)
            rval = CMD_QUIT;
    }
    else if(strs.size() == 2) // 2 word command
    {
        if(strs[0].find("maze") != std::string::npos
          && strs[1].find("start") != std::string::npos  )
        {
            rval = CMD_MAZE_START;
        }
        else if(strs[0].find("map") != std::string::npos)
        {
            if(strs[1].find("start") != std::string::npos  )
                rval = CMD_MAP_START;
            else
                rval = CMD_MAP_CLEAR;
        }
    }
    else
    {
        if(strs[0].find("ack") != std::string::npos)
        {
            if(strs[1].find("sign") != std::string::npos)
            {
                if(strs.size() < 3 || strs[2].size() == 0)
                {
                    fprintf(stderr, "!!!!!! sign cannot recognized\n");
                    rval = CMD_ACK_SIGN_CANNOT;
                }
                else if(strs[2].find("cannot") != std::string::npos)
                    rval = CMD_ACK_SIGN_CANNOT;
                else
                {
                    rval = CMD_ACK_SIGN;
                    Str2Sign(strs[2], env);
                }
            }
            else if(strs[2].find("cannot") != std::string::npos)
            {
                rval = CMD_ACK_CANNOT;
            }
            else
            {
                rval = CMD_ACK;

                Str2Dir(strs[1], robotMov);

                std::vector<std::string> env_str;
                for(int i = 2; i < strs.size(); i++)
                    env_str.push_back(strs[i]);

                if(env_str.size() < 5)
                    fprintf(stderr, "too short robot map");
                else
                {
                    Str2Env(env_str, env);

                    if(*robotMov == eMovCmd_UTURN)
                        env->back = 1;
                }
            }
        }
    }


    fflush(stdout);

    return rval;
}
