#include "mainwindow.h"
#include <QString>
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QPainter>
#include <QColor>
#include <QAction>
#include <QProgressDialog>
#include <QFileDialog>
#include <QCoreApplication>
#include <QtCore>
#include <QFontDatabase>
#include <QApplication>
#include <QTime>
#include  <QTime>
#include <QDialog>
#include <QMediaPlayer>
using namespace std;

const int BOXSIZE=60;
const int MENUSIZE=23;
const int PANELX=4*BOXSIZE;
const int PANELY=MENUSIZE;
const char title[]="推箱子 1.0";

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
        myplaylist = new QMediaPlaylist(this);        /*设定循环播放背景音乐*/
        myplaylist->addMedia(QUrl("qrc:/music/BGM.mp3"));
        myplaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        myplayer = new QMediaPlayer(this);
        myplayer->setPlaylist(myplaylist);
        myplayer->setVolume(80);
        myplayer->play();
    setWindowIcon(QIcon(":/icon/favicon0.ico"));        /*设定窗口图标*/
    cnt=0;
    setFocus();        /*设定焦点*/
    wallPixmap.load(":/image/wall0.png");        /*设定图片*/
    aimPixmap.load(":/image/aim0.png");
    manPixmap.load(":/image/man.png");
    manOnAimPixmap.load(":/image/manOnAim.png");
    boxPixmap.load(":/image/box1.png");
    boxOnAimPixmap.load(":/image/box2.png");
    wallPixmap=wallPixmap.scaled(BOXSIZE,BOXSIZE);        /*设定图片大小*/
    aimPixmap=aimPixmap.scaled(BOXSIZE,BOXSIZE);
    manPixmap=manPixmap.scaled(BOXSIZE,BOXSIZE);
    manOnAimPixmap=manOnAimPixmap.scaled(BOXSIZE,BOXSIZE);
    boxPixmap=boxPixmap.scaled(BOXSIZE,BOXSIZE);
    boxOnAimPixmap=boxOnAimPixmap.scaled(BOXSIZE,BOXSIZE);
    this->setWindowTitle(title);        /*设定窗口名字*/
    roundLabel=new QLabel(this);        /*关数标签*/
    roundLabel->setAlignment(Qt::AlignCenter);        /*标签居中*/
    stepLabel=new QLabel(this);        /*步数标签*/
    nextBtn=new QPushButton(QString("下一关"),this);       /*四个在游戏界面的按钮*/
    restartBtn=new QPushButton(QString("重新开始"),this);
    goBackBtn=new QPushButton(QString("回退"),this);
    preBtn=new QPushButton(QString("上一关"),this);

    menu=menuBar()->addMenu(tr("选项"));      /*第一个菜单栏*/
    QAction *action=menu->addAction(tr("读取外部关卡"));      /*添加菜单选项*/
    connect(action,SIGNAL(triggered()),this,SLOT(onActionClicked()));      /*建立按钮与函数的联系*/
    menu->addSeparator();      /*添加菜单分割线*/
    QAction *action2=menu->addAction(tr("重新开始"));      /*添加菜单选项*/
    connect(action2,SIGNAL(triggered()),this,SLOT(onRestartBtnClicked()));      /*建立按钮与函数的联系*/
    menu->addSeparator();      /*添加菜单分割线*/
    QAction *action3=menu->addAction(tr("上一关"));      /*添加菜单选项*/
    connect(action3,SIGNAL(triggered()),this,SLOT(onPreBtnClicked()));      /*建立按钮与函数的联系*/
    QAction *action1=menu->addAction(tr("下一关"));      /*添加菜单选项*/
    connect(action1,SIGNAL(triggered()),this,SLOT(onNextBtnClicked()));      /*建立按钮与函数的联系*/

    QMenu *aboutMenu=menuBar()->addMenu(tr("帮助"));      /*第二个菜单栏*/
    QAction *helpAction=aboutMenu->addAction(tr("快捷键"));      /*添加菜单选项*/
    connect(helpAction,SIGNAL(triggered()),this,SLOT(showHelpInformation()));      /*建立按钮与函数的联系*/
    QAction *aboutAction=aboutMenu->addAction(tr("关于"));      /*添加菜单选项*/
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(showAboutInformation()));      /*建立按钮与函数的联系*/

    connect(nextBtn,SIGNAL(clicked()),this,SLOT(onNextBtnClicked()));      /*四个在游戏界面的按钮*/
    connect(restartBtn,SIGNAL(clicked()),this,SLOT(onRestartBtnClicked()));
    connect(goBackBtn,SIGNAL(clicked()),this,SLOT(onGoBackBtnClicked()));
    connect(preBtn,SIGNAL(clicked()),this,SLOT(onPreBtnClicked()));      /*建立按钮与函数的联系*/


    game=NULL;
    round=0;        /*初始关卡为第0关*/
    loadGame();
}

