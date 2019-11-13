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
HEADERS += ./src/aboutdialog.h \
        ./src/banneditemwidget.h \
        ./src/conversationsfilterwidget.h \
        ./src/idlabel.h \
        ./src/messagewebview.h \
        ./src/ringbutton.h \
        ./src/smartlistmodel.h \
        ./src/utils.h \
        ./src/accountitemdelegate.h \
        ./src/bannedlistmodel.h \
        ./src/currentaccountcombobox.h \
        ./src/invitebuttonswidget.h \
        ./src/nameregistrationdialog.h \
        ./src/ringcontactlineedit.h \
        ./src/smartlistselectorbuttonnotifier.h \
        ./src/version.h \
        ./src/accountlistmodel.h \
        ./src/bezierconnectorwidget.h \
        ./src/deleteaccountdialog.h \
        ./src/levelmeter.h \
        ./src/navwidget.h \
        ./src/ringthemeutils.h \
        ./src/smartlistview.h \
        ./src/videooverlay.h \
        ./src/advancedsettingswidget.h \
        ./src/callwidget.h \
        ./src/deviceitemwidget.h \
        ./src/linkdevicedialog.h \
        ./src/newwizardwidget.h \
        ./src/runguard.h \
        ./src/splashscreen.h \
        ./src/videoview.h \
        ./src/advancedsipsettingwidget.h \
        ./src/contactpicker.h \
        ./src/downloadmanager.h \
        ./src/lrcinstance.h \
        ./src/passworddialog.h \
        ./src/selectareadialog.h \
        ./src/toggleswitch.h \
        ./src/videowidget.h \
        ./src/animationhelpers.h \
        ./src/contactpickeritemdelegate.h \
        ./src/globalsystemtray.h \
        ./src/mainwindow.h \
        ./src/photoboothwidget.h \
        ./src/settingskey.h \
        ./src/updateconfirmdialog.h \
        ./src/webchathelpers.h \
        ./src/avatargraphicsview.h \
        ./src/conversationitemdelegate.h \
        ./src/iconbutton.h \
        ./src/messagewebpage.h \
        ./src/pixbufmanipulator.h \
        ./src/settingswidget.h \
        ./src/updatedownloaddialog.h \
        ./src/sipinputpanel.h \
        ./src/callaudioonlyavataroverlay.h \
        ./src/overlaybutton.h \
        ./src/accountmigrationdialog.h \
        ./src/previewwidget.h \
        ./src/rendermanager.h \
        ./src/distantwidget.h \
        ./src/videowidgetbase.h \
        ./src/section.h \
        ./src/collapsiblepasswordwidget.h \
        ./src/notifypushbutton.h \
        ./src/popupdialog.h \
        ./src/recordoverlay.h \
        ./src/widgethelpers.h \
        ./src/recordwidget.h \
        ./src/changelogscrollwidget.h
SOURCES += ./src/aboutdialog.cpp \
        ./src/banneditemwidget.cpp \
        ./src/conversationsfilterwidget.cpp \
        ./src/idlabel.cpp \
        ./src/messagewebview.cpp \
        ./src/ringbutton.cpp \
        ./src/smartlistview.cpp \
        ./src/videoview.cpp \
        ./src/accountitemdelegate.cpp \
        ./src/bannedlistmodel.cpp \
        ./src/currentaccountcombobox.cpp \
        ./src/invitebuttonswidget.cpp \
        ./src/nameregistrationdialog.cpp \
        ./src/ringcontactlineedit.cpp \
        ./src/splashscreen.cpp \
        ./src/videowidget.cpp \
        ./src/animationhelpers.cpp \
        ./src/accountlistmodel.cpp \
        ./src/bezierconnectorwidget.cpp \
        ./src/deleteaccountdialog.cpp \
        ./src/levelmeter.cpp \
        ./src/navwidget.cpp \
        ./src/runguard.cpp \
        ./src/toggleswitch.cpp \
        ./src/webchathelpers.cpp \
        ./src/advancedsettingswidget.cpp \
        ./src/callwidget.cpp \
        ./src/deviceitemwidget.cpp \
        ./src/linkdevicedialog.cpp \
        ./src/newwizardwidget.cpp \
        ./src/selectareadialog.cpp \
        ./src/updateconfirmdialog.cpp \
        ./src/advancedsipsettingwidget.cpp \
        ./src/contactpicker.cpp \
        ./src/downloadmanager.cpp \
        ./src/main.cpp \
        ./src/passworddialog.cpp \
        ./src/settingswidget.cpp \
        ./src/updatedownloaddialog.cpp \
        ./src/animationhelpers.cpp \
        ./src/contactpickeritemdelegate.cpp \
        ./src/globalsystemtray.cpp \
        ./src/mainwindow.cpp \
        ./src/photoboothwidget.cpp \
        ./src/smartlistmodel.cpp \
        ./src/utils.cpp \
        ./src/avatargraphicsview.cpp \
        ./src/conversationitemdelegate.cpp \
        ./src/iconbutton.cpp \
        ./src/messagewebpage.cpp \
        ./src/pixbufmanipulator.cpp \
        ./src/smartlistselectorbuttonnotifier.cpp \
        ./src/videooverlay.cpp \
        ./src/sipinputpanel.cpp \
        ./src/callaudioonlyavataroverlay.cpp \
        ./src/overlaybutton.cpp \
        ./src/accountmigrationdialog.cpp \
        ./src/previewwidget.cpp \
        ./src/rendermanager.cpp \
        ./src/distantwidget.cpp \
        ./src/videowidgetbase.cpp \
        ./src/section.cpp \
        ./src/collapsiblepasswordwidget.cpp \
        ./src/notifypushbutton.cpp \
        ./src/popupdialog.cpp \
        ./src/recordoverlay.cpp \
        ./src/widgethelpers.cpp \
        ./src/recordwidget.cpp \
        ./src/changelogscrollwidget.cpp
FORMS += ./src/ui/aboutdialog.ui \
        ./src/ui/advancedsipsettingwidget.ui \
        ./src/ui/callwidget.ui \
        ./src/ui/deviceitemwidget.ui \
        ./src/ui/mainwindow.ui \
        ./src/ui/passworddialog.ui \
        ./src/ui/ringbutton.ui \
        ./src/ui/updatedownloaddialog.ui \
        ./src/ui/animatedoverlay.ui \
        ./src/ui/contactpicker.ui \
        ./src/ui/invitebuttonswidget.ui \
        ./src/ui/nameregistrationdialog.ui \
        ./src/ui/settingswidget.ui \
        ./src/ui/videooverlay.ui \
        ./src/ui/advancedsettingswidget.ui \
        ./src/ui/banneditemwidget.ui \
        ./src/ui/deleteaccountdialog.ui \
        ./src/ui/linkdevicedialog.ui \
        ./src/ui/newwizardwidget.ui \
        ./src/ui/photoboothwidget.ui \
        ./src/ui/updateconfirmdialog.ui \
        ./src/ui/videoview.ui \
        ./src/ui/sipkeypad.ui \
        ./src/ui/callaudioonlyavataroverlay.ui \
        ./src/ui/accountmigration.ui \
        ./src/ui/section.ui \
        ./src/ui/collapsiblepasswordwidget.ui \
        ./src/ui/popupdialog.ui \
        ./src/ui/recordoverlay.ui \
        ./src/ui/recordwidget.ui \
        ./src/ui/changelogscrollwidget.ui
RESOURCES += resources/ressources.qrc \
    src/ui/ressources.qrc
