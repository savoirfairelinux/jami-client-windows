/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#pragma once

#include "accountlistmodel.h"
#include "smartlistmodel.h"
#include "utils.h"
#include "lrcinstance.h"
#include "widgethelpers.h"

#include <QQuickImageProvider>
#include <QtQuick/QtQuick>
#include <QQuickItem>
#include <QQuickWidget>

#include <QPair>

namespace Ui {
    class SidePanelQuickWidget;
}


class SidePanelQuickWidget : public QQuickWidget
{
    Q_OBJECT;

public:
    explicit SidePanelQuickWidget(QWidget* parent = nullptr);
    ~SidePanelQuickWidget();

    void accountListUpdate();
    AccountListModel* getCurrentModel() { return accountListModel_.get(); }

protected:
    void resizeEvent(QResizeEvent* event);

private:
    std::unique_ptr<AccountListModel> accountListModel_;
};

class AccountImageProvider : public QObject, public QQuickImageProvider
{

private:
    SidePanelQuickWidget* toProvide_;

public:
    AccountImageProvider(SidePanelQuickWidget* toProvide) :
        QQuickImageProvider(QQuickImageProvider::Image, QQmlImageProviderBase::ForceAsynchronousImageLoading)
    {
        toProvide_ = toProvide;
    }

    QPair<int, int> getIndexFromID(const QString& id) {
        // should be string like account_0_0
        auto list = id.split('_', QString::SkipEmptyParts);
        if (list.contains("account")) {
            return QPair<int, int>(list[1].toInt(), list[2].toInt());
        }
        qDebug().noquote() << "accountImage provider id format incorrect";
        return QPair<int, int>(0, 0);
    }

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        Q_UNUSED(size);
        Q_UNUSED(requestedSize);

        auto model = toProvide_->getCurrentModel();
        auto indexPair = getIndexFromID(id);
        return model->data(model->index(indexPair.first, indexPair.second), AccountListModel::Role::Picture).value<QImage>();
    }
};