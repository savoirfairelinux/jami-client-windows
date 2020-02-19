/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "smartlistmodel.h"
#include "utils.h"
#include "lrcinstance.h"
#include "widgethelpers.h"

#include <QDialog>
#include <QSortFilterProxyModel>

namespace Ui {
class ContactPicker;
}

class SelectableProxyModel : public QSortFilterProxyModel
{
public:
    using FilterPredicate = std::function<bool(const QModelIndex&, const QRegExp&)>;

    explicit SelectableProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent) {
        setSourceModel(parent);
    }

    void setPredicate(FilterPredicate filterPredicate) {
        filterPredicate_ = filterPredicate;
    }

    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
        // Accept all contacts in conversation list filtered with account type, except those in a call
        auto index = sourceModel()->index(source_row, 0, source_parent);
        if (filterPredicate_) {
            return filterPredicate_(index, filterRegExp());
        }
    }

private:
    std::function<bool(const QModelIndex&, const QRegExp&)> filterPredicate_;

};

class ContactPicker : public PopupWidget
{
    Q_OBJECT;

public:
    explicit ContactPicker(QWidget *parent = 0);
    ~ContactPicker();
    void setTitle(const QString& title);
    void setType(const SmartListModel::Type& type);
    void setCurrentCalleeDisplayName(const QString& CalleeDisplayName);

signals:
    void contactWillJoinConference(const QString& contactUri);
    void callWillJoinConference(const QString& callId);
    void contactWillDoTransfer(const QString& contactUri);

protected slots:
    void accept();

private slots:
    void on_smartList_clicked(const QModelIndex &index);
    void on_ringContactLineEdit_textChanged(const QString &arg1);

private:
    Ui::ContactPicker *ui;

    std::unique_ptr<SmartListModel> smartListModel_;
    SelectableProxyModel* selectableProxyModel_;
    QString CalleeDisplayName_;

    SmartListModel::Type listModeltype_;
};