bool MainWindow::loadGame(QString filename,QString qs)
{
    QFile file(filename);       /*构造一个以filename为文件名的QFile对象*/
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))        /*规定打开的文件格式*/
        return false;
    delete game;
    string str=QString(file.readAll()).toStdString();        /*按照string格式读取文件中全部内容*/
    istringstream sin(str);
    string s,ss;
    int n,m;
    sin>>n>>m;      /*读取行数和列数*/
    getline(sin,ss);
    while(getline(sin,ss))      /*按行读取文件*/
        s.append(ss);
    while(n<7)      /*防止地图太小而使界面显示不全*/
    {
        n++;
        for(int i=0;i<m;i++)
            s.append(" ");
    }
    game=new Game(n,m,s);
    repaint();
    setFixedSize(BOXSIZE*(m+4),MENUSIZE+BOXSIZE*n);      /*设定窗口部件的大小*/
    QFont font;
    font.setPointSize(BOXSIZE/2);      /*设定字体的大小*/
    int x=BOXSIZE/2,y=MENUSIZE+BOXSIZE/2;
    int w=BOXSIZE*3,h=BOXSIZE;
    roundLabel->setGeometry(x,y,w,h);      /*设定标签的位置和大小*/
    if(qs!=tr(""))roundLabel->setText("第"+qs+"关");
    else roundLabel->setText("自定义");
    roundLabel->setFont(font);
    y+=BOXSIZE+BOXSIZE/2;      /*设定按钮的位置和大小*/
    preBtn->setGeometry(x,y,w,h);
    y+=BOXSIZE+BOXSIZE/2;
    restartBtn->setGeometry(x,y,w/2-2,h);
    goBackBtn->setGeometry(x+w/2+2,y,w/2-2,h);
    y+=BOXSIZE+BOXSIZE/2;
    nextBtn->setGeometry(x,y,w,h);
    y=(y+n*(BOXSIZE))/2+MENUSIZE/2;
    font.setPointSize(BOXSIZE/3);      /*设定标签的位置和大小*/
    stepLabel->setFont(font);
    stepLabel->setText("步数: 0");
    stepLabel->setGeometry(x,y,w,h);
    return true;
}

bool MainWindow::loadGame()      /*读取文件重载游戏*/
{
    QString qs;
    qs.setNum(round);
    return loadGame(":/map/map"+qs+".txt",qs);
}

MainWindow::~MainWindow()      /*析构函数*/
{
    delete game;
    delete roundLabel;
    delete stepLabel;
    delete nextBtn;
    delete restartBtn;
    delete preBtn;
    delete goBackBtn;
}

void MainWindow::nextStep()     /*自动寻路下一步*/
{
    game->move(*showAnsIt);
    showAnsIt++;
    checkGame();
    if(showAnsIt==showAns.end())
    {
        cnt=0;
    }
}

