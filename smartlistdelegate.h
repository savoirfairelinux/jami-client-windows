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

#pragma once

#include <QObject>
#include <QStyledItemDelegate>

class QPainter;

class SmartListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SmartListDelegate(QObject* parent = 0);
    void setHoveredRow(const QModelIndex& i);
    inline void setSelectedRow(const QModelIndex& i){selectedRow_ = i;};

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    constexpr static int sizeImage_ = 48;
    constexpr static int cellHeight_ = 60;
    constexpr static int cellWidth_ = 324;
    constexpr static int dy = 6;
    constexpr static int dx = 12;
    QModelIndex hoveredRow_;
    QModelIndex selectedRow_;

signals:
    void rowHovering(const QModelIndex& index, const QRect&) const;
    
};
