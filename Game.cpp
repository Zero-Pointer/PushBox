#include "Game.h"
#include <queue>
#include <QDebug>

const int Game::WALL=-1;                        //墙
const int Game::AIM=1;                          //目标
const int Game::MAN=2;                          //小人
const int Game::BOX=4;                          //盒子
const int Game::mx[]={ 0, 0, 1,-1};
const int Game::my[]={ 1,-1, 0, 0};
const char Game::mv[]={'R','L','D','U'};        //移动
const int Game::RIGHT=0;                        //向右
const int Game::LEFT=1;                         //向左
const int Game::DOWN=2;                         //向下
const int Game::UP=3;                           //向上

Game::Game(int N,int M,string s):n(N),m(M)
{
    startStatus.aim=&aim;                       //startstatus的指针指向Game的aim
    startStatus.g=0;
    startStatus.pre=-1;
    mp=new int*[n];
    for(int i=0;i<n;i++)mp[i]=new int[m];       //展开n*m的地图
    wall=new bool*[n];
    for(int i=0;i<n;i++)wall[i]=new bool[m];    //声明n*m个墙判断
    isAim=new bool*[n];
    for(int i=0;i<n;i++)isAim[i]=new bool[m];   //声明n*m个目标判断
    string::iterator it=s.begin();              //定义一个it迭代器指向s的开头
    for(int i=0;i<n;i++)                        //铺地图
    {
        for(int j=0;j<m;j++)
        {
            isAim[i][j]=false;                  //初始化目标
            if(*it=='#')
                wall[i][j]=true;        //判断是不是墙
            else
            {
                wall[i][j]=false;
                switch(*it)                     //不是墙
                {
                case '*':                       //box
                    startStatus.box.insert(Point(i,j));
                    break;
                case '0':                       //人物
                    startStatus.man=Point(i,j);
                    break;
                case '+':                       //箱子+目标
                    startStatus.box.insert(Point(i,j)); //no break
                case '$':
                    aim.push_back(Point(i,j));  //
                    isAim[i][j]=true;
                    break;
                }
            }
            it++;                               //下一个字符
        }
    }
    restart();
}

Game::~Game()                                   //析构
{
    for(int i=0;i<n;i++)delete[]wall[i];
    delete[]wall;
    for(int i=0;i<n;i++)delete[]isAim[i];
    delete[]isAim;
    for(int i=0;i<n;i++)delete[]mp[i];
    delete[]mp;
}

void Game::goBack()                             //回退
{
    if(oldStatus.empty())return;                //如果没有历史操作
    mp[player.man.x][player.man.y]-=MAN;        //变回00（空）
    for(set<Point>::iterator i=player.box.begin();i!=player.box.end();i++)
        mp[i->x][i->y]-=BOX;
    player=oldStatus.top();oldStatus.pop();     //player记录最后一步位置，弹出历史记录
    mp[player.man.x][player.man.y]+=MAN;        //将人物放到上一步的位置
    for(set<Point>::iterator i=player.box.begin();i!=player.box.end();i++)
        mp[i->x][i->y]+=BOX;
}

