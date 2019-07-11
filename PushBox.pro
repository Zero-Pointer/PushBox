#-------------------------------------------------
#
# Project created by QtCreator 2017-01-03T16:16:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    Game.cpp

HEADERS  += mainwindow.h \
    Game.h \
    Edge.h \
    KM.h \
    Point.h \
    Status.h

RESOURCES += \
    sourse/sourse.qrc

RC_ICONS = sourse/icon/cirno.ico
QT   +=   multimedia
