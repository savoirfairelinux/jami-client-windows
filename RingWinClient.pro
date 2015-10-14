#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg winextras xml

VERSION = 0.3.0
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)

DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

BUILD=$${BUILD}
TARGET = Ring
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

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
    navstack.cpp \
    navbar.cpp \
    mainbar.cpp \
    navwidget.cpp \
    accountdetails.cpp \
    aboutdialog.cpp \
    videowidget.cpp \
    utils.cpp \
    wizarddialog.cpp \
    windowscontactbackend.cpp \
    historydelegate.cpp \
    contactdelegate.cpp \
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
    transferdialog.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navstack.h \
    navbar.h \
    mainbar.h \
    navwidget.h \
    accountdetails.h \
    aboutdialog.h \
    videowidget.h \
    utils.h \
    wizarddialog.h \
    windowscontactbackend.h \
    historydelegate.h \
    contactdelegate.h \
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
    transferdialog.h

FORMS    += mainwindow.ui \
    callwidget.ui \
    configurationwidget.ui \
    navbar.ui \
    mainbar.ui \
    accountdetails.ui \
    aboutdialog.ui \
    wizarddialog.ui \
    instantmessagingwidget.ui \
    videoview.ui \
    videooverlay.ui \
    contactpicker.ui \
    contactmethodpicker.ui \
    transferdialog.ui

win32: LIBS += -lole32 -luuid -lshlwapi

INCLUDEPATH += $${RING}/include/libringclient

LIBS += -L$${RING}/lib -lringclient

RESOURCES += \
    ressources.qrc

RC_FILE = ico.rc

TRANSLATIONS = translations/ring_client_windows.ts \
               translations/ring_client_windows_fr.ts \

maketranslationdir.target = customtarget
maketranslationdir.commands = $(MKDIR) $$OUT_PWD/share/ring/translations/

updateqm.input = TRANSLATIONS
updateqm.output = $$OUT_PWD/release/share/ring/translations/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$OUT_PWD/release/share/ring/translations/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link

QMAKE_EXTRA_TARGETS += maketranslationdir
QMAKE_EXTRA_COMPILERS += updateqm

PRE_TARGETDEPS += customtarget compiler_updateqm_make_all

QM_FILES.files = share
QM_FILES.path = $$OUT_PWD/release

INSTALLS += QM_FILES

DISTFILES += \
    License.rtf \
    ringtones/konga.ul \
    ring.nsi

win32 {

    RINGTONES.files = ringtones
    RINGTONES.path = $$OUT_PWD/release

    PACKAGING.files = ring.nsi images/ring.ico
    PACKAGING.path = $$OUT_PWD/release

    LICENSE.files = License.rtf
    LICENSE.path = $$OUT_PWD/release

    RUNTIMEDIR=/usr/i686-w64-mingw32/bin/

    RUNTIME.files = $${RING}/bin/libring.dll $${RING}/bin/libringclient.dll
    RUNTIME.path = $$OUT_PWD/release

    LRC_TRANSLATION.files = $${RING}/share/libringclient/translations
    LRC_TRANSLATION.path = $$OUT_PWD/release/share/libringclient/

    QTRUNTIME.files = $$RUNTIMEDIR/Qt5Core.dll $$RUNTIMEDIR/Qt5Widgets.dll \
                            $$RUNTIMEDIR/Qt5Gui.dll $$RUNTIMEDIR/Qt5Svg.dll \
                            $$RUNTIMEDIR/Qt5Xml.dll $$RUNTIMEDIR/Qt5WinExtras.dll
    QTRUNTIME.path = $$OUT_PWD/release

    QTDEPSRUNTIME.files = $$RUNTIMEDIR/zlib1.dll \
                            $$RUNTIMEDIR/libfreetype-6.dll $$RUNTIMEDIR/libglib-2.0-0.dll \
                            $$RUNTIMEDIR/libharfbuzz-0.dll \
                            $$RUNTIMEDIR/libintl-8.dll $$RUNTIMEDIR/libpcre-1.dll \
                            $$RUNTIMEDIR/libpcre16-0.dll $$RUNTIMEDIR/libpng16-16.dll \
                            $$RUNTIMEDIR/libjpeg-8.dll $$RUNTIMEDIR/libiconv-2.dll \
                            $$RUNTIMEDIR/libidn-11.dll
    QTDEPSRUNTIME.path = $$OUT_PWD/release

    QTPLATFORMS.files = $$(QTDIR)/plugins/platforms/qwindows.dll
    QTPLATFORMS.path = $$OUT_PWD/release/platforms

    QTPLUGINIMAGE.files = $$(QTDIR)/plugins/imageformats/
    QTPLUGINIMAGE.path = $$OUT_PWD/release

    LIBSTD.files = $$RUNTIMEDIR/libgcc_s_sjlj-1.dll $$RUNTIMEDIR/libstdc++-6.dll \
                    $$RUNTIMEDIR/libwinpthread-1.dll
    LIBSTD.path = $$OUT_PWD/release

    INSTALLS += RINGTONES PACKAGING LICENSE RUNTIME LRC_TRANSLATION QTRUNTIME QTDEPSRUNTIME \
                QTPLUGINIMAGE QTPLATFORMS LIBSTD
}
