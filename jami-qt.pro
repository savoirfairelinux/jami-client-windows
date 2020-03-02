win32-msvc {
    TARGET = Jami
    TEMPLATE = vcapp

    QT += core winextras qml quickcontrols2 quick quickwidgets widgets xml multimedia multimediawidgets network webenginewidgets svg sql

    CONFIG += suppress_vcproj_warnings c++17 qtquickcompiler

    QTQUICK_COMPILER_SKIPPED_RESOURCES += ./ressources.qrc

    # compiler options
    QMAKE_CXXFLAGS += /wd"4068" /wd"4099" /wd"4189" /wd"4267" /wd"4577" /wd"4467" /wd"4715" /wd"4828"
    QMAKE_CXXFLAGS += /MP /GS /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /Zc:inline /fp:precise /errorReport:prompt
    QMAKE_CXXFLAGS += /Gd /Oi /MD /std:c++17 /FC /EHsc /nologo /sdl

    # linker options
    QMAKE_LFLAGS+= /ignore:4006,4049,4078,4098 /FORCE:MULTIPLE /INCREMENTAL:NO /Debug /LTCG /NODEFAULTLIB:LIBCMT

    # preprocessor defines
    DEFINES += UNICODE QT_NO_DEBUG NDEBUG

    # dependencies
    LRC= ../lrc
    DRING= ../daemon
    QRENCODE= qrencode-win32/qrencode-win32

    # client deps
    INCLUDEPATH += $${QRENCODE}
    LIBS += $${QRENCODE}/vc8/qrcodelib/x64/Release-Lib/qrcodelib.lib

    # lrc
    INCLUDEPATH += $${LRC}/src/
    LIBS += $${LRC}/msvc/release/ringclient_static.lib
    LIBS += $${LRC}/msvc/src/qtwrapper/Release/qtwrapper.lib

    # daemon
    INCLUDEPATH += ../daemon/contrib/msvc/include/
    LIBS += $${DRING}/build-local/x64/ReleaseLib_win32/bin/dring.lib
    LIBS += $${DRING}/contrib/msvc/lib/x64/libgnutls.lib

    # windows system libs
    LIBS += Shell32.lib Ole32.lib Advapi32.lib Shlwapi.lib User32.lib Gdi32.lib Crypt32.lib Strmiids.lib

    # output paths
    OBJECTS_DIR = obj/.obj
    MOC_DIR = obj/.moc
    RCC_DIR = obj/.rcc
    UI_DIR = obj/.ui

    # ReleaseCompile config
    contains(CONFIG, ReleaseCompile) {
        CONFIG(ReleaseCompile) {
            message(ReleaseCompile config enabled)
            Release: DEFINES += COMPILE_ONLY
        }
    }

    # beta config
    contains(CONFIG, Beta) {
        CONFIG(Beta) {
            message(Beta config enabled)
            Release: DESTDIR = x64/Beta
            Release: DEFINES += BETA
        }
    } else {
        Release: DESTDIR = x64/Release
    }
    Debug: DESTDIR = x64/Debug

    # qt dir
    QMAKE_INCDIR_QT=$(QTDIR)\include
    QMAKE_LIBDIR=$(QTDIR)\lib
    QMAKE_MOC=$(QTDIR)\bin\moc.exe
    QMAKE_QMAKE=$(QTDIR)\bin\qmake.exe

    # exe icons
    Release: RC_FILE = ico.rc
}

