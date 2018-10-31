/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

#include <QWidget>
#include <QItemDelegate>

namespace Ui {
class SelectableMessageWidget;
}

class SelectableMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SelectableMessageWidget(  QModelIndex index,
                                        QWidget* parent = 0);
    ~SelectableMessageWidget();

    bool isFreeable();

protected:
    void leaveEvent(QEvent* event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent  *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent* e);

private:
    Ui::SelectableMessageWidget* ui;

    QWidget* parent_;
    QModelIndex index_;
    QString msg_;
    bool isFreeable_;

private slots:
    void slotStartDrag();
    void slotEndDrag();

signals:
    void exited() const;

};