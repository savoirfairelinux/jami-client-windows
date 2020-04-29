/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "accountlistmodel.h"
#include "lrcinstance.h"

#include <QImage>
#include <QObject>
#include <QPair>
#include <QQuickImageProvider>
#include <QString>

class QrImageProvider : public QObject, public QQuickImageProvider
{
public:
    QrImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image,
                              QQmlImageProviderBase::ForceAsynchronousImageLoading)
    {}

    QPair<int, int>
    getIndexFromID(const QString &id)
    {
        // should be string like account_0_0 (index 0, index 1)
        auto list = id.split('_', QString::SkipEmptyParts);
        if (list.contains("account")) {
            return QPair<int, int>(list[1].toInt(), list[2].toInt());
        }
        qDebug().noquote() << "accountImage provider id format incorrect";
        return QPair<int, int>(0, 0);
    }

    QImage
    requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        Q_UNUSED(size);

        auto indexPair = getIndexFromID(id);

        auto accountList = LRCInstance::accountModel().getAccountList();
        if (accountList.size() <= indexPair.first) {
            return QImage();
        }

        auto &accountInfo = LRCInstance::accountModel().getAccountInfo(
            accountList.at(indexPair.first));

        if (!requestedSize.isEmpty())
            return Utils::setupQRCode(accountInfo.profileInfo.uri, 0)
                .scaled(requestedSize, Qt::KeepAspectRatio);
        else
            return Utils::setupQRCode(accountInfo.profileInfo.uri, 0);
    }
};