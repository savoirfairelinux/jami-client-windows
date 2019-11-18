win32-msvc {
    TEMPLATE = vcapp
    TARGET = Jami

    QT += winextras quick quickwidgets widgets xml multimedia multimediawidgets network webenginewidgets svg sql

    CONFIG += suppress_vcproj_warnings

    #CONFIG += Beta

    # compiler options
    QMAKE_CXXFLAGS += /wd"4068" /wd"4099" /wd"4189" /wd"4267" /wd"4577" /wd"4467" /wd"4715" /wd"4828"
    QMAKE_CXXFLAGS += /MP /GS /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /Zc:inline /fp:precise /errorReport:prompt /WX- /Zc:forScope
    QMAKE_CXXFLAGS += /Gd /Oi /MD /std:c++17 /FC /EHsc /nologo /sdl

    # linker options
    QMAKE_LFLAGS+= /ignore:4006,4049,4078,4098 /FORCE:MULTIPLE /INCREMENTAL:NO /Debug /LTCG /NODEFAULTLIB:LIBCMT

    # preprocessor defines
    DEFINES += UNICODE PROCESS_DPI_AWARE=1 QT_NO_DEBUG NDEBUG

   # Beta { DEFINES += BETA }

    # dependencies
    LRC=../lrc
    DRING=../daemon
    QRENCODE=../client-windows/qrencode-win32/qrencode-win32

    # client deps
    INCLUDEPATH += $${QRENCODE}
    LIBS += $${QRENCODE}/vc8/qrcodelib/x64/Release-Lib/qrcodelib.lib

    # lrc
    INCLUDEPATH += $${LRC}/src/
    LIBS += $${LRC}/msvc/release/ringclient_static.lib
    LIBS += $${LRC}/msvc/src/qtwrapper/Release/qtwrapper.lib

    # daemon
    INCLUDEPATH += ../daemon/contrib/msvc/include/
    LIBS += $${DRING}/MSVC/x64/ReleaseLib_win32/bin/dring.lib
    LIBS += $${DRING}/contrib/msvc/lib/x64/libgnutls.lib

    # windows system libs
    LIBS += Shell32.lib Ole32.lib Advapi32.lib Shlwapi.lib User32.lib Gdi32.lib Crypt32.lib Strmiids.lib

    # output paths
    DESTDIR = x64/Release
    OBJECTS_DIR = obj/.obj
    MOC_DIR = obj/.moc
    RCC_DIR = obj/.rcc
    UI_DIR = obj/.ui
}

unix {
    TARGET = jami-qt
    TEMPLATE = app

    QT += quick quickwidgets widgets xml multimedia multimediawidgets network webenginewidgets svg

    CONFIG += c++17

    isEmpty(LRC) { LRC=../../install/lrc/ }

    INCLUDEPATH += $${LRC}/include/libringclient
    INCLUDEPATH += $${LRC}/include

    LIBS += -L$${LRC}/lib -lringclient
    LIBS += -lqrencode
}



# Input
HEADERS += ./aboutdialog.h \
        ./banneditemwidget.h \
        ./conversationsfilterwidget.h \
        ./idlabel.h \
        ./messagewebview.h \
        ./ringbutton.h \
        ./smartlistmodel.h \
        ./utils.h \
        ./accountitemdelegate.h \
        ./bannedlistmodel.h \
        ./currentaccountcombobox.h \
        ./invitebuttonswidget.h \
        ./nameregistrationdialog.h \
        ./ringcontactlineedit.h \
        ./smartlistselectorbuttonnotifier.h \
        ./version.h \
        ./accountlistmodel.h \
        ./bezierconnectorwidget.h \
        ./deleteaccountdialog.h \
        ./levelmeter.h \
        ./navwidget.h \
        ./ringthemeutils.h \
        ./smartlistview.h \
        ./videooverlay.h \
        ./advancedsettingswidget.h \
        ./callwidget.h \
        ./deviceitemwidget.h \
        ./linkdevicedialog.h \
        ./newwizardwidget.h \
        ./runguard.h \
        ./splashscreen.h \
        ./videoview.h \
        ./advancedsipsettingwidget.h \
        ./contactpicker.h \
        ./lrcinstance.h \
        ./passworddialog.h \
        ./selectareadialog.h \
        ./toggleswitch.h \
        ./animationhelpers.h \
        ./contactpickeritemdelegate.h \
        ./globalsystemtray.h \
        ./mainwindow.h \
        ./photoboothwidget.h \
        ./settingskey.h \
        ./updateconfirmdialog.h \
        ./webchathelpers.h \
        ./avatargraphicsview.h \
        ./conversationitemdelegate.h \
        ./iconbutton.h \
        ./messagewebpage.h \
        ./pixbufmanipulator.h \
        ./settingswidget.h \
        ./updatedownloaddialog.h \
        ./sipinputpanel.h \
        ./callaudioonlyavataroverlay.h \
        ./overlaybutton.h \
        ./accountmigrationdialog.h \
        ./previewwidget.h \
        ./rendermanager.h \
        ./distantwidget.h \
        ./videowidgetbase.h \
        ./section.h \
        ./collapsiblepasswordwidget.h \
        ./notifypushbutton.h \
        ./popupdialog.h \
        ./recordoverlay.h \
        ./widgethelpers.h \
        ./recordwidget.h \
        ./networkmanager.h \
        ./connectivitymonitor.h
