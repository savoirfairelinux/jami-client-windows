/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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
 ***************************************************************************/

#pragma once

#include <QObject>
#include <QItemDelegate>

class ContactRequestItemDelegate : public QItemDelegate
{
public:
    ContactRequestItemDelegate(QObject* parent = 0);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    constexpr static int cellHeight_ = 60;
    constexpr static int sizeImage_ = 48;
    constexpr static int dxImage_ = 16;
    constexpr static int dyImage_ = 6;

    constexpr static int dxText_ = dxImage_ + sizeImage_ + 12;
    constexpr static int dyText_ = 13;

    constexpr static int separatorYPadding_ = 20;
};
