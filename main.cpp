#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap Pixmap(":/image/showfor.png");
    QSplashScreen splash(Pixmap);
     splash.show();

//   for(long index=0;index<=10000000;index++);
    MainWindow w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
