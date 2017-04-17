/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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

#include <QDialog>
#include <QSortFilterProxyModel>

#include "personmodel.h"
#include "contactmethod.h"

namespace Ui {
class ContactPicker;
}

class OnlyPersonProxyModel : public QSortFilterProxyModel
{
public:
    explicit OnlyPersonProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent)
    {
        setSourceModel(parent);
    }
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
    {
        bool match = filterRegExp().indexIn(sourceModel()->index(source_row,0,source_parent).data(Qt::DisplayRole).toString()) != -1;
        return match && !sourceModel()->index(source_row,0,source_parent).parent().isValid();
    }
};

class ContactPicker : public QDialog
{
    Q_OBJECT

public:
    explicit ContactPicker(ContactMethod* number, QWidget *parent = 0);
    ~ContactPicker();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
protected slots:
     void accept();

//UI SLOTS
private slots:
    void on_contactView_doubleClicked(const QModelIndex &index);
    void on_okButton_clicked();
    void on_createNewButton_clicked();

    void on_searchBar_textChanged(const QString &arg1);

private:
    Ui::ContactPicker *ui;
    Person *personSelected_;
    ContactMethod* number_;
    OnlyPersonProxyModel* contactProxyModel_;
    QPoint mpos_;
};
