/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include <QPainter>
#include <QStyledItemDelegate>

class ImDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ImDelegate(QObject *parent = 0);
    enum DisplayOptions {
        AUTHOR = 1,
        DATE
    };

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
    void formatMsg(const QModelIndex& index, QString& msg) const;
    QRect getBoundingRect(const Qt::AlignmentFlag& dir, const QString& msg, const QStyleOptionViewItem &option) const;

    const QFont fontMsg_ = QFont("Arial", 10);
    const QSize iconSize_ {38,38};

    constexpr static int padding_ = 5;
};

