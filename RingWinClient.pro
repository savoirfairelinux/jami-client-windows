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
RING_CLIENT_BUILD_DATE = $$system(date +%Y%m%d)
}
NIGHTLY_VERSION =$$system(date +'%Y%m%d')

DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
DEFINES += NIGHTLY_VERSION=\\\"$$NIGHTLY_VERSION\\\"
DEFINES += RING_CLIENT_BUILD_DATE=\"\\\"$$RING_CLIENT_BUILD_DATE\\\"\"
DEFINES += LRC_IMPORT

BUILD=$${BUILD}
TARGET = Ring
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

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
    accountstatedelegate.cpp \
    videoview.cpp \
    videooverlay.cpp \
    contactpicker.cpp \
    globalsystemtray.cpp \
    conversationitemdelegate.cpp \
    conversationsfilterwidget.cpp \
    callutilsdialog.cpp \
    idlabel.cpp \
    ringcontactlineedit.cpp \
    pixbufmanipulator.cpp \
    qualitydialog.cpp \
    ringbutton.cpp \
    photoboothdialog.cpp \
    sendcontactrequestwidget.cpp \
    contactrequestwidget.cpp \
    smartlistselectorbuttonnotifier.cpp \
    deleteaccountdialog.cpp \
    bannedcontactswidget.cpp \
    photoboothwidget.cpp \
    smartlistmodel.cpp \
    smartlistview.cpp \
    accountitemdelegate.cpp \
    accountlistmodel.cpp \
    invitebuttonswidget.cpp \
    wizardwidget.cpp \
    currentaccountcombobox.cpp \
    conversationfilterbutton.cpp \
    messagewebpage.cpp \
    messagewebview.cpp \
    webchathelpers.cpp \
    animationhelpers.cpp

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
    accountstatedelegate.h \
    videoview.h \
    videooverlay.h \
    contactpicker.h \
    settingskey.h \
    globalsystemtray.h \
    conversationitemdelegate.h \
    conversationsfilterwidget.h \
    callutilsdialog.h \
    idlabel.h \
    ringcontactlineedit.h \
    pixbufmanipulator.h \
    qualitydialog.h \
    ringthemeutils.h \
    ringbutton.h \
    photoboothdialog.h \
    sendcontactrequestwidget.h \
    contactrequestwidget.h \
    smartlistselectorbuttonnotifier.h \
    deleteaccountdialog.h \
    bannedcontactswidget.h \
    photoboothwidget.h \
    lrcinstance.h \
    smartlistmodel.h \
    smartlistview.h \
    accountitemdelegate.h \
    accountlistmodel.h \
    invitebuttonswidget.h \
    wizardwidget.h \
    currentaccountcombobox.h \
    conversationfilterbutton.h \
    messagewebpage.h \
    messagewebview.h \
    webchathelpers.h \
    animationhelpers.h


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
    videoview.ui \
    videooverlay.ui \
    contactpicker.ui \
    callutilsdialog.ui \
    qualitydialog.ui \
    ringbutton.ui \
    photoboothdialog.ui \
    sendcontactrequestwidget.ui \
    contactrequestwidget.ui \
    deleteaccountdialog.ui \
    bannedcontactswidget.ui \
    photoboothwidget.ui \
    invitebuttonswidget.ui \
    wizardwidget.ui \
    animatedoverlay.ui

win32: LIBS += -lole32 -luuid -lshlwapi -lgdi32
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
    translations/ring_client_windows_nl_BE.ts \
    translations/ring_client_windows_he.ts \
    translations/ring_client_windows_sk_SK.ts \
    translations/ring_client_windows_pl.ts \
    translations/ring_client_windows_es_AR.ts \
    translations/ring_client_windows_nl.ts \
    translations/ring_client_windows_it_IT.ts \
    translations/ring_client_windows_bg.ts \
    translations/ring_client_windows_pt_PT.ts \
    translations/ring_client_windows_id.ts \
    translations/ring_client_windows_en_GB.ts \
    translations/ring_client_windows_pl_PL.ts \
    translations/ring_client_windows.ts \
    translations/ring_client_windows_eu.ts \
    translations/ring_client_windows_eo.ts \
    translations/ring_client_windows_nl_NL.ts \
    translations/ring_client_windows_ru_RU.ts \
    translations/ring_client_windows_hr.ts \
    translations/ring_client_windows_da.ts \
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
                            $$RUNTIMEDIR/Qt5Network.dll $$RUNTIMEDIR/Qt5Sql.dll \
                            $$RUNTIMEDIR/Qt5WebEngineWidgets.dll $$RUNTIMEDIR/Qt5WebChannel.dll

    QTRUNTIME.path = $$OUT_PWD/release

    QTDEPSRUNTIME.files = $$RUNTIMEDIR/zlib1.dll \
                            $$RUNTIMEDIR/libfreetype-6.dll $$RUNTIMEDIR/libglib-2.0-0.dll \
                            $$RUNTIMEDIR/libharfbuzz-0.dll \
                            $$RUNTIMEDIR/libintl-8.dll $$RUNTIMEDIR/libpcre-1.dll \
                            $$RUNTIMEDIR/libpcre2-16-0.dll $$RUNTIMEDIR/libpng16-16.dll \
                            $$RUNTIMEDIR/libjpeg-62.dll $$RUNTIMEDIR/iconv.dll \
                            $$RUNTIMEDIR/libidn-11.dll $$RUNTIMEDIR/liblzma-5.dll \
                            $$RUNTIMEDIR/libGLESv2.dll $$RUNTIMEDIR/libbz2-1.dll \
                            $$RUNTIMEDIR/libsqlite3-0.dll
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
    LIBS += -L$${RING}/lib -lWinSparkle
    WINSPARKLE.files = $${RING}/bin/WinSparkle.dll $${RING}/bin/libringclient.dll
    WINSPARKLE.path = $$OUT_PWD/release
    INSTALLS += WINSPARKLE
    message("AUTO UPDATES enabled")
}