unix {
    TARGET = jami-qt
    TEMPLATE = app

    QT += quick quickwidgets widgets xml multimedia multimediawidgets network webenginewidgets svg quickcontrols2

    #check Qt version
    QT_VERSION = $$[QT_VERSION]
    QT_VERSION = $$split(QT_VERSION, ".")
    QT_VER_MAJ = $$member(QT_VERSION, 0)
    QT_VER_MIN = $$member(QT_VERSION, 1)

    lessThan(QT_VER_MIN, 12) {
        QMAKE_CXXFLAGS += -std=c++17
    }
    greaterThan(QT_VER_MIN, 12) | equals(QT_VER_MIN, 12) {
        CONFIG += c++17
    }

    isEmpty(LRC) { LRC=../../install/lrc/ }

    INCLUDEPATH += $${LRC}/include/libringclient
    INCLUDEPATH += $${LRC}/include
    INCLUDEPATH += ../src

    LIBS += -L$${LRC}/lib -lringclient
    LIBS += -lqrencode
}



# Input
HEADERS += ./src/aboutdialog.h \
        ./src/banneditemwidget.h \
        ./src/conversationsfilterwidget.h \
        ./src/idlabel.h \
        ./src/messagewebview.h \
        ./src/ringbutton.h \
        ./src/smartlistmodel.h \
        ./src/utils.h \
        ./src/accountitemdelegate.h \
        ./src/bannedlistmodel.h \
        ./src/currentaccountcombobox.h \
        ./src/invitebuttonswidget.h \
        ./src/nameregistrationdialog.h \
        ./src/ringcontactlineedit.h \
        ./src/smartlistselectorbuttonnotifier.h \
        ./src/version.h \
        ./src/accountlistmodel.h \
        ./src/bezierconnectorwidget.h \
        ./src/deleteaccountdialog.h \
        ./src/levelmeter.h \
        ./src/navwidget.h \
        ./src/ringthemeutils.h \
        ./src/smartlistview.h \
        ./src/videooverlay.h \
        ./src/advancedsettingswidget.h \
        ./src/callwidget.h \
        ./src/deviceitemwidget.h \
        ./src/linkdevicedialog.h \
        ./src/newwizardwidget.h \
        ./src/runguard.h \
        ./src/splashscreen.h \
        ./src/videoview.h \
        ./src/advancedsipsettingwidget.h \
        ./src/contactpicker.h \
        ./src/lrcinstance.h \
        ./src/passworddialog.h \
        ./src/selectareadialog.h \
        ./src/toggleswitch.h \
        ./src/animationhelpers.h \
        ./src/contactpickeritemdelegate.h \
        ./src/globalsystemtray.h \
        ./src/mainwindow.h \
        ./src/photoboothwidget.h \
        ./src/settingskey.h \
        ./src/updateconfirmdialog.h \
        ./src/webchathelpers.h \
        ./src/conversationitemdelegate.h \
        ./src/iconbutton.h \
        ./src/messagewebpage.h \
        ./src/pixbufmanipulator.h \
        ./src/settingswidget.h \
        ./src/updatedownloaddialog.h \
        ./src/sipinputpanel.h \
        ./src/callaudioonlyavataroverlay.h \
        ./src/overlaybutton.h \
        ./src/accountmigrationdialog.h \
        ./src/previewwidget.h \
        ./src/rendermanager.h \
        ./src/distantwidget.h \
        ./src/videowidgetbase.h \
        ./src/section.h \
        ./src/collapsiblepasswordwidget.h \
        ./src/notifypushbutton.h \
        ./src/popupdialog.h \
        ./src/recordoverlay.h \
        ./src/widgethelpers.h \
        ./src/recordwidget.h \
        ./src/networkmanager.h \
        ./src/connectivitymonitor.h \
        ./src/userprofile.h \
        ./src/mainapplication.h \
        ./src/accountimageprovider.h
        ./src/lrcinterface.h
