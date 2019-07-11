#ifndef EDGE_H
#define EDGE_H

class Edge{
public:
    int to;                 //箱子
    int v;                  //距离
	Edge()
	{
	}
	Edge(int To,int V)
	{
		to=To;
		v=V;
	}
};

#endif //#ifndef EDGE_H
