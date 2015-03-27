#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RING_CLIENT_WINDOWS_VERSION = 0.2.1

TARGET = RingClientWindows
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
    navwidget.cpp \
    accountdetails.cpp \
    videowidget.cpp \
    minimalhistorybackend.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navstack.h \
    navbar.h \
    mainbar.h \
    navwidget.h \
    accountdetails.h \
    videowidget.h \
    minimalhistorybackend.h

FORMS    += mainwindow.ui \
    callwidget.ui \
    configurationwidget.ui \
    navbar.ui \
    mainbar.ui \
    accountdetails.ui

INCLUDEPATH += /home/edric/Documents/CrossWorkspace/ring/binArch/include/libringclient

win32: LIBS += -L/home/edric/Documents/CrossWorkspace/ring/binArch/lib/ -lringclient

RESOURCES += \
    ressources.qrc
