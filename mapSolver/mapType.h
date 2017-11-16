#pragma once

#include <string>
#include <stdio.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

//#define SIMUL_MODE

class CEnvInfo
{
public:
    int front; // 1 wall
	int left;
	int right;
    int center; // 1 start, 2 end, 3 red dot

    int back; // only use for u-turn

	std::string frontSign;
	std::string leftSign;
	std::string rightSign;

        CEnvInfo() : front(0), left(0), right(0), center(0), back(0) { }

	void clear()
	{
        front = left = right = center = back = 0;
		frontSign = leftSign = rightSign = "";
	}
};

class cPoint
{
public:
	int x;
	int y;

	cPoint(int px, int py)
	{
		x = px;
		y = py;
	};

	cPoint()
	{
		x = y = 0;
	};
};

enum eMovCmd
{
        eMovCmd_STOP,
        eMovCmd_FORWARD,
        eMovCmd_LEFT,
        eMovCmd_RIGHT,
        eMovCmd_UTURN,
        eMovCmd_SEARCH_SIGN
};

enum eMapNode
{
    eMapNode_UNKNOWN,
    eMapNode_BACK,
    eMapNode_CLEAR,
    eMapNode_WALL,
    eMapNode_START,
    eMapNode_GOAL,
    eMapNode_REDDOT,

    eMapNode_W0, // 7
    eMapNode_W1,
    eMapNode_W2,
    eMapNode_W3,
    eMapNode_W4,
    eMapNode_W5,
    eMapNode_W6,
    eMapNode_W7,
    eMapNode_W8,
    eMapNode_W9,
    eMapNode_W10, // 17

    eMapNode_A0, // 18
    eMapNode_A1,
    eMapNode_A2,
    eMapNode_A3,
    eMapNode_A4,
    eMapNode_A5,
    eMapNode_A6,
    eMapNode_A7,
    eMapNode_A8,
    eMapNode_A9,
    eMapNode_A10, // 28

    eMapNode_D0,
    eMapNode_D1,
    eMapNode_D2,
    eMapNode_D3,
    eMapNode_D4,
    eMapNode_D5,
    eMapNode_D6,
    eMapNode_D7,
    eMapNode_D8,
    eMapNode_D9,
    eMapNode_D10,

    eMapNode_S0,
    eMapNode_S1,
    eMapNode_S2,
    eMapNode_S3,
    eMapNode_S4,
    eMapNode_S5,
    eMapNode_S6,
    eMapNode_S7,
    eMapNode_S8,
    eMapNode_S9,
    eMapNode_S10
};

//missing string printf
//this is safe and convenient but not exactly efficient
inline std::string format(const char* fmt, ...)
{
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize){ //fail delete buffer and try again
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize+1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}
