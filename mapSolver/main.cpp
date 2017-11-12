#include <stdio.h>
#include <iostream>
#include "mapsolver_main.h"

int main(int argc, char *argv[])
{
    std::string line;
    MapSolver_main mapSol_main;

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
                    printf("%s\r\n", outmsg[i+1].c_str());
                else if(outmsg[i].find("algorithm-response") != std::string::npos)
                    printf("%s\r\n", outmsg[i+1].c_str());

                fflush(stdout);
            }
        }
    }

    return 0;
}
