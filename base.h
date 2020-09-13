/** base header file **/
#ifndef _BASE_H
#define _BASE_H
/** Included files **/
//#include <iostream>

/** Define macros variables **/
#define PI 3.1416//59265

/** Define macro functions **/
#define DTR(file,value) ff.read(reinterpret_cast<char*>(&value),sizeof(value))

/** Define custom variables **/
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

/// Declare structs
struct s_point
{
	float x, y;

	s_point operator +(s_point p)
	{
        s_point q;
        q.x = x + p.x;
        q.y = y + p.y;
        return q;
	};
	s_point operator -(s_point p)
	{
        s_point q;
        q.x = x - p.x;
        q.y = y - p.y;
        return q;
	};
	s_point operator *(float f)
	{
        s_point q;
        q.x = x * f;
        q.y = y * f;
        return q;
	};
	s_point operator /(float f)
	{
        s_point q;
        q.x = x / f;
        q.y = y / f;
        return q;
	};
	void operator +=(s_point p)
	{
        x += p.x;
        y += p.y;
	};
	void operator -=(s_point p)
	{
        x -= p.x;
        y -= p.y;
	};

	// Comperators
	bool operator ==(s_point p) { return x == p.x && y == p.y; };
	bool operator !=(s_point p) { return !(x == p.x && y == p.y); };
};

/** Endif **/
#endif
