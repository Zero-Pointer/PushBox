#ifndef KM_H                                     //2017.1.5
#define KM_H
#include "Edge.h"
#include <iostream>
#include <vector>

#define INF 0x3f3f3f3f

using namespace std;

class KM{
private:
	int n;
    vector<Edge> *edge;
	int *from;
	int *X;
	int *Y;
	bool *L;
	bool *R;
	int ans;
	void init()
	{
		ans=0;
	    for(int i=0;i<n;i++)
		{
		    from[i]=-1;
		    Y[i]=0;
		    X[i]=-INF;
            for(vector<Edge>::iterator j=edge[i].begin();j!=edge[i].end();j++)
		    {
		    	X[i]=max(X[i],j->v);
		    }
		    ans+=X[i];
		}
	}
	
	bool dfs(int nown)
	{
		L[nown]=true;
        for(vector<Edge>::iterator i=edge[nown].begin();i!=edge[nown].end();i++)
		{
		    if(X[nown]+Y[i->to]<i->v)
			{
				printf("KM wrong!\n");
				exit(1);
			}
			if(X[nown]+Y[i->to]!=i->v)continue;
			
			if(R[i->to])continue;
			R[i->to]=true;
			if(from[i->to]==-1||dfs(from[i->to]))
			{
				from[i->to]=nown;
			    return true;
			}
		}
		return false;
	}
	
public:
    int solve()
	{
		if(ans!=INF)return ans;
		init();
	    for(int k=0;k<n;k++)
		{
			for(int i=0;i<n;i++)R[i]=L[i]=false;
			if(dfs(k))continue;
			int d=INF;
			for(int i=0;i<n;i++)
				if(L[i])
                    for(vector<Edge>::iterator j=edge[i].begin();j!=edge[i].end();j++)
						if(!R[j->to])
					    	d=min(d,X[i]+Y[j->to]-j->v);
			ans-=d;
			for(int i=0;i<n;i++)
				if(L[i])
				    X[i]-=d;
			for(int i=0;i<n;i++)
				if(R[i])
				    Y[i]+=d;
			k--;
	    }
	    return ans;
	}
	KM(int N)
	{
		ans=INF;
		n=N;
        edge=new vector<Edge>[n];
		from=new int[n];
		X   =new int[n];
		Y   =new int[n];
		L   =new bool[n];
		R   =new bool[n];
	}
	~KM()
    {
		delete[] edge;
		delete[] from;
		delete[] X;
		delete[] Y;
		delete[] L;
		delete[] R;
	}
	
	void add_edge(int a,int b,int v)
	{
        edge[a].push_back(Edge(b,v));
	}
	
	int getFrom(int y)
	{
		return from[y];
	}
	
};

#endif //#ifndef KM_H
