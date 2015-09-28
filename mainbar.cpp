/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "mainbar.h"
#include "ui_mainbar.h"

#include <QSettings>
#include <QMessageBox>

#include "settingskey.h"

#include "callmodel.h"
#include "aboutdialog.h"

MainBar::MainBar(QWidget *parent) :
    NavWidget(END, parent),
    ui(new Ui::MainBar),
    menu_(new QMenu())
{
    ui->setupUi(this);

    auto aboutAction = new QAction(tr("About"), this);
    menu_->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    auto exitAction = new QAction(tr("Exit"), this);
    menu_->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(on_exitButton_clicked()));

    ui->logoToolButton->setMenu(menu_);
    ui->logoToolButton->setPopupMode(QToolButton::InstantPopup);
}

MainBar::~MainBar()
{
    delete ui;
}

void
MainBar::on_confButton_clicked()
{
    emit NavigationRequested(ScreenEnum::ConfScreen);
}

void
MainBar::on_callLineEdit_returnPressed()
{
    callAction();
}

void
MainBar::on_callButton_clicked()
{
    callAction();
}

void
MainBar::callAction() {
    if (ui->callLineEdit->text().isEmpty())
        return;
    auto outCall = CallModel::instance()->dialingCall(ui->callLineEdit->text());
    outCall->setDialNumber(ui->callLineEdit->text());
    outCall->performAction(Call::Action::ACCEPT);
}

void
MainBar::showAboutDialog() {
    AboutDialog *aboutDialog = new AboutDialog();

    aboutDialog->exec();

    delete aboutDialog;
}

void
MainBar::on_exitButton_clicked()
{
    QSettings settings;

    if (not settings.value(SettingsKey::closeOrMinimized).isValid()) {
        QMessageBox confirmationDialog;

        confirmationDialog.setText("Do you want to keep Ring minimized ?");
        confirmationDialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        auto ret = confirmationDialog.exec();

        if (ret == QMessageBox::Ok)
            settings.setValue(SettingsKey::closeOrMinimized, true);
        else
            settings.setValue(SettingsKey::closeOrMinimized, false);
    }
    if (settings.value(SettingsKey::closeOrMinimized).toBool() == true)
        emit minimize();
    else
        QCoreApplication::exit();
}

void
MainBar::on_minimizeButton_clicked()
{
    emit minimize();
}

void
MainBar::atExit() {

}
