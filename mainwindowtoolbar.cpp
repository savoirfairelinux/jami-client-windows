/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
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

#include <QDebug>
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QMenuBar>
#include "mainwindowtoolbar.h"

MainWindowToolBar::MainWindowToolBar(QToolBar* parent) :
    QToolBar(parent),
    spacer_(new QWidget())
{
  setFixedHeight(25);

  setMovable(false);

  contactslist_ = addAction(QIcon(":images/contactswidgetlist.png"), tr("contacts"));
  contactslist_->setCheckable(true);

  history_ = addAction(QIcon(":images/historylist.png"), tr("history"));
  history_->setCheckable(true);

  spacer_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  addWidget(spacer_);

  settings_ = addAction(QIcon(":images/settings.png"), tr("settings"));

  connect(history_, &QAction::triggered, this, &MainWindowToolBar::historyToggled);
  connect(contactslist_, &QAction::triggered, this, &MainWindowToolBar::contactslistToggled);

}

MainWindowToolBar::~MainWindowToolBar()
{
    delete spacer_;
}

void
MainWindowToolBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    QBrush background(QColor(63, 63, 63));

    painter.setBrush(background);
    painter.setPen(Qt::NoPen ); // No stroke

    painter.drawRect(0, 0, width(), height());
}

void
MainWindowToolBar::historyToggled(bool checked)
{
    if(checked)
    {
        contactslist_->setChecked(false);
        history_->setChecked(true);
    }
    else
        history_->setChecked(false);
}

void
MainWindowToolBar::contactslistToggled(bool checked)
{
    if(checked)
    {
        history_->setChecked(false);
        contactslist_->setChecked(true);
    }
    else
        contactslist_->setChecked(false);
}
