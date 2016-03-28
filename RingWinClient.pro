#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg xml

win32: QT += winextras

VERSION = 1.0.0
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)

NIGHTLY_VERSION =$$system(date +'%Y%m%d')

DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
DEFINES += NIGHTLY_VERSION=\\\"$$NIGHTLY_VERSION\\\"

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
    smartlistdelegate.cpp \
    callutilsdialog.cpp \
    combar.cpp \
    idlabel.cpp \
    smartlist.cpp \
    ringcontactlineedit.cpp \
    pixbufmanipulator.cpp \
    qualitydialog.cpp \
    ringbutton.cpp \
    photoboothdialog.cpp

HEADERS  += mainwindow.h \
    callwidget.h \
    configurationwidget.h \
    navstack.h \
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
    smartlistdelegate.h \
    callutilsdialog.h \
    combar.h \
    idlabel.h \
    smartlist.h \
    ringcontactlineedit.h \
    pixbufmanipulator.h \
    qualitydialog.h \
    ringthemeutils.h \
    ringbutton.h \
    photoboothdialog.h

contains(DEFINES, URI_PROTOCOL) {
 HEADERS += shmclient.h
 SOURCES += shmclient.cpp
}

FORMS    += mainwindow.ui \
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
    photoboothdialog.ui

win32: LIBS += -lole32 -luuid -lshlwapi

INCLUDEPATH += $${RING}/include/libringclient
INCLUDEPATH += $${RING}/include

LIBS += -L$${RING}/lib -lringclient

RESOURCES += \
    ressources.qrc

RC_FILE = ico.rc

TRANSLATIONS = translations/ring_client_windows.ts \
               translations/ring_client_windows_fr.ts \
               translations/ring_client_windows_ar.ts \
               translations/ring_client_windows_de.ts \
               translations/ring_client_windows_lt.ts \
               translations/ring_client_windows_ru.ts \
               translations/ring_client_windows_uk.ts \
               translations/ring_client_windows_cs_CZ.ts \
               translations/ring_client_windows_eo.ts \
               translations/ring_client_windows_es.ts \
               translations/ring_client_windows_he.ts \
               translations/ring_client_windows_pt_BR.ts \
               translations/ring_client_windows_pt.ts \
               translations/ring_client_windows_ru_RU.ts \
               translations/ring_client_windows_tr.ts

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
    ring.nsi

win32 {

    RINGTONES.files = $${RING}/share/ring/ringtones
    RINGTONES.path = $$OUT_PWD/release

    PACKAGING.files = ring.nsi images/ring.ico
    PACKAGING.path = $$OUT_PWD/release

    LICENSE.files = License.rtf
    LICENSE.path = $$OUT_PWD/release

    RUNTIMEDIR=$$[QT_INSTALL_BINS]

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
                            $$RUNTIMEDIR/libidn-11.dll $$RUNTIMEDIR/liblzma-5.dll \
                            $$RUNTIMEDIR/libGLESv2.dll $$RUNTIMEDIR/libbz2-1.dll \
                            $$RUNTIMEDIR/iconv.dll
    QTDEPSRUNTIME.path = $$OUT_PWD/release

    QTPLATFORMS.files = $$(QTDIR)/plugins/platforms/qwindows.dll
    QTPLATFORMS.path = $$OUT_PWD/release/platforms

    QTPLUGINIMAGE.files = $$(QTDIR)/plugins/imageformats/
    QTPLUGINIMAGE.path = $$OUT_PWD/release

    LIBSTD.files = $$RUNTIMEDIR/libgcc_s_sjlj-1.dll $$RUNTIMEDIR/libstdc++-6.dll \
                    $$RUNTIMEDIR/libwinpthread-1.dll $$RUNTIMEDIR/libgcc_s_seh-1.dll
    LIBSTD.path = $$OUT_PWD/release

    INSTALLS += RINGTONES PACKAGING LICENSE RUNTIME LRC_TRANSLATION QTRUNTIME QTDEPSRUNTIME \
                QTPLUGINIMAGE QTPLATFORMS LIBSTD

    ENABLE_AUTOUPDATE=$${ENABLE_AUTOUPDATE}
    equals (ENABLE_AUTOUPDATE, True) {
       DEFINES += ENABLE_AUTOUPDATE
       LIBS += -L$${RING}/lib -lWinSparkle
       WINSPARKLE.files = $${RING}/bin/WinSparkle.dll $${RING}/bin/libringclient.dll
       WINSPARKLE.path = $$OUT_PWD/release
       INSTALLS += WINSPARKLE
       message("AUTO UPDATES enabled")
    }
}
