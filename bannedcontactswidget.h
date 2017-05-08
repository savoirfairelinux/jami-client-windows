/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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


#ifndef BANNEDCONTACTSWIDGET_H
#define BANNEDCONTACTSWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QObject>
#include <QItemDelegate>

// LRC
#include "account.h"
#include "contactmethod.h"

/* item delegate */

class BannedContactItemDelegate : public QItemDelegate
{
public:
    BannedContactItemDelegate(QObject* parent = 0);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    constexpr static int cellHeight_ = 60;
    constexpr static int sizeImage_ = 48;
    constexpr static int dxImage_ = 16;
    constexpr static int dyImage_ = 6;

    constexpr static int dxText_ = dxImage_;
    constexpr static int dyText_ = cellHeight_/2;

    constexpr static int separatorYPadding_ = 20;
};

/* widget */
namespace Ui {
class BannedContactsWidget;
}

class BannedContactsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BannedContactsWidget(QWidget *parent = 0);
    ~BannedContactsWidget();
    void setAccount(Account *ac);

private slots:
    void setCurrentContact(const QModelIndex index);
    void debanContact();

private:
    Account* account_ {};
    BannedContactItemDelegate* bannedItemDelegate_;
    ContactMethod* bannedContact_;
    Ui::BannedContactsWidget *ui;
};
#endif // BANNEDCONTACTSWIDGET_H
