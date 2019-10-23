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
HEADERS += \
           ./src/dialogs/aboutdialog.h \
           ./src/bridging/lrcinstance.h \
           ./src/bridging/rendermanager.h \
           ./src/delegates/accountitemdelegate.h \
           ./src/delegates/contactpickeritemdelegate.h \
           ./src/delegates/conversationitemdelegate.h \
           ./src/dialogs/accountmigrationdialog.h \
           ./src/dialogs/contactpicker.h \
           ./src/dialogs/deleteaccountdialog.h \
           ./src/dialogs/linkdevicedialog.h \
           ./src/dialogs/nameregistrationdialog.h \
           ./src/dialogs/passworddialog.h \
           ./src/dialogs/popupdialog.h \
           ./src/dialogs/updateconfirmdialog.h \
           ./src/dialogs/updatedownloaddialog.h \
           ./src/mainwindow/advancedsettingswidget.h \
           ./src/mainwindow/advancedsipsettingwidget.h \
           ./src/mainwindow/banneditemwidget.h \
           ./src/mainwindow/bezierconnectorwidget.h \
           ./src/mainwindow/callaudioonlyavataroverlay.h \
           ./src/mainwindow/callwidget.h \
           ./src/mainwindow/conversationsfilterwidget.h \
           ./src/mainwindow/collapsiblepasswordwidget.h \
           ./src/mainwindow/currentaccountcombobox.h \
           ./src/mainwindow/deviceitemwidget.h \
           ./src/mainwindow/distantwidget.h \
           ./src/mainwindow/iconbutton.h \
           ./src/mainwindow/idlabel.h \
           ./src/mainwindow/invitebuttonswidget.h \
           ./src/mainwindow/levelmeter.h \
           ./src/mainwindow/mainwindow.h \
           ./src/mainwindow/messagewebpage.h \
           ./src/mainwindow/messagewebview.h \
           ./src/mainwindow/navwidget.h \
           ./src/mainwindow/newwizardwidget.h \
           ./src/mainwindow/notifypushbutton.h \
           ./src/mainwindow/overlaybutton.h \
           ./src/mainwindow/photoboothwidget.h \
           ./src/mainwindow/previewwidget.h \
           ./src/mainwindow/recordoverlay.h \
           ./src/mainwindow/recordwidget.h \
           ./src/mainwindow/ringbutton.h \
           ./src/mainwindow/ringcontactlineedit.h \
           ./src/mainwindow/selectareadialog.h \
           ./src/mainwindow/settingskey.h \
           ./src/mainwindow/settingswidget.h \
           ./src/mainwindow/sipinputpanel.h \
           ./src/mainwindow/smartlistselectorbuttonnotifier.h \
           ./src/mainwindow/smartlistview.h \
           ./src/mainwindow/toggleswitch.h \
           ./src/mainwindow/videooverlay.h \
           ./src/mainwindow/videoview.h \
           ./src/mainwindow/videowidgetbase.h \
           ./src/mainwindow/webchathelpers.h \
           ./src/misc/animationhelpers.h \
           ./src/misc/connectivitymonitor.h \
           ./src/misc/globalsystemtray.h \
           ./src/misc/networkmanager.h \
           ./src/misc/pixbufmanipulator.h \
           ./src/misc/ringthemeutils.h \
           ./src/misc/runguard.h \
           ./src/misc/section.h \
           ./src/misc/splashscreen.h \
           ./src/misc/utils.h \
           ./src/misc/version.h \
           ./src/misc/widgethelpers.h \
           ./src/models/accountlistmodel.h \
           ./src/models/bannedlistmodel.h \
           ./src/models/smartlistmodel.h

SOURCES += \
          ./src/bridging/rendermanager.cpp \
          ./src/delegates/accountitemdelegate.cpp \
          ./src/delegates/contactpickeritemdelegate.cpp \
          ./src/delegates/conversationitemdelegate.cpp \
          ./src/dialogs/aboutdialog.cpp \
          ./src/dialogs/accountmigrationdialog.cpp \
          ./src/dialogs/contactpicker.cpp \
          ./src/dialogs/deleteaccountdialog.cpp \
          ./src/dialogs/linkdevicedialog.cpp \
          ./src/dialogs/nameregistrationdialog.cpp \
          ./src/dialogs/passworddialog.cpp \
          ./src/dialogs/popupdialog.cpp \
          ./src/dialogs/updateconfirmdialog.cpp \
          ./src/dialogs/updatedownloaddialog.cpp \
          ./src/main.cpp \
          ./src/mainwindow/advancedsettingswidget.cpp \
          ./src/mainwindow/advancedsipsettingwidget.cpp \
          ./src/mainwindow/banneditemwidget.cpp \
          ./src/mainwindow/bezierconnectorwidget.cpp \
          ./src/mainwindow/callaudioonlyavataroverlay.cpp \
          ./src/mainwindow/callwidget.cpp \
          ./src/mainwindow/collapsiblepasswordwidget.cpp \
          ./src/mainwindow/conversationsfilterwidget.cpp \
          ./src/mainwindow/currentaccountcombobox.cpp \
          ./src/mainwindow/deviceitemwidget.cpp \
          ./src/mainwindow/distantwidget.cpp \
          ./src/mainwindow/iconbutton.cpp \
          ./src/mainwindow/idlabel.cpp \
          ./src/mainwindow/invitebuttonswidget.cpp \
          ./src/mainwindow/levelmeter.cpp \
          ./src/mainwindow/mainwindow.cpp \
          ./src/mainwindow/messagewebpage.cpp \
          ./src/mainwindow/messagewebview.cpp \
          ./src/mainwindow/navwidget.cpp \
          ./src/mainwindow/newwizardwidget.cpp \
          ./src/mainwindow/notifypushbutton.cpp \
          ./src/mainwindow/overlaybutton.cpp \
          ./src/mainwindow/photoboothwidget.cpp \
          ./src/mainwindow/previewwidget.cpp \
          ./src/mainwindow/recordoverlay.cpp \
          ./src/mainwindow/recordwidget.cpp \
          ./src/mainwindow/ringbutton.cpp \
          ./src/mainwindow/ringcontactlineedit.cpp \
          ./src/mainwindow/selectareadialog.cpp \
          ./src/mainwindow/settingswidget.cpp \
          ./src/mainwindow/sipinputpanel.cpp \
          ./src/mainwindow/smartlistselectorbuttonnotifier.cpp \
          ./src/mainwindow/smartlistview.cpp \
          ./src/mainwindow/toggleswitch.cpp \
          ./src/mainwindow/videooverlay.cpp \
          ./src/mainwindow/videoview.cpp \
          ./src/mainwindow/videowidgetbase.cpp \
          ./src/mainwindow/webchathelpers.cpp \
          ./src/misc/animationhelpers.cpp \
          ./src/misc/connectivitymonitor.cpp \
          ./src/misc/globalsystemtray.cpp \
          ./src/misc/networkmanager.cpp \
          ./src/misc/pixbufmanipulator.cpp \
          ./src/misc/runguard.cpp \
          ./src/misc/section.cpp \
          ./src/misc/splashscreen.cpp \
          ./src/misc/utils.cpp \
          ./src/misc/widgethelpers.cpp \
          ./src/models/accountlistmodel.cpp \
          ./src/models/bannedlistmodel.cpp \
          ./src/models/smartlistmodel.cpp
FORMS += \
          ./src/ui/aboutdialog.ui \
          ./src/ui/accountdetails.ui \
          ./src/ui/accountmigration.ui \
          ./src/ui/advancedsettingswidget.ui \
          ./src/ui/advancedsipsettingwidget.ui \
          ./src/ui/animatedoverlay.ui \
          ./src/ui/banneditemwidget.ui \
          ./src/ui/callaudioonlyavataroverlay.ui \
          ./src/ui/callwidget.ui \
          ./src/ui/collapsiblepasswordwidget.ui \
          ./src/ui/contactpicker.ui \
          ./src/ui/deleteaccountdialog.ui \
          ./src/ui/deviceitemwidget.ui \
          ./src/ui/invitebuttonswidget.ui \
          ./src/ui/linkdevicedialog.ui \
          ./src/ui/mainwindow.ui \
          ./src/ui/nameregistrationdialog.ui \
          ./src/ui/newwizardwidget.ui \
          ./src/ui/passworddialog.ui \
          ./src/ui/photoboothwidget.ui \
          ./src/ui/popupdialog.ui \
          ./src/ui/recordoverlay.ui \
          ./src/ui/recordwidget.ui
          ./src/ui/ringbutton.ui \
          ./src/ui/section.ui \
          ./src/ui/settingswidget.ui \
          ./src/ui/sipkeypad.ui \
          ./src/ui/updateconfirmdialog.ui \
          ./src/ui/updatedownloaddialog.ui \
          ./src/ui/videooverlay.ui \
          ./src/ui/videoview.ui
RESOURCES += resources/ressources.qrc
