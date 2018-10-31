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
#include <QTextDocument>
#include <QItemDelegate>

enum class MsgSeq;

class ImDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ImDelegate(QObject *parent = 0);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const override;

private:
    void formatMsg(const QModelIndex& index, QString& msg) const;
    QRect getBoundingRect(const Qt::AlignmentFlag& dir, const QStyleOptionViewItem &option,
        QTextDocument& txtDoc) const;

    void paintBubble(QPainter* painter, const QRect& rect, const MsgSeq& msgseq, bool outgoing) const;

    //const QFont fontMsg_ = QFont("Arial", 10);
    const QString defaultStylesheet_ = QString("p { color: '#0f0f0f'; opacity: 100; font-size : 16px; text-align : left; }");
    constexpr static int sizeImage_ = 24;
    constexpr static int margin_ = 5;
    constexpr static int padding_ = 5;
    constexpr static int bubbleRadius_ = 12;
};
