#-------------------------------------------------
#
# Project created by QtCreator 2018-06-28T15:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg xml network winextras

VERSION = 2.0.0
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)

DEFINES += NIGHTLY_VERSION=$$system("echo %date:~10,4%%date:~4,2%%date:~7,2%")

TARGET = Ring
TEMPLATE = app

contains(BUILD, Debug) {
    QMAKE_STRIP = echo
    CONFIG += console
}

isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = lrelease
}

SOURCES += main.cpp\
        mainwindow.cpp \
    callwidget.cpp \
    configurationwidget.cpp \
    navwidget.cpp \
    accountdetails.cpp \
    aboutdialog.cpp \
    videowidget.cpp \
    utils.cpp \
    wizarddialog.cpp \
    windowscontactbackend.cpp \
    selectareadialog.cpp \
    accountserializationadapter.cpp \
    instantmessagingwidget.cpp \
    accountstatedelegate.cpp \
    videoview.cpp \
    videooverlay.cpp \
    imdelegate.cpp \
    contactpicker.cpp \
    contactmethodpicker.cpp \
    globalsystemtray.cpp \
    smartlistdelegate.cpp \
    callutilsdialog.cpp \
    combar.cpp \
    idlabel.cpp \
    ringcontactlineedit.cpp \
    pixbufmanipulator.cpp \
    qualitydialog.cpp \
    ringbutton.cpp \
    photoboothdialog.cpp \
    sendcontactrequestwidget.cpp \
    currentaccountwidget.cpp \
    contactrequestwidget.cpp \
    contactrequestitemdelegate.cpp \
    quickactcontactrequestwidget.cpp \
    contactrequestlistview.cpp \
    deleteaccountdialog.cpp \
    bannedcontactswidget.cpp \
    photoboothwidget.cpp \
    deletecontactdialog.cpp \
    smartlistmodel.cpp \
    smartlistview.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navwidget.h \
    accountdetails.h \
    aboutdialog.h \
    videowidget.h \
    utils.h \
    wizarddialog.h \
    windowscontactbackend.h \
    selectareadialog.h \
    accountserializationadapter.h \
    instantmessagingwidget.h \
    accountstatedelegate.h \
    videoview.h \
    videooverlay.h \
    imdelegate.h \
    contactpicker.h \
    contactmethodpicker.h \
    settingskey.h \
    globalsystemtray.h \
    smartlistdelegate.h \
    callutilsdialog.h \
    combar.h \
    idlabel.h \
    ringcontactlineedit.h \
    pixbufmanipulator.h \
    qualitydialog.h \
    ringthemeutils.h \
    ringbutton.h \
    photoboothdialog.h \
    sendcontactrequestwidget.h \
    currentaccountwidget.h \
    contactrequestwidget.h \
    contactrequestitemdelegate.h \
    quickactcontactrequestwidget.h \
    contactrequestlistview.h \
    deleteaccountdialog.h \
    bannedcontactswidget.h \
    photoboothwidget.h \
    deletecontactdialog.h \
    lrcinstance.h \
    smartlistmodel.h \
    smartlistview.h

contains(DEFINES, URI_PROTOCOL) {
 HEADERS += shmclient.h
 SOURCES += shmclient.cpp
}

FORMS    +=
    mainwindow.ui \
    callwidget.ui \
    configurationwidget.ui \
    accountdetails.ui \
    aboutdialog.ui \
    wizarddialog.ui \
    instantmessagingwidget.ui \
    videoview.ui \
    videooverlay.ui \
    contactpicker.ui \
    contactmethodpicker.ui \
    callutilsdialog.ui \
    combar.ui \
    qualitydialog.ui \
    ringbutton.ui \
    photoboothdialog.ui \
    sendcontactrequestwidget.ui \
    currentaccountwidget.ui \
    contactrequestwidget.ui \
    quickactcontactrequestwidget.ui \
    deleteaccountdialog.ui \
    bannedcontactswidget.ui \
    photoboothwidget.ui \
    deletecontactdialog.ui

INCLUDEPATH += $${RING}/lrc/src
INCLUDEPATH += $${RING}/client-windows/winsparkle/include
INCLUDEPATH += $${RING}/client-windows/qrencode-win32/qrencode-win32


CONFIG( debug, debug|release ) {
    # debug
    # daemon lib
    LIBS += -L$$PWD/../daemon/contrib/msvc/lib/x64/
    LIBS += -L$$PWD/../daemon/MSVC/x64/DebugLib_win32/bin/ -ldring

    # lrc lib
    LIBS += -L$$PWD/../lrc/msvc/Debug/ -lringclient_s

    # winsparkle lib
    LIBS += -L$$PWD/winsparkle/x64/release/ -lWinSparkle

    # qrcode lib
    LIBS += -L$$PWD/qrencode-win32/qrencode-win32/vc8/qrcodelib/x64/Release-Lib/ -lqrcodelib
} else {
    # release
    # debug
    # daemon lib
    LIBS += -L$$PWD/../daemon/contrib/msvc/lib/x64/
    LIBS += -L$$PWD/../daemon/MSVC/x64/ReleaseLib_win32/bin/ -ldring

    # lrc lib
    LIBS += -L$$PWD/../lrc/msvc/Release/ -lringclient_s

    # winsparkle lib
    LIBS += -L$$PWD/winsparkle/x64/release/ -lWinSparkle

    # qrcode lib
    LIBS += -L$$PWD/qrencode-win32/qrencode-win32/vc8/qrcodelib/x64/Release-Lib/ -lqrcodelib

    QMAKE_LFLAGS_RELEASE += /FORCE:MULTIPLE
}

