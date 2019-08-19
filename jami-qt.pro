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
    ./videowidget.h \
    ./downloadbar.h \
    ./updateconfirmdialog.h \
    ./downloadmanger.h \
    ./callaudioonlyavataroverlay.h \
    ./sipinputpanel.h

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
    ./windowscontactbackend.cpp \
    ./downloadbar.cpp \
    ./updateconfirmdialog.cpp \
    ./downloadmanger.cpp \
    ./callaudioonlyavataroverlay.cpp \
    ./sipinputpanel.cpp

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
    ./videoview.ui \
    ./downloadbar.ui \
    ./updateconfirmdialog.ui \
    ./contactpicker.ui \
    ./callaudioonlyavataroverlay.ui \
    ./sipkeypad.ui

RESOURCES += ressources.qrc