void MainWindow::preStep()     /*自动寻路上一步*/
{
    if(showAnsIt==showAns.begin())return;
    game->goBack();
    showAnsIt--;
    checkGame();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if(cnt==3)           /*自动寻路模式下文字颜色变红*/
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::red);
        stepLabel->setPalette(pe);
    }
    else
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::black);
        stepLabel->setPalette(pe);
    }
    QPainter painter(this);     /*背景颜色*/
    QColor color(240,240,240);
    painter.setPen(color);
    painter.setBrush(QBrush(color,Qt::SolidPattern));     /*背景颜色填充效果*/
    painter.drawRect(QRect(PANELX,PANELY,BOXSIZE*game->m,BOXSIZE*game->n));     /*背景颜色填充范围*/
    for(int i=0;i<game->n;i++)     /*在背景上添加图标*/
    {
        for(int j=0;j<game->m;j++)
        {       /*值为0是没有图片*/
            if(game->mp[i][j]==Game::WALL)       /*值为-1是墙图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE,PANELY+i*BOXSIZE,BOXSIZE,BOXSIZE,wallPixmap);
            else if(game->mp[i][j]==Game::AIM)        /*值为1是目的地图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE+15,PANELY+i*BOXSIZE+15,30,30,aimPixmap);
            else if(game->mp[i][j]==Game::MAN)      /*值为2是人图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE+10,PANELY+i*BOXSIZE,40,60,manPixmap);
            else if(game->mp[i][j]==(Game::MAN+Game::AIM))       /*值为3是人在目的地图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE+10,PANELY+i*BOXSIZE,40,60,manOnAimPixmap);
            else if(game->mp[i][j]==Game::BOX)       /*值为4是箱子图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE,PANELY+i*BOXSIZE,BOXSIZE,BOXSIZE,boxPixmap);
            else if(game->mp[i][j]==(Game::BOX+Game::AIM))       /*值为5是箱子在目的地图片*/
                painter.drawPixmap(PANELX+j*BOXSIZE,PANELY+i*BOXSIZE,BOXSIZE,BOXSIZE,boxOnAimPixmap);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QMutexLocker locker(&mutex);
    if(event->button()==Qt::LeftButton)     /*点击鼠标左键*/
    {    player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        if(cnt==3)nextStep();
        else
        {
            int y=(event->pos().x()-PANELX)/BOXSIZE,x=(event->pos().y()-PANELY)/BOXSIZE;     /*返回x和y的单位值*/
            if(x<0||x>=game->n||y<0||y>=game->m)
                return;
            vector<int>v=game->move(x,y);     /*获取移动路径*/
            for(vector<int>::iterator i=v.begin();i!=v.end();i++)
            {
               game->move(*i);     /*将鼠标移动函数重载为键盘移动函数*/
               QElapsedTimer timer;     /*防止界面冻结*/
               timer.start();
               while(timer.elapsed()<50);
               checkGame();
            }
        }
    }
    else if(event->button()==Qt::RightButton)     /*点击鼠标右键*/
    {    player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        if(cnt==3)
            preStep();

        else
            onGoBackBtnClicked();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    QMutexLocker locker(&mutex);
    if(cnt==3)     /*自动寻路模式下键盘操作*/
    {
        switch(e->key())
        {
        case Qt::Key_Escape:
            cnt=0;
            checkGame();
            break;
        case Qt::Key_Left:
            player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
            player.play();
            preStep();
            break;
        case Qt::Key_Right:
            player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
            player.play();
            nextStep();
            break;
        }
        return;
    }
    if(cnt==2)     /*判断进入自动寻路模式*/
    {
        if(e->key()=='9')     /*进入自动寻路模式*/
        {
            QFont adfont;
            adfont.setPointSize(BOXSIZE/2);
            cnt++;
            dlg.setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
            dlg.resize(400,400);
            QLabel label(&dlg);
            label.setText(QObject::tr("广告招租"));
            label.move(100,(400-BOXSIZE/2)/2);
            label.setFont(adfont);
            QTimer::singleShot(6000, &dlg,&QDialog::close);
            dlg.exec();
            game->start();
            int nowV=0,maxV=game->getH();
            int maxRange=6666;
            QProgressDialog process(this);      /*进度条对话框*/
            QString qs;
            process.setLabelText(tr("估计步数:")+qs.setNum(maxV*10));      /*进度条文本内容*/
            process.setWindowTitle(tr("搜索路径中…"));      /*进度条标题*/
            process.setWindowModality(Qt::WindowModal);      /*其他窗口不响应输入信号*/
            process.setRange(0,maxRange);      /*处理路线数量*/
            process.setCancelButtonText(tr("取消"));      /*取消按钮*/
            process.setMinimumDuration(0);      /*出现进度条的等待时间*/
            process.setMinimumWidth(300);      /*对话框最小宽度*/
            process.show();
            while(nowV<maxRange)
            {
                nowV=min(nowV+1,(maxV-game->minGH)*maxRange/maxV);      /*计算已处理路线的数量*/
                process.setValue(nowV);
                QApplication::processEvents();      /*在处理路线时防止程序不响应*/
                if(process.wasCanceled())      /*取消搜索*/
                {
                    game->minGH=-1;
                    game->wait();
                    cnt=0;
                    return;
                }
            }
            game->wait();
            showAns=game->ans;
            showAnsIt=showAns.begin();
            game->restart();
            checkGame();
            return;
        }
        else    cnt=0;
    }
    if(cnt==1)
    {
        if(e->key()=='1')
            cnt++;
        else    cnt=0;
    }
    if(cnt==0)
    {
        if(e->key()=='2')
            cnt++;
        else    cnt=0;
    }
    switch (e->key())       /*键盘按钮控制*/
    {
    case 'W':
    case Qt::Key_Up:
        player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        game->move(Game::UP);
        break;
    case 'S':
    case Qt::Key_Down:
        player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        game->move(Game::DOWN);
        break;
    case 'A':
    case Qt::Key_Left:
        player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        game->move(Game::LEFT);
        break;
    case 'D':
    case Qt::Key_Right:
        player.setMedia(QUrl::fromLocalFile("qrc:/music/active.mp3"));
        player.play();
        game->move(Game::RIGHT);
        break;
    case 'R':
        onRestartBtnClicked();
        break;
    case 'P':
        onGoBackBtnClicked();
    }
    checkGame();
}

void MainWindow::checkGame()
{
    repaint();
    QString s;
    stepLabel->setText("步数: "+s.setNum(game->getStep()));     /*记录步数*/
    if(game->isWin())
    {       /*通关*/
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("恭喜过关"),
                                            tr("使用")+stepLabel->text()+"\n开始下一关?",
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)

            round++;
        else
        {
            game->restart();
            checkGame();
            return;
        }
        if(!loadGame())     /*全部通关*/
        {
            reply = QMessageBox::question(this, tr("恭喜通关"),
                                                tr("您已通关,是否从第零关开始?"),
                                                QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                round=0;
                loadGame();
            }
            else if(reply == QMessageBox::No)
                exit(0);
            else round--;
        }
    }
}

void MainWindow::onNextBtnClicked()      /*下一关*/
{
    if(cnt==3)
    {
        cnt=0;
    }
    round++;
    if(!loadGame())round--;
    update();
    delete myplayer;
        delete myplaylist;
        myplaylist = new QMediaPlaylist(this);
        myplaylist->addMedia(QUrl::fromLocalFile("D:\\BGM.mp3"));
        myplaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        myplayer = new QMediaPlayer(this);
        myplayer->setPlaylist(myplaylist);
        myplayer->setVolume(80);
        myplayer->play();
    setFocus();
}

void MainWindow::onRestartBtnClicked()      /*重新开始*/
{
    if(cnt==3)
    {
        cnt=0;
    }
    game->restart();
    checkGame();
    delete myplayer;
        delete myplaylist;
        myplaylist = new QMediaPlaylist(this);
        myplaylist->addMedia(QUrl::fromLocalFile("D:\\BGM.mp3"));
        myplaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        myplayer = new QMediaPlayer(this);
        myplayer->setPlaylist(myplaylist);
        myplayer->setVolume(80);
        myplayer->play();
    setFocus();
}

void MainWindow::onPreBtnClicked()      /*上一关*/
{
    if(cnt==3)
    {
        cnt=0;
    }
    round--;
    if(!loadGame())round++;
    update();
    delete myplayer;
        delete myplaylist;
        myplaylist = new QMediaPlaylist(this);
        myplaylist->addMedia(QUrl::fromLocalFile("D:\\BGM.mp3"));
        myplaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        myplayer = new QMediaPlayer(this);
        myplayer->setPlaylist(myplaylist);
        myplayer->setVolume(80);
        myplayer->play();
    setFocus();
}

void MainWindow::onGoBackBtnClicked()      /*回退按钮*/
{
    if(cnt==3)
        return;
    game->goBack();
    checkGame();
    setFocus();
}

void MainWindow::onActionClicked()      /*读取自定义地图*/
{
    if(cnt==3)
    {
        cnt=0;
    }
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("选择自定义地图"),
                                                    tr(""),
                                                    tr(".txt(*.txt)")
                                                    );
    loadGame(filename,tr(""));
}

void MainWindow::showHelpInformation()      /*帮助对话框*/
{
    QMessageBox::information(NULL, "帮助",tr(
                                 "移动:   方向键/WASD/鼠标\n"
                                 "重新开始: R\n"
                                 "回退:   P/鼠标右键\n"
                                 "自动寻路(据说这个是神秘暗号)\n"
                                 "寻路上一步:    右方向键/鼠标左键\n"
                                 "寻路下一步:    左方向键/鼠标右键\n"
                                 "停止寻路: Esc\n")
                             , QMessageBox::Yes);
}
void MainWindow::showAboutInformation()      /*关于对话框*/
{
    QMessageBox::information(NULL, "关于",tr(
                             "游戏团队: 219 (输入这个可能会有神秘事件发生)\n"
                             "本游戏始于Qt作业，不能用于商业用途")
                             , QMessageBox::Yes);
}
