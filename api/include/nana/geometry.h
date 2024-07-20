#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <algorithm>
#include <functional>

using namespace std;

typedef function <double(double)> func1d;
typedef function <double(double, double)> func2d;

struct real_point
{
	double x, y;
};

struct real_rectangle
{
	double x, y, width, height;
	real_rectangle () : x (0.0), y (0.0), width (0.0), height (0.0)
	{}

	real_rectangle (double x, double y, double width, double height) : x (x), y (y), width (width), height (height)
	{}
	
	real_rectangle(real_point a, real_point b) : x(min (a.x, b.x)), y(min (a.y, b.y)), width(max (a.x,b.x) - x), height(max (a.y,b.y) - y)
	{}
};

template<typename T> T abs(T x) { return x * x; }
template<typename T> T sqr(T x) { return x * x; }
template<typename T> T vect(T x1, T y1, T x2, T y2) { return x1 * y2 - y1 * x2; }

#endif
