TEMPLATE = app
TARGET = jami-qt

QT += widgets xml multimedia testlib multimediawidgets network webenginewidgets

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
            webchathelpers.h \
            advancedsipsettingwidget.h \
            downloadbar.h \
            updateconfimdialog.h \
            downloadmanger.h
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
            videooverlay.ui \
            advancedsipsettingwidget.ui \
            downloadbar.ui \
            updateconfirm.ui
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
            webchathelpers.cpp \
            advancedsipsettingwidget.cpp \
            downloadbar.cpp \
            updateconfimdialog.cpp \
            downloadmanger.cpp
RESOURCES += ressources.qrc
