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
	std::string frontSign;
	std::string leftSign;
	std::string rightSign;

        CEnvInfo() : front(0), left(0), right(0), center(0) { }

	void clear()
	{
                front = left = right = center = 0;
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
    eMapNode_REDDOT
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
