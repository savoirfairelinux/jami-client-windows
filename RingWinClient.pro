#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T14:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg xml network

win32: QT += winextras

VERSION = 2.0.0
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)

RING_CLIENT_BUILD_DATE = $$(RING_CLIENT_BUILD_DATE)
isEmpty(RING_CLIENT_BUILD_DATE){
RING_CLIENT_BUILD_DATE = $$system(date +%s)
}
NIGHTLY_VERSION =$$system(date +'%Y%m%d')

DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
DEFINES += NIGHTLY_VERSION=\\\"$$NIGHTLY_VERSION\\\"
DEFINES += RING_CLIENT_BUILD_DATE=\"\\\"$$RING_CLIENT_BUILD_DATE\\\"\"

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
    pathpassworddialog.cpp \
    photoboothdialog.cpp \
    sendcontactrequestwidget.cpp \
    currentaccountwidget.cpp \
    contactrequestwidget.cpp \
    contactrequestitemdelegate.cpp \
    quickactcontactrequestwidget.cpp \
    contactrequestlistwidget.cpp \
    deleteaccountdialog.cpp \
    bannedcontactswidget.cpp \
    photoboothwidget.cpp \
    deletecontactdialog.cpp

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
    pathpassworddialog.h \
    photoboothdialog.h \
    sendcontactrequestwidget.h \
    currentaccountwidget.h \
    contactrequestwidget.h \
    contactrequestitemdelegate.h \
    quickactcontactrequestwidget.h \
    contactrequestlistwidget.h \
    deleteaccountdialog.h \
    bannedcontactswidget.h \
    photoboothwidget.h \
    deletecontactdialog.h

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
    pathpassworddialog.ui \
    photoboothdialog.ui \
    sendcontactrequestwidget.ui \
    currentaccountwidget.ui \
    contactrequestwidget.ui \
    quickactcontactrequestwidget.ui \
    deleteaccountdialog.ui \
    bannedcontactswidget.ui \
    photoboothwidget.ui \
    deletecontactdialog.ui

win32: LIBS += -lole32 -luuid -lshlwapi
LIBS += -lqrencode

INCLUDEPATH += $${RING}/include/libringclient
INCLUDEPATH += $${RING}/include

LIBS += -L$${RING}/lib -lringclient

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
    translations/ring_client_windows_he.ts \
    translations/ring_client_windows_sk_SK.ts \
    translations/ring_client_windows_pl.ts \
    translations/ring_client_windows_nl.ts \
    translations/ring_client_windows_it_IT.ts \
    translations/ring_client_windows_bg.ts \
    translations/ring_client_windows_id.ts \
    translations/ring_client_windows_en_GB.ts \
    translations/ring_client_windows_pl_PL.ts \
    translations/ring_client_windows.ts \
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

    RUNTIME.files = $${RING}/bin/libring.dll $${RING}/bin/libringclient.dll  $${RING}/bin/libqrencode.dll
    RUNTIME.path = $$OUT_PWD/release

    LRC_TRANSLATION.files = $${RING}/share/libringclient/translations
    LRC_TRANSLATION.path = $$OUT_PWD/release/share/libringclient/

    QTRUNTIME.files = $$RUNTIMEDIR/Qt5Core.dll $$RUNTIMEDIR/Qt5Widgets.dll \
                            $$RUNTIMEDIR/Qt5Gui.dll $$RUNTIMEDIR/Qt5Svg.dll \
                            $$RUNTIMEDIR/Qt5Xml.dll $$RUNTIMEDIR/Qt5WinExtras.dll \
                            $$RUNTIMEDIR/Qt5Network.dll
    QTRUNTIME.path = $$OUT_PWD/release

    QTDEPSRUNTIME.files = $$RUNTIMEDIR/zlib1.dll \
                            $$RUNTIMEDIR/libfreetype-6.dll $$RUNTIMEDIR/libglib-2.0-0.dll \
                            $$RUNTIMEDIR/libharfbuzz-0.dll \
                            $$RUNTIMEDIR/libintl-8.dll $$RUNTIMEDIR/libpcre-1.dll \
                            $$RUNTIMEDIR/libpcre16-0.dll $$RUNTIMEDIR/libpng16-16.dll \
                            $$RUNTIMEDIR/libjpeg-62.dll $$RUNTIMEDIR/iconv.dll \
                            $$RUNTIMEDIR/libidn-11.dll $$RUNTIMEDIR/liblzma-5.dll \
                            $$RUNTIMEDIR/libGLESv2.dll $$RUNTIMEDIR/libbz2-1.dll
    QTDEPSRUNTIME.path = $$OUT_PWD/release

    QTPLATFORMS.files = $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    QTPLATFORMS.path = $$OUT_PWD/release/platforms

    QTPLUGINIMAGE.files = $$[QT_INSTALL_PLUGINS]/imageformats/
    QTPLUGINIMAGE.path = $$OUT_PWD/release

    LIBSTD.files = $$RUNTIMEDIR/libgcc_s_sjlj-1.dll $$RUNTIMEDIR/libstdc++-6.dll \
                    $$RUNTIMEDIR/libwinpthread-1.dll $$RUNTIMEDIR/libgcc_s_seh-1.dll
    LIBSTD.path = $$OUT_PWD/release

    INSTALLS += RINGTONES PACKAGING LICENSE RUNTIME LRC_TRANSLATION QTRUNTIME QTDEPSRUNTIME \
                QTPLUGINIMAGE QTPLATFORMS LIBSTD

    DEFINES += ENABLE_AUTOUPDATE
    LIBS += -L$${RING}/lib -lWinSparkle
    WINSPARKLE.files = $${RING}/bin/WinSparkle.dll $${RING}/bin/libringclient.dll
    WINSPARKLE.path = $$OUT_PWD/release
    INSTALLS += WINSPARKLE
    message("AUTO UPDATES enabled")
}
