/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                           *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "changelogscrollwidget.h"
#include "ui_changelogscrollwidget.h"

#include "version.h"
#include "ringthemeutils.h"
#include "utils.h"
#include "settingskey.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>

ChangeLogScrollWidget::ChangeLogScrollWidget(QWidget *parent) :
    PopupWidget(parent, RingTheme::lightGrey_, PopupDialog::SpikeLabelAlignment::None),
    ui(new Ui::ChangeLogScrollWidget)
{
    ui->setupUi(this);

    ui->versionLabel->setText((isBeta ? "Beta " : "") + QString("Version: ") + QString(VERSION_STRING));

    connect(ui->logCloseButton, &QAbstractButton::clicked,
        [this] {
            getContainer()->accept();
        });
}

ChangeLogScrollWidget::~ChangeLogScrollWidget()
{
    delete ui;
}

void
ChangeLogScrollWidget::insertScrollableHTML(const QString & html)
{
    ui->logTextEdit->insertHtml(html);
}

void
ChangeLogScrollWidget::setToCenter(const QRect& rect)
{
    int x = (rect.width() - getContainer()->width()) / 2;
    int y = (rect.height() - getContainer()->height()) / 2;
    getContainer()->move(x, y);
}

void
ChangeLogScrollWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    QSettings settings("jami.net", "Jami");

    if (!settings.contains(SettingsKey::changeLogShownOnce)) {
        settings.setValue(SettingsKey::changeLogShownOnce, true);
    }
}