SOURCES += ./src/aboutdialog.cpp \
        ./src/banneditemwidget.cpp \
        ./src/conversationsfilterwidget.cpp \
        ./src/idlabel.cpp \
        ./src/messagewebview.cpp \
        ./src/ringbutton.cpp \
        ./src/smartlistview.cpp \
        ./src/videoview.cpp \
        ./src/accountitemdelegate.cpp \
        ./src/bannedlistmodel.cpp \
        ./src/currentaccountcombobox.cpp \
        ./src/invitebuttonswidget.cpp \
        ./src/nameregistrationdialog.cpp \
        ./src/ringcontactlineedit.cpp \
        ./src/splashscreen.cpp \
        ./src/accountlistmodel.cpp \
        ./src/bezierconnectorwidget.cpp \
        ./src/deleteaccountdialog.cpp \
        ./src/levelmeter.cpp \
        ./src/navwidget.cpp \
        ./src/runguard.cpp \
        ./src/toggleswitch.cpp \
        ./src/webchathelpers.cpp \
        ./src/advancedsettingswidget.cpp \
        ./src/callwidget.cpp \
        ./src/deviceitemwidget.cpp \
        ./src/linkdevicedialog.cpp \
        ./src/newwizardwidget.cpp \
        ./src/selectareadialog.cpp \
        ./src/updateconfirmdialog.cpp \
        ./src/advancedsipsettingwidget.cpp \
        ./src/contactpicker.cpp \
        ./src/main.cpp \
        ./src/passworddialog.cpp \
        ./src/settingswidget.cpp \
        ./src/updatedownloaddialog.cpp \
        ./src/animationhelpers.cpp \
        ./src/contactpickeritemdelegate.cpp \
        ./src/globalsystemtray.cpp \
        ./src/mainwindow.cpp \
        ./src/photoboothwidget.cpp \
        ./src/smartlistmodel.cpp \
        ./src/utils.cpp \
        ./src/conversationitemdelegate.cpp \
        ./src/iconbutton.cpp \
        ./src/messagewebpage.cpp \
        ./src/pixbufmanipulator.cpp \
        ./src/smartlistselectorbuttonnotifier.cpp \
        ./src/videooverlay.cpp \
        ./src/sipinputpanel.cpp \
        ./src/callaudioonlyavataroverlay.cpp \
        ./src/overlaybutton.cpp \
        ./src/accountmigrationdialog.cpp \
        ./src/previewwidget.cpp \
        ./src/rendermanager.cpp \
        ./src/distantwidget.cpp \
        ./src/videowidgetbase.cpp \
        ./src/section.cpp \
        ./src/collapsiblepasswordwidget.cpp \
        ./src/notifypushbutton.cpp \
        ./src/popupdialog.cpp \
        ./src/recordoverlay.cpp \
        ./src/widgethelpers.cpp \
        ./src/recordwidget.cpp \
        ./src/networkmanager.cpp \
        ./src/connectivitymonitor.cpp \
        ./src/userprofile.cpp \
        ./src/mainapplication.cpp \
        ./src/lrcinterface.cpp
FORMS += ./src/aboutdialog.ui \
        ./src/advancedsipsettingwidget.ui \
        ./src/callwidget.ui \
        ./src/deviceitemwidget.ui \
        ./src/mainwindow.ui \
        ./src/passworddialog.ui \
        ./src/ringbutton.ui \
        ./src/updatedownloaddialog.ui \
        ./src/contactpicker.ui \
        ./src/invitebuttonswidget.ui \
        ./src/nameregistrationdialog.ui \
        ./src/settingswidget.ui \
        ./src/videooverlay.ui \
        ./src/advancedsettingswidget.ui \
        ./src/banneditemwidget.ui \
        ./src/deleteaccountdialog.ui \
        ./src/linkdevicedialog.ui \
        ./src/newwizardwidget.ui \
        ./src/photoboothwidget.ui \
        ./src/updateconfirmdialog.ui \
        ./src/videoview.ui \
        ./src/sipkeypad.ui \
        ./src/callaudioonlyavataroverlay.ui \
        ./src/accountmigration.ui \
        ./src/section.ui \
        ./src/collapsiblepasswordwidget.ui \
        ./src/popupdialog.ui \
        ./src/recordoverlay.ui \
        ./src/recordwidget.ui
RESOURCES += ./ressources.qrc \
             ./qml.qrc