SOURCES += ./aboutdialog.cpp \
        ./banneditemwidget.cpp \
        ./conversationsfilterwidget.cpp \
        ./idlabel.cpp \
        ./messagewebview.cpp \
        ./ringbutton.cpp \
        ./smartlistview.cpp \
        ./videoview.cpp \
        ./accountitemdelegate.cpp \
        ./bannedlistmodel.cpp \
        ./currentaccountcombobox.cpp \
        ./invitebuttonswidget.cpp \
        ./nameregistrationdialog.cpp \
        ./ringcontactlineedit.cpp \
        ./splashscreen.cpp \
        ./animationhelpers.cpp \
        ./accountlistmodel.cpp \
        ./bezierconnectorwidget.cpp \
        ./deleteaccountdialog.cpp \
        ./levelmeter.cpp \
        ./navwidget.cpp \
        ./runguard.cpp \
        ./toggleswitch.cpp \
        ./webchathelpers.cpp \
        ./advancedsettingswidget.cpp \
        ./callwidget.cpp \
        ./deviceitemwidget.cpp \
        ./linkdevicedialog.cpp \
        ./newwizardwidget.cpp \
        ./selectareadialog.cpp \
        ./updateconfirmdialog.cpp \
        ./advancedsipsettingwidget.cpp \
        ./contactpicker.cpp \
        ./main.cpp \
        ./passworddialog.cpp \
        ./settingswidget.cpp \
        ./updatedownloaddialog.cpp \
        ./animationhelpers.cpp \
        ./contactpickeritemdelegate.cpp \
        ./globalsystemtray.cpp \
        ./mainwindow.cpp \
        ./photoboothwidget.cpp \
        ./smartlistmodel.cpp \
        ./utils.cpp \
        ./avatargraphicsview.cpp \
        ./conversationitemdelegate.cpp \
        ./iconbutton.cpp \
        ./messagewebpage.cpp \
        ./pixbufmanipulator.cpp \
        ./smartlistselectorbuttonnotifier.cpp \
        ./videooverlay.cpp \
        ./sipinputpanel.cpp \
        ./callaudioonlyavataroverlay.cpp \
        ./overlaybutton.cpp \
        ./accountmigrationdialog.cpp \
        ./previewwidget.cpp \
        ./rendermanager.cpp \
        ./distantwidget.cpp \
        ./videowidgetbase.cpp \
        ./section.cpp \
        ./collapsiblepasswordwidget.cpp \
        ./notifypushbutton.cpp \
        ./popupdialog.cpp \
        ./recordoverlay.cpp \
        ./widgethelpers.cpp \
        ./recordwidget.cpp \
        ./networkmanager.cpp \
        ./connectivitymonitor.cpp
FORMS += ./aboutdialog.ui \
        ./advancedsipsettingwidget.ui \
        ./callwidget.ui \
        ./deviceitemwidget.ui \
        ./mainwindow.ui \
        ./passworddialog.ui \
        ./ringbutton.ui \
        ./updatedownloaddialog.ui \
        ./animatedoverlay.ui \
        ./contactpicker.ui \
        ./invitebuttonswidget.ui \
        ./nameregistrationdialog.ui \
        ./settingswidget.ui \
        ./videooverlay.ui \
        ./advancedsettingswidget.ui \
        ./banneditemwidget.ui \
        ./deleteaccountdialog.ui \
        ./linkdevicedialog.ui \
        ./newwizardwidget.ui \
        ./photoboothwidget.ui \
        ./updateconfirmdialog.ui \
        ./videoview.ui \
        ./sipkeypad.ui \
        ./callaudioonlyavataroverlay.ui \
        ./accountmigration.ui \
        ./section.ui \
        ./collapsiblepasswordwidget.ui \
        ./popupdialog.ui \
        ./recordoverlay.ui \
        ./recordwidget.ui \

RESOURCES += ressources.qrc
