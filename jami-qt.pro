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
HEADERS += aboutdialog.h \
            bannedcontactswidget.h \
            idlabel.h \
            globalsystemtray.h \
            selectareadialog.h \
            ringcontactlineedit.h \
            version.h \
            windowscontactbackend.h \
            smartlistview.h \
            settingskey.h \
            photoboothwidget.h \
            navwidget.h \
            videowidget.h \
            videoview.h \
            accountitemdelegate.h \
            accountlistmodel.h \
            advancedsettingswidget.h \
            avatargraphicsview.h \
            animationhelpers.h \
            bannedlistmodel.h \
            callwidget.h \
            conversationitemdelegate.h \
            deleteaccountdialog.h \
            currentaccountcombobox.h \
            conversationsfilterwidget.h \
            lrcinstance.h \
            linkdevwidget.h \
            invitebuttonswidget.h \
            messagewebview.h \
            messagewebpage.h \
            mainwindow.h \
            photoboothdialog.h \
            passworddialog.h \
            newwizardwidget.h \
            regnamedialog.h \
            pixbufmanipulator.h \
            ringbutton.h \
            settingsitemwidget.h \
            setavatardialog.h \
            runguard.h \
            ringthemeutils.h \
            settingswidget.h \
            smartlistselectorbuttonnotifier.h \
            smartlistmodel.h \
            shmclient.h \
            videooverlay.h \
            utils.h \
            webchathelpers.h
FORMS += contactdialog.ui \
            videoview.ui \
            animatedoverlay.ui \
            bannedcontactswidget.ui \
            ringbutton.ui \
            deviceeditwidget.ui \
            invitebuttonswidget.ui \
            photoboothdialog.ui \
            passworddialog.ui \
            setavatardialog.ui \
            deleteaccountdialog.ui \
            aboutdialog.ui \
            accountdetails.ui \
            mainwindow.ui \
            linkdevwidget.ui \
            photoboothwidget.ui \
            newwizardwidget.ui \
            regnamedialog.ui \
            advancedsettingswidget.ui \
            callwidget.ui \
            settingswidget.ui \
            videooverlay.ui
SOURCES += aboutdialog.cpp \
            bannedcontactswidget.cpp \
            idlabel.cpp \
            globalsystemtray.cpp \
            pixbufmanipulator.cpp \
            navwidget.cpp \
            ringcontactlineedit.cpp \
            smartlistview.cpp \
            selectareadialog.cpp \
            windowscontactbackend.cpp \
            videowidget.cpp \
            accountlistmodel.cpp \
            accountitemdelegate.cpp \
            advancedsettingswidget.cpp \
            animationhelpers.cpp \
            bannedlistmodel.cpp \
            avatargraphicsview.cpp \
            callwidget.cpp \
            conversationsfilterwidget.cpp \
            conversationitemdelegate.cpp \
            deleteaccountdialog.cpp \
            currentaccountcombobox.cpp \
            main.cpp \
            linkdevwidget.cpp \
            invitebuttonswidget.cpp \
            messagewebview.cpp \
            messagewebpage.cpp \
            mainwindow.cpp \
            photoboothdialog.cpp \
            passworddialog.cpp \
            newwizardwidget.cpp \
            regnamedialog.cpp \
            photoboothwidget.cpp \
            ringbutton.cpp \
            settingsitemwidget.cpp \
            setavatardialog.cpp \
            runguard.cpp \
            settingswidget.cpp \
            smartlistselectorbuttonnotifier.cpp \
            smartlistmodel.cpp \
            shmclient.cpp \
            videooverlay.cpp \
            utils.cpp \
            videoview.cpp \
            webchathelpers.cpp
RESOURCES += ressources.qrc
TRANSLATIONS += translations/ring_client_windows.ts \
                translations/ring_client_windows_ar.ts \
                translations/ring_client_windows_bg.ts \
                translations/ring_client_windows_ca.ts \
                translations/ring_client_windows_cs_CZ.ts \
                translations/ring_client_windows_da.ts \
                translations/ring_client_windows_da_DK.ts \
                translations/ring_client_windows_de.ts \
                translations/ring_client_windows_de_DE.ts \
                translations/ring_client_windows_el.ts \
                translations/ring_client_windows_en_GB.ts \
                translations/ring_client_windows_eo.ts \
                translations/ring_client_windows_es.ts \
                translations/ring_client_windows_es_AR.ts \
                translations/ring_client_windows_et_EE.ts \
                translations/ring_client_windows_eu.ts \
                translations/ring_client_windows_fa_IR.ts \
                translations/ring_client_windows_fi.ts \
                translations/ring_client_windows_fr.ts \
                translations/ring_client_windows_fr_CA.ts \
                translations/ring_client_windows_fr_FR.ts \
                translations/ring_client_windows_he.ts \
                translations/ring_client_windows_hi_IN.ts \
                translations/ring_client_windows_hr.ts \
                translations/ring_client_windows_hu.ts \
                translations/ring_client_windows_id.ts \
                translations/ring_client_windows_it.ts \
                translations/ring_client_windows_it_IT.ts \
                translations/ring_client_windows_ja.ts \
                translations/ring_client_windows_ko_KR.ts \
                translations/ring_client_windows_lt.ts \
                translations/ring_client_windows_ms.ts \
                translations/ring_client_windows_nb.ts \
                translations/ring_client_windows_nl.ts \
                translations/ring_client_windows_nl_BE.ts \
                translations/ring_client_windows_nl_NL.ts \
                translations/ring_client_windows_pa.ts \
                translations/ring_client_windows_pl.ts \
                translations/ring_client_windows_pl_PL.ts \
                translations/ring_client_windows_pt.ts \
                translations/ring_client_windows_pt_BR.ts \
                translations/ring_client_windows_pt_PT.ts \
                translations/ring_client_windows_ro.ts \
                translations/ring_client_windows_ru.ts \
                translations/ring_client_windows_ru_RU.ts \
                translations/ring_client_windows_sk_SK.ts \
                translations/ring_client_windows_sl.ts \
                translations/ring_client_windows_sq_AL.ts \
                translations/ring_client_windows_sv.ts \
                translations/ring_client_windows_tr.ts \
                translations/ring_client_windows_uk.ts \
                translations/ring_client_windows_zh.ts \
                translations/ring_client_windows_zh_CN.ts \
                translations/ring_client_windows_zh_TW.ts
