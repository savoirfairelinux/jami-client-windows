/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "ui_banneditemwidget.h"

#include "banneditemwidget.h"

#include "../bridging/lrcinstance.h"
#include "../misc/utils.h"

BannedItemWidget::BannedItemWidget(const QString& name,
                                   const QString& id,
                                   QWidget* parent)
    : QWidget(parent),
    ui(new Ui::BannedItemWidget)
{
    ui->setupUi(this);
    ui->labelContactName->setText(name);
    ui->labelContactId->setText(id);

    auto avatarImage = Utils::fallbackAvatar(QSize(48, 48), id, name);
    ui->labelContactAvatar->setPixmap(QPixmap::fromImage(avatarImage));

    ui->btnReAddContact->setToolTip(QObject::tr("Add as contact"));

    connect(ui->btnReAddContact, &QPushButton::clicked, this,
        [this]() {
            emit btnReAddContactClicked();
        });
}

BannedItemWidget::~BannedItemWidget()
{
    disconnect(this);
    delete ui;
}

QSize
BannedItemWidget::sizeHint() const
{
    return QSize();
}
