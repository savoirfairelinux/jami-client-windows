/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

//temporary
//TODO remove
#include <QImage>
#include <QPainter>
#include <QFont>

// Client
#include "clientaccountmodel.h"
#include "smartlistmodel.h"
#include "ringthemeutils.h"

//temporary
#include <QImage>
#include "pixbufmanipulator.h"
#include <QPainter>
#include <QFont>
#include <QTextOption>
#include <QFile>

// LRC
#include "api/newaccountmodel.h"
#include "api/account.h"

ClientAccountModel::ClientAccountModel(const lrc::api::NewAccountModel& mdl, QObject *parent)
    : QAbstractItemModel(parent),
      mdl_(mdl)
{
    connect(&mdl_, &lrc::api::NewAccountModel::accountStatusChanged,
            [this]{ emit layoutChanged();});
    connect(&mdl_, &lrc::api::NewAccountModel::accountAdded,
            [this]{ emit layoutChanged();});
    connect(&mdl_, &lrc::api::NewAccountModel::accountRemoved,
            [this]{ emit layoutChanged();});
}

QModelIndex
ClientAccountModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
        return QModelIndex();

    if(column != 0)
        return QModelIndex();

    if(row >= 0 && row < rowCount())
        return createIndex(row, column);

    return QModelIndex();
}

QModelIndex
ClientAccountModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return QModelIndex();
}

int
ClientAccountModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return mdl_.getAccountList().size();

    return 0;
}

int
ClientAccountModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    return 0;
}

QVariant
ClientAccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto& accInfo = mdl_.getAccountInfo(mdl_.getAccountList().at(index.row()));

    switch(role) {
    case Qt::DisplayRole:
    case Role::Alias:
        return QVariant(QString::fromStdString(accInfo.profileInfo.alias));
    case Role::SmartListModel:
        return QVariant::fromValue<::SmartListModel*>(new ::SmartListModel(accInfo));
    case Role::Avatar:
    {
        //TODO replace chunk with call to decorationrole in qpixbuf manipulator when LRC integrates it
        auto byteArray = QString::fromStdString(accInfo.profileInfo.avatar);

        if (byteArray.isEmpty() || byteArray.isNull()){

            // We start with a transparent avatar
            QImage avatar(QSize(500,500), QImage::Format_ARGB32);
            avatar.fill(Qt::transparent);

            // We pick a color based on the passed character
            QColor avColor = RingTheme::avatarColors_[accInfo.profileInfo.uri.at(0) % 16];

            // We draw a circle with this color
            QPainter painter(&avatar);
            painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
            painter.setPen(Qt::transparent);
            painter.setBrush(avColor);
            painter.drawEllipse(avatar.rect());

            // Then we paint a letter in the circle
            QFont segoeFont("Segoe UI", avatar.height()/2); // We use Segoe UI as recommended by Windows guidelines
            painter.setFont(segoeFont);
            painter.setPen(Qt::white);
            QRect textRect = avatar.rect();
            textRect.moveTop(textRect.top()-(avatar.height()/20)); // Empirical value that seems to correct centering nicely
            auto letter = QString::fromUtf8(accInfo.profileInfo.alias.empty() ? accInfo.profileInfo.uri.c_str() : accInfo.profileInfo.alias.c_str());
            painter.drawText(textRect, QString(letter.toUpper().at(0)), QTextOption(Qt::AlignCenter));

            return avatar;
        } else {
            return QVariant(QImage::fromData(QByteArray::fromBase64(byteArray.toStdString().c_str())));
        }
    }
//    case Role::Id:
//        return QVariant(QString::fromStdString(accInfo.id));
    case Role::Uri:
        return QVariant(QString::fromStdString(accInfo.profileInfo.uri));
    case Role::BestId:
    {
        auto registered = QString::fromStdString(accInfo.registeredName);
        auto uri = QString::fromStdString(accInfo.profileInfo.uri);
        return QVariant(registered.isEmpty() ? uri : registered);
    }
    case Role::BestName:
    {
        auto uri = QString::fromStdString(accInfo.profileInfo.uri);
        auto registered = QString::fromStdString(accInfo.registeredName);
        auto alias = QString::fromStdString(accInfo.profileInfo.alias);
        return QVariant(QString( alias.isEmpty() ? (registered.isEmpty() ? uri : registered) : alias));
    }
    case Role::Type:
        return QVariant::fromValue<lrc::api::profile::Type>(accInfo.profileInfo.type);
    case Role::ProfileInfo:
        return QVariant::fromValue<lrc::api::profile::Info>(accInfo.profileInfo);
    }

    return QVariant();
}

Qt::ItemFlags
ClientAccountModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);
    else
        return QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;
}

std::vector<std::__cxx11::string>
ClientAccountModel::getAccountList() const
{
    return mdl_.getAccountList();
}

const
lrc::api::account::Info &ClientAccountModel::getAccountInfo(const std::__cxx11::string &accountId) const
{
    return mdl_.getAccountInfo(accountId);
}

QModelIndex
ClientAccountModel::selectedAccountIndex()
{
    return index(selectedAccountRow_);
}

void
ClientAccountModel::setSelectedAccount(QModelIndex& newAccountIndex)
{
    selectedAccountRow_ = newAccountIndex.row();
    emit currentAccountChanged(newAccountIndex);
}
