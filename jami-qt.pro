TEMPLATE = app
TARGET = jami-qt

QT += widgets xml multimedia multimediawidgets network webenginewidgets

isEmpty(LRC) {
    LRC=../../install/lrc/
}

INCLUDEPATH += $${LRC}/include/libringclient
INCLUDEPATH += $${LRC}/include

LIBS += -L$${LRC}/lib -lringclient
LIBS += -lqrencode

# Input
HEADERS += ./pixbufmanipulator.h \
    ./ringthemeutils.h \
    ./runguard.h \
    ./settingskey.h \
    ./utils.h \
    ./version.h \
    ./webchathelpers.h \
    ./windowscontactbackend.h \
    ./aboutdialog.h \
    ./callwidget.h \
    ./accountlistmodel.h \
    ./accountitemdelegate.h \
    ./deleteaccountdialog.h \
    ./globalsystemtray.h \
    ./idlabel.h \
    ./mainwindow.h \
    ./navwidget.h \
    ./photoboothwidget.h \
    ./invitebuttonswidget.h \
    ./currentaccountcombobox.h \
    ./animationhelpers.h \
    ./advancedsettingswidget.h \
    ./avatargraphicsview.h \
    ./bannedlistmodel.h \
    ./linkdevicedialog.h \
    ./bezierconnectorwidget.h \
    ./lrcinstance.h \
    ./messagewebview.h \
    ./messagewebpage.h \
    ./passworddialog.h \
    ./newwizardwidget.h \
    ./advancedsipsettingwidget.h \
    ./iconbutton.h \
    ./deviceitemwidget.h \
    ./banneditemwidget.h \
    ./ringbutton.h \
    ./ringcontactlineedit.h \
    ./nameregistrationdialog.h \
    ./selectareadialog.h \
    ./smartlistview.h \
    ./conversationitemdelegate.h \
    ./smartlistmodel.h \
    ./conversationsfilterwidget.h \
    ./smartlistselectorbuttonnotifier.h \
    ./settingswidget.h \
    ./toggleswitch.h \
    ./videooverlay.h \
    ./videoview.h \
    ./videowidget.h
SOURCES += ./advancedsettingswidget.cpp \
    ./advancedsipsettingwidget.cpp \
    ./avatargraphicsview.cpp \
    ./banneditemwidget.cpp \
    ./bannedlistmodel.cpp \
    ./animationhelpers.cpp \
    ./currentaccountcombobox.cpp \
    ./aboutdialog.cpp \
    ./accountitemdelegate.cpp \
    ./accountlistmodel.cpp \
    ./callwidget.cpp \
    ./deleteaccountdialog.cpp \
    ./deviceitemwidget.cpp \
    ./globalsystemtray.cpp \
    ./iconbutton.cpp \
    ./idlabel.cpp \
    ./bezierconnectorwidget.cpp \
    ./invitebuttonswidget.cpp \
    ./linkdevicedialog.cpp \
    ./main.cpp \
    ./mainwindow.cpp \
    ./messagewebpage.cpp \
    ./messagewebview.cpp \
    ./navwidget.cpp \
    ./newwizardwidget.cpp \
    ./passworddialog.cpp \
    ./photoboothwidget.cpp \
    ./pixbufmanipulator.cpp \
    ./nameregistrationdialog.cpp \
    ./ringbutton.cpp \
    ./ringcontactlineedit.cpp \
    ./runguard.cpp \
    ./selectareadialog.cpp \
    ./conversationsfilterwidget.cpp \
    ./settingswidget.cpp \
    ./smartlistselectorbuttonnotifier.cpp \
    ./smartlistview.cpp \
    ./conversationitemdelegate.cpp \
    ./smartlistmodel.cpp \
    ./toggleswitch.cpp \
    ./utils.cpp \
    ./videooverlay.cpp \
    ./videoview.cpp \
    ./videowidget.cpp \
    ./webchathelpers.cpp \
    ./windowscontactbackend.cpp
FORMS += ./aboutdialog.ui \
    ./advancedsettingswidget.ui \
    ./advancedsipsettingwidget.ui \
    ./animatedoverlay.ui \
    ./banneditemwidget.ui \
    ./callwidget.ui \
    ./deleteaccountdialog.ui \
    ./deviceitemwidget.ui \
    ./invitebuttonswidget.ui \
    ./linkdevicedialog.ui \
    ./mainwindow.ui \
    ./nameregistrationdialog.ui \
    ./newwizardwidget.ui \
    ./passworddialog.ui \
    ./photoboothdialog.ui \
    ./photoboothwidget.ui \
    ./ringbutton.ui \
    ./settingswidget.ui \
    ./videooverlay.ui \
    ./videoview.ui
TRANSLATIONS += ./translations/ring_client_windows.ts \
    ./translations/ring_client_windows_ar.ts \
    ./translations/ring_client_windows_bg.ts \
    ./translations/ring_client_windows_ca.ts \
    ./translations/ring_client_windows_cs_CZ.ts \
    ./translations/ring_client_windows_da_DK.ts \
    ./translations/ring_client_windows_de.ts \
    ./translations/ring_client_windows_de_DE.ts \
    ./translations/ring_client_windows_el.ts \
    ./translations/ring_client_windows_en_GB.ts \
    ./translations/ring_client_windows_eo.ts \
    ./translations/ring_client_windows_es.ts \
    ./translations/ring_client_windows_es_AR.ts \
    ./translations/ring_client_windows_et_EE.ts \
    ./translations/ring_client_windows_eu.ts \
    ./translations/ring_client_windows_fa_IR.ts \
    ./translations/ring_client_windows_fi.ts \
    ./translations/ring_client_windows_fr.ts \
    ./translations/ring_client_windows_fr_CA.ts \
    ./translations/ring_client_windows_fr_FR.ts \
    ./translations/ring_client_windows_he.ts \
    ./translations/ring_client_windows_hi_IN.ts \
    ./translations/ring_client_windows_hr.ts \
    ./translations/ring_client_windows_hu.ts \
    ./translations/ring_client_windows_id.ts \
    ./translations/ring_client_windows_it.ts \
    ./translations/ring_client_windows_it_IT.ts \
    ./translations/ring_client_windows_ja.ts \
    ./translations/ring_client_windows_ko_KR.ts \
    ./translations/ring_client_windows_lt.ts \
    ./translations/ring_client_windows_ms.ts \
    ./translations/ring_client_windows_nb.ts \
    ./translations/ring_client_windows_nl.ts \
    ./translations/ring_client_windows_nl_BE.ts \
    ./translations/ring_client_windows_nl_NL.ts \
    ./translations/ring_client_windows_pa.ts \
    ./translations/ring_client_windows_pl.ts \
    ./translations/ring_client_windows_pl_PL.ts \
    ./translations/ring_client_windows_pt.ts \
    ./translations/ring_client_windows_pt_BR.ts \
    ./translations/ring_client_windows_ro.ts \
    ./translations/ring_client_windows_ru.ts \
    ./translations/ring_client_windows_ru_RU.ts \
    ./translations/ring_client_windows_sk_SK.ts \
    ./translations/ring_client_windows_sl.ts \
    ./translations/ring_client_windows_sq_AL.ts \
    ./translations/ring_client_windows_sv.ts \
    ./translations/ring_client_windows_tr.ts \
    ./translations/ring_client_windows_uk.ts \
    ./translations/ring_client_windows_zh.ts \
    ./translations/ring_client_windows_zh_CN.ts \
    ./translations/ring_client_windows_zh_TW.ts
RESOURCES += ressources.qrc