const vector<int>& Game::solve()
{
    multiset<Status> ts;                        //用于记录走可能性
    map<Status,Status>searched;
    startStatus.h=-1;
    searched.insert(make_pair(startStatus,startStatus));
    startStatus.h=minGH;
    ts.insert(startStatus);
    while(ts.size())
    {
        if(minGH==-1)
            return ans;
        Status now(*ts.begin());
        ts.erase(ts.begin());
        minGH=min(minGH,now.h);
        now.h=-1;
        now.g++;

        for(int i=0;i<4;i++)
        {
            if(minGH==-1)
                return ans;
            Status next(now);
            next.pre=i;
            next.man.x+=mx[i];
            next.man.y+=my[i];
            if(wall[next.man.x][next.man.y])
                continue;
            if(next.box.erase(next.man))            //下一步是箱子，把箱子坐标除掉，之后再压入
            {
                Point pushBox=next.man;             //pushBox附小人的坐标
                pushBox.x+=mx[i];                   //箱子动
                pushBox.y+=my[i];                   //
                if(wall[pushBox.x][pushBox.y])
                    continue; //如果箱子撞墙，跳过
                if(next.box.find(pushBox)!=next.box.end())  //如果撞箱子
                    continue;
                if(!isAim[pushBox.x][pushBox.y])    //如果不是目标
                {
                    int flag=-1;                    //flag -1；并且可以记录方向，检测是否贴箱子
                    if(wall[pushBox.x+mx[i]][pushBox.y+my[i]])  //若箱子贴墙
                    {
                        if(wall[pushBox.x+mx[i^2]][pushBox.y+my[i^2]])continue;//验证是否在角落，逆时针90
                        if(wall[pushBox.x+mx[i^3]][pushBox.y+my[i^3]])continue;//顺时针90
                        int cntAim=0,cntBox=1;                  //这一条线上的目标数和箱子数，箱子数初始为（被推的那个）
                        bool ff=true;                           //判断是否进入隧道
                        for(int xx=pushBox.x+mx[i^2],yy=pushBox.y+my[i^2];!wall[xx][yy];xx+=mx[i^2],yy+=my[i^2])
                        {
                            if(!(wall[xx+mx[i]][yy+my[i]]||wall[xx+mx[i^1]][yy+my[i^1]]))   //如果两边都不是墙
                            {
                                ff=false;                       //返回false
                                break;
                            }
                            if(isAim[xx][yy])
                                cntAim++;
                            else if(next.box.find(Point(xx,yy))!=next.box.end())
                                cntBox++;
                        }
                        if(ff)
                            for(int xx=pushBox.x+mx[i^3],yy=pushBox.y+my[i^3];!wall[xx][yy];xx+=mx[i^3],yy+=my[i^3])
                        {
                            if(!(wall[xx+mx[i]][yy+my[i]]||wall[xx+mx[i^1]][yy+my[i^1]]))       //两边都不是墙
                            {
                                ff=false;
                                break;
                            }
                                                                //如果两面都是墙，数数路上有多少墙或者箱子
                            if(isAim[xx][yy])
                                cntAim++;
                            else if(next.box.find(Point(xx,yy))!=next.box.end())
                                cntBox++;
                        }
                                                                //检测推到墙边上之后，目标数是否满足箱子数
                        if(ff&&cntAim<cntBox)                   //如果有墙或者目标数少于路径箱子数
                            continue;                           //不这么走
                        if(next.box.find(Point(pushBox.x+mx[i^2],pushBox.y+my[i^2]))!=next.box.end())
                            flag=i^2;                           //查箱子两侧有没有箱子
                        else if(next.box.find(Point(pushBox.x+mx[i^3],pushBox.y+my[i^3]))!=next.box.end())
                            flag=i^3;
                    }
                    else if(next.box.find(Point(pushBox.x+mx[i],pushBox.y+my[i]))!=next.box.end())      //如果贴箱子
                    {
                        if(wall[pushBox.x+mx[i^2]][pushBox.y+my[i^2]])      //判断箱子两边会不会有墙（另一种死角）
                            flag=i;
                        else if(wall[pushBox.x+mx[i^3]][pushBox.y+my[i^3]])
                            flag=i;
                    }
                    if(flag!=-1)                                            //检测被贴的箱子是否陷入死角
                    {
                        if(wall[pushBox.x+mx[flag]+mx[flag^2]][pushBox.y+my[flag]+my[flag^2]])
                            continue;
                        if(wall[pushBox.x+mx[flag]+mx[flag^3]][pushBox.y+my[flag]+my[flag^3]])
                            continue;
                    }
                }

                next.box.insert(pushBox);   //全部通过，就把这个箱子推一步

            }
            if(searched.find(next)!=searched.end())         //防止走重
                continue;
            searched.insert(make_pair(next,now));
            next.solveH();
            if(next.h==0)
            {
                next.h=-1;
                ans.resize(next.g);
                for(int i=next.g-1;i>=0;i--)
                {
                    ans[i]=next.pre;
                    map<Status,Status>::iterator it=searched.find(next);
                    next=it->second;
                }
                return ans;
            }
            ts.insert(next);
        }
    }
    ans.resize(0);
    return ans;
}

void Game::restart()                            //重开
{
    while(!oldStatus.empty())oldStatus.pop();   //弹出所有历史记录
    player=startStatus;                         //回到最初状态
    for(int i=0;i<n;i++)                        //重新铺地图
    {
        for(int j=0;j<m;j++)
        {
            if(wall[i][j])mp[i][j]=WALL;
            else
            {
                if(isAim[i][j])mp[i][j]=AIM;
                else mp[i][j]=0;
            }
        }
    }
    mp[player.man.x][player.man.y]+=MAN;        //重新设置人位置
    for(set<Point>::iterator i=player.box.begin();i!=player.box.end();i++)
        mp[i->x][i->y]+=BOX;                    //设置箱子
}

