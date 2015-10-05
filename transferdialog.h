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

#include <QDialog>
#include <QSortFilterProxyModel>

#include "callmodel.h"

namespace Ui {
class TransferDialog;
}

class ActiveCallsProxyModel : public QSortFilterProxyModel
{
public:
    ActiveCallsProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent)
    {
        setSourceModel(parent);
    }
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
    {
        if (not sourceModel() || source_parent.isValid())
            return false;
        auto idx = sourceModel()->index(source_row,0,source_parent);
        if (not idx.isValid())
            return false;
        return idx.data(static_cast<int>(Call::Role::State))
                .value<Call::State>() != Call::State::CURRENT;
    }
};


class TransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferDialog(QWidget *parent = 0);
    ~TransferDialog();

//UI SLOTS
protected slots:
    void showEvent(QShowEvent *event);
private slots:
    void on_transferButton_clicked();
    void on_activeCallsView_doubleClicked(const QModelIndex &index);
    void on_activeCallsView_clicked(const QModelIndex &index);

private:
    Ui::TransferDialog *ui;
    Call *selectedCall_;
    ActiveCallsProxyModel *activeProxy_;

    void removeProxyModel();
};

