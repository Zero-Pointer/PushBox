#ifndef STATUS_H
#define STATUS_H
#include <set>
#include "Point.h"
#include "Game.h"
#include "KM.h"

#define INF 0x3f3f3f3f

class Status
{
public:
    vector<Point> *aim;
    int g;
    int h;
    int pre;
    set<Point> box;
    Point man;
    Status()
    {
    }
    Status(const Status &o)
    {
    	g=o.g;
    	h=o.h;
    	man=o.man;
    	pre=o.pre;
    	aim=o.aim;
        for(set<Point>::iterator i=o.box.begin();i!=o.box.end();i++)box.insert(*i);
    }
    void solveH()
    {
        KM km(aim->size());
        set<Point>::iterator it;
		int i;
		h=INF;
    	for(it=box.begin(),i=0;it!=box.end();it++,i++)
    	{
    		h=min(h,man-*it-1);
            for(unsigned int j=0;j<aim->size();j++)
    		    km.add_edge(i,j,-(*it-((*aim)[j])));
    	}
    	h+=-km.solve();
//    	h=min(1,h);
    }
    void operator=(const Status &o)
    {
    	g=o.g;
    	h=o.h;
    	man=o.man;
    	pre=o.pre;
    	aim=o.aim;
    	box.clear();
        for(set<Point>::iterator i=o.box.begin();i!=o.box.end();i++)box.insert(*i);
    }
    bool operator<(const Status &o)const
    {
    	if(h==-1)
    	{
    		if(man!=o.man)return man<o.man;
            set<Point>::iterator i=box.begin(),j=o.box.begin();
    		while(i!=box.end())
    		{
    		    if(*i!=*j)return *i<*j;
    		    i++;j++;
    		}
    		return false;
    	}
    	return g+h<o.g+o.h;
    }
    bool operator!=(const Status &o)const
    {
        return !(*this==o);
    }
    bool operator==(const Status &o)const
    {
    	if(h==-1)
    	{
		    if(man!=o.man)return false;
            set<Point>::iterator i=box.begin(),j=o.box.begin();
    		while(i!=box.end())
    		{
    		    if(*i!=*j)return false;
    		    i++;j++;
    		}
    		return true;
    	}
    	return g+h==o.g+o.h;
    }
};

#endif // STATUS_H
