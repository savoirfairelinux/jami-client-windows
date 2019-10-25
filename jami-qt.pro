TEMPLATE = app
TARGET = jami-qt

CONFIG += c++17

QT += widgets xml multimedia multimediawidgets network webenginewidgets svg

isEmpty(LRC) {
    LRC=../../install/lrc/
}

INCLUDEPATH += $${LRC}/include/libringclient
INCLUDEPATH += $${LRC}/include

LIBS += -L$${LRC}/lib -lringclient
LIBS += -lqrencode

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
        ./downloadmanager.h \
        ./lrcinstance.h \
        ./passworddialog.h \
        ./selectareadialog.h \
        ./toggleswitch.h \
        ./videowidget.h \
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
        ./audiorecordview.h \
        ./cliprecorder.h \
        ./videorecordview.h
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
        ./videowidget.cpp \
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
        ./downloadmanager.cpp \
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
        ./audiorecordview.cpp \
        ./cliprecorder.cpp \
        ./videorecordview.cpp
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
        ./audiorecordview.ui \
        ./cliprecorder.ui \
        ./videorecordview.ui
RESOURCES += ressources.qrc
