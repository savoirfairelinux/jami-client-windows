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
#include "recentmodel.h"

namespace Ui {
    class CallUtilsDialog;
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
        auto call = idx.data(static_cast<int>(Call::Role::Object)).value<Call*>();
        return call->state() != Call::State::CURRENT
                && call != CallModel::instance().selectedCall();
    }
};

class NotActiveProxyModel : public QSortFilterProxyModel
{
public:
    NotActiveProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent)
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
        return not RecentModel::instance().hasActiveCall(RecentModel::instance().peopleProxy()->mapToSource(idx));
    }
};

class CallUtilsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallUtilsDialog(QWidget* parent = 0);
    ~CallUtilsDialog();

    void setConfMode(bool active);

//UI SLOTS
protected slots:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_transferButton_clicked();
    void on_activeCallsView_doubleClicked(const QModelIndex& index);
    void on_activeCallsView_clicked(const QModelIndex& index);
    void on_currentCallButton_toggled(bool checked);
    void on_contactView_doubleClicked(const QModelIndex& index);

private:
    Ui::CallUtilsDialog* ui;
    Call* selectedCall_;
    ActiveCallsProxyModel* activeProxy_;
    NotActiveProxyModel *notActiveProxy_;
    bool confMode_;

    void removeProxyModel();
};

