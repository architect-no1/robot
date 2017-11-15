#include <stdio.h>
#include <iostream>
#include "mapsolver_main.h"
#include <string>

int main(int argc, char *argv[])
{
    int algo_mode = 0;

    fprintf(stderr, "ver 1.3 red dot finish\n");

    if(argc > 1)
    {
        std::string arg_str(argv[1]);
        if(arg_str.find("-ff") != std::string::npos)
            algo_mode = 1;
    }

    std::string line;
    MapSolver_main mapSol_main(algo_mode);

    mapSol_main.init();

    while (true)
    {
        std::getline(std::cin, line);

        std::vector<std::string> outmsg = mapSol_main.process(line);

        if(outmsg.size() > 0)
        {
            for(int i = 0; i < outmsg.size(); i+=2)
            {
                if(outmsg[i].find("robot-control") != std::string::npos)
                    printf("%s\n", outmsg[i+1].c_str());
                else if(outmsg[i].find("algorithm-response") != std::string::npos)
                    printf("%s\n", outmsg[i+1].c_str());

                fflush(stdout);
            }
        }
    }

    return 0;
}
