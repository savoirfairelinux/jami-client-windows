#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg

VERSION = 0.1.0

DEFINES += VERSION=\\\"$$VERSION\\\"

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
    minimalhistorybackend.cpp \
    aboutdialog.cpp \
    pivotviewwidget.cpp \
    pivotcontrol.cpp \
    videowidget.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navstack.h \
    navbar.h \
    mainbar.h \
    navwidget.h \
    accountdetails.h \
    minimalhistorybackend.h \
    aboutdialog.h \
    pivotviewwidget.h \
    pivotcontrol.h \
    videowidget.h

FORMS    += mainwindow.ui \
    callwidget.ui \
    configurationwidget.ui \
    navbar.ui \
    mainbar.ui \
    accountdetails.ui \
    aboutdialog.ui \
    pivotviewwidget.ui

INCLUDEPATH += /home/edric/Documents/CrossWorkspace/ring/binArch/include/libringclient

win32: LIBS += -L/home/edric/Documents/CrossWorkspace/ring/binArch/lib/ -lringclient
linux: LIBS += -L/usr/local/lib/ -lringclient

RESOURCES += \
    ressources.qrc

RC_FILE = ico.rc

DISTFILES += \
    ring.wxs \
    License.rtf