LIBS += -lOle32 -lAdvapi32 -lShlwapi -lUser32 -lGdi32 -lCrypt32 -lStrmiids

RESOURCES += \
    ressources.qrc

RC_FILE = ico.rc

TRANSLATIONS = \
    translations/ring_client_windows_nb.ts \
    translations/ring_client_windows_pa.ts \
    translations/ring_client_windows_pt_BR.ts \
    translations/ring_client_windows_pt.ts \
    translations/ring_client_windows_ms.ts \
    translations/ring_client_windows_de.ts \
    translations/ring_client_windows_uk.ts \
    translations/ring_client_windows_sq_AL.ts \
    translations/ring_client_windows_ca.ts \
    translations/ring_client_windows_es.ts \
    translations/ring_client_windows_da_DK.ts \
    translations/ring_client_windows_et_EE.ts \
    translations/ring_client_windows_de_DE.ts \
    translations/ring_client_windows_lt.ts \
    translations/ring_client_windows_fr_FR.ts \
    translations/ring_client_windows_nl_BE.ts \
    translations/ring_client_windows_he.ts \
    translations/ring_client_windows_sk_SK.ts \
    translations/ring_client_windows_pl.ts \
    translations/ring_client_windows_es_AR.ts \
    translations/ring_client_windows_nl.ts \
    translations/ring_client_windows_it_IT.ts \
    translations/ring_client_windows_bg.ts \
    translations/ring_client_windows_id.ts \
    translations/ring_client_windows_en_GB.ts \
    translations/ring_client_windows_pl_PL.ts \
    translations/ring_client_windows.ts \
    translations/ring_client_windows_eu.ts \
    translations/ring_client_windows_eo.ts \
    translations/ring_client_windows_nl_NL.ts \
    translations/ring_client_windows_ru_RU.ts \
    translations/ring_client_windows_hr.ts \
    translations/ring_client_windows_zh_CN.ts \
    translations/ring_client_windows_fr.ts \
    translations/ring_client_windows_tr.ts \
    translations/ring_client_windows_cs_CZ.ts \
    translations/ring_client_windows_zh_TW.ts \
    translations/ring_client_windows_fr_CA.ts \
    translations/ring_client_windows_ko_KR.ts \
    translations/ring_client_windows_zh.ts \
    translations/ring_client_windows_fa_IR.ts \
    translations/ring_client_windows_fi.ts \
    translations/ring_client_windows_sv.ts \
    translations/ring_client_windows_it.ts \
    translations/ring_client_windows_el.ts \
    translations/ring_client_windows_ja.ts \
    translations/ring_client_windows_hu.ts \
    translations/ring_client_windows_sl.ts \
    translations/ring_client_windows_hi_IN.ts \
    translations/ring_client_windows_ro.ts \
    translations/ring_client_windows_ru.ts \
    translations/ring_client_windows_ar.ts \

RINGTONES.files = $${RING}/daemon/ringtones
RINGTONES.path = $$OUT_PWD/release

PACKAGING.files = ring.nsi images/ring.ico
PACKAGING.path = $$OUT_PWD/release

LICENSE.files = License.rtf
LICENSE.path = $$OUT_PWD/release

RUNTIMEDIR=$$[QT_INSTALL_BINS]

RUNTIME.files = $${RING}/lrc/msvc/Release/ringclient.dll
RUNTIME.path = $$OUT_PWD/release

LRC_TRANSLATION.files = $${RING}/share/libringclient/translations
LRC_TRANSLATION.path = $$OUT_PWD/release/share/libringclient/

QTRUNTIME.files = $$RUNTIMEDIR/Qt5Core.dll $$RUNTIMEDIR/Qt5Widgets.dll \
                        $$RUNTIMEDIR/Qt5Gui.dll $$RUNTIMEDIR/Qt5Svg.dll \
                        $$RUNTIMEDIR/Qt5Xml.dll $$RUNTIMEDIR/Qt5WinExtras.dll \
                        $$RUNTIMEDIR/Qt5Network.dll $$RUNTIMEDIR/Qt5Sql.dll
QTRUNTIME.path = $$OUT_PWD/release

QTDEPSRUNTIME.path = $$OUT_PWD/release

QTPLATFORMS.files = $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
QTPLATFORMS.path = $$OUT_PWD/release/platforms

QTPLUGINIMAGE.files = $$[QT_INSTALL_PLUGINS]/imageformats/
QTPLUGINIMAGE.path = $$OUT_PWD/release

QTSQLDRIVERS.files = $$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite.dll
QTSQLDRIVERS.path = $$OUT_PWD/release/sqldrivers

LIBSTD.files = $$RUNTIMEDIR/libgcc_s_sjlj-1.dll $$RUNTIMEDIR/libstdc++-6.dll \
                $$RUNTIMEDIR/libwinpthread-1.dll $$RUNTIMEDIR/libgcc_s_seh-1.dll
LIBSTD.path = $$OUT_PWD/release

INSTALLS += RINGTONES PACKAGING LICENSE RUNTIME LRC_TRANSLATION QTRUNTIME QTDEPSRUNTIME \
            QTPLUGINIMAGE QTPLATFORMS QTSQLDRIVERS LIBSTD

DEFINES += ENABLE_AUTOUPDATE