bool Game::isWin()                              //判断是否赢
{
    for(set<Point>::iterator i=player.box.begin();i!=player.box.end();i++)
        if(!isAim[i->x][i->y])
            return false;
    return true;                                //箱子的位置都是目标就胜利
}

void Game::move(int k)                          //移动
{
    if(k<0||k>=4)return;                        //如果超出限制就直接返回
    Point p(player.man.x+mx[k],player.man.y+my[k]);     //将人物下一步坐标赋给P
    if(wall[p.x][p.y])return;                   //如果是墙不操作（人撞墙）
    if(player.box.find(p)!=player.box.end())    //P有箱子
    {
        Point pp(p.x+mx[k],p.y+my[k]);          //PP再沿路径走一步
        if(wall[pp.x][pp.y])return;             //如果PP是墙就不操作（箱子撞墙）
        if(player.box.find(pp)!=player.box.end())return;    //如果箱子被箱子挡住
        oldStatus.push(player);                 //将小人当前位置记录进历史记录
        mp[p.x][p.y]-=BOX;                      //
        player.box.erase(p);
        mp[pp.x][pp.y]+=BOX;
        player.box.insert(pp);
    }
    else oldStatus.push(player);                //P没箱子，也没墙（上面判断），记录进历史记录
    mp[player.man.x][player.man.y]-=MAN;        //去除小人原位置
    player.man=p;                               //P转正
    mp[player.man.x][player.man.y]+=MAN;        //给小人附新位置
    player.g++;                                 //步数+1
}


vector<int> Game::move(int x,int y)             //鼠标移动
{
    vector<int>v(0);                            //定义一个为0的点容器
    if(wall[x][y]||(player.man.x==x&&player.man.y==y))return v;     //点的墙或点的人直接返回
    if(player.box.find(Point(x,y))!=player.box.end())           //如果点的是箱子
    {
        for(int i=0;i<4;i++)                                    //判断是不是挨着箱子
        {
            if(x==player.man.x+mx[i]&&y==player.man.y+my[i])    //就走一步（按着点的位置）
            {
                v.push_back(i);
                return v;
            }
        }
        return v;                                               //点的是箱子但是一步走不过去，直接返回
    }
                                                //不是箱子
    queue<Point>q;                              //定义一个q点队列
    int **searched=new int*[n];
    for(int i=0;i<n;i++)
    {
        searched[i]=new int[m];
        for(int j=0;j<m;j++)searched[i][j]=-1;  //定义一个全为-1的searched（WALL=-1）
    }
    searched[player.man.x][player.man.y]=4;     //将人的位置定义为4（BOX）
    q.push(player.man);                         //将人物坐标加进q
    bool flag=false;                            //定义一个flag，false
    while(!q.empty())                           //
    {
        Point now=q.front(),next;               //定义now是q的第一个元素，定义一个next对象
        q.pop();                                //弹出q第一个元素，弹空
        for(int i=0;i<4;i++)                    //判断怎么走到目标（X，Y）
        {
            next.x=now.x+mx[i];
            next.y=now.y+my[i];
            if(searched[next.x][next.y]!=-1)continue;   //如果走超了或者走过了
            searched[next.x][next.y]=i;          //附为方向
            if(wall[next.x][next.y]||player.box.find(next)!=player.box.end())continue;
                                                 //如果下一步是墙或者箱子，跳过这个走法
            if(next.x==x&&next.y==y)             //如果下一步就是点的那一步，走到了
            {
                flag=true;                          //flag立起来
                break;                              //循环停止
            }
            q.push(next);                           //将next压入q
        }
        if(flag)break;                              //如果没有，继续找
    }
    if(searched[x][y]!=-1)                          //如果被标记了方向
    {
        Point now(x,y);                             //定义now点
        while(searched[now.x][now.y]!=4)            //只要不是4（原来的位置）
        {
            int t=searched[now.x][now.y];           //提取地图上记录的数字
            v.push_back(t);                         //压进v
            now.x-=mx[t];                           //now走一步
            now.y-=my[t];
        }
        reverse(v.begin(),v.end());                 //返回完整路径v
    }
    for(int i=0;i<n;i++)delete[] searched[i];
    delete[] searched;
    return v;
}

int Game::getStep()                                 //获取步数
{
    return player.g;
}

void Game::run()
{
    if(ans.size())
    {
        minGH=0;
        return;
    }
    solve();
    minGH=0;
}

int Game::getH()
{
    startStatus.solveH();
    return minGH=startStatus.h;
}

