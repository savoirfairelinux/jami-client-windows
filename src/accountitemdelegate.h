/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
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
#pragma once

#include <QPainter>
#include <QItemDelegate>
#include <QPainterPath>

class AccountItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit AccountItemDelegate(QObject* parent = nullptr);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    constexpr static int fontSize_ = 11;
    const QFont font_ = QFont("Arial", fontSize_);
    constexpr static int topPadding_ = 6;
    constexpr static int bottomPadding_ = 6;
    constexpr static int leftPadding_ = 12;
    constexpr static int rightPadding_ = 12;
    constexpr static int avatarSize_ = 36;
    constexpr static int cellHeight_ = 48;
};
