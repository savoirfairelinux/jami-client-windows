#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RingWinClient
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += console

SOURCES += main.cpp\
        mainwindow.cpp \
    callwidget.cpp \
    configurationwidget.cpp \
    navstack.cpp \
    navbar.cpp \
    mainbar.cpp \
    navwidget.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navstack.h \
    navbar.h \
    mainbar.h \
    navwidget.h

FORMS    += mainwindow.ui \
    callwidget.ui \
    configurationwidget.ui \
    navbar.ui \
    mainbar.ui

INCLUDEPATH += /home/edric/Documents/CrossWorkspace/ring/binArch/include/libringclient

win32: LIBS += -L/home/edric/Documents/CrossWorkspace/ring/binArch/lib/ -lringclient
