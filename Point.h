#ifndef POINT_H
#define POINT_H
#include <algorithm>

using namespace std;

class Point
{
public:
    int x;
    int y;
    Point()
    {
    }
    Point(int X,int Y)
    {
    	x=X;
    	y=Y;
    }
    bool operator<(const Point &o)const
    {
    	if(x!=o.x)return x<o.x;
    	return y<o.y;
    }
	bool operator==(const Point &o)const
    {
    	if(x==o.x&&y==o.y)return true;
    	return false;
    }
	bool operator!=(const Point &o)const
    {
    	if(x==o.x&&y==o.y)return false;
    	return true;
    }
    int operator-(const Point &o)const
    {
    	return abs(x-o.x)+abs(y-o.y);
    }
};

#endif // POINT_H
