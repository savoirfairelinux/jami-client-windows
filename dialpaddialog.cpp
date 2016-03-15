/***************************************************************************
 * Copyright (C) 2016 by Savoir-faire Linux                                *
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

#include "dialpaddialog.h"
#include "ui_dialpaddialog.h"

#include <QKeyEvent>

#include "callmodel.h"

DialpadDialog::DialpadDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DialpadDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    for (int i = 0; i < layout()->count(); i++) {
        if (QPushButton* button = qobject_cast<QPushButton*>(layout()->itemAt(i)->widget()))
            connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    }

    installEventFilter(this);
}

DialpadDialog::~DialpadDialog()
{
    delete ui;
}

void
DialpadDialog::onButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    CallModel::instance().selectedCall()->playDTMF(button->text());
}

bool DialpadDialog::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if ((keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_9)
                || keyEvent->key() == Qt::Key_Asterisk
                || keyEvent->key() == Qt::Key_NumberSign) {
            CallModel::instance().selectedCall()->playDTMF(QChar(keyEvent->key()));
            return true;
        }
    }
    return QDialog::eventFilter(target, event);
}
