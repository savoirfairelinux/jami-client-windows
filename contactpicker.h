/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include <QDialog>
#include <QSortFilterProxyModel>

#include "smartlistmodel.h"
#include "utils.h"
#include "lrcinstance.h"

namespace Ui {
class ContactPicker;
}

class ConferenceableProxyModel : public QSortFilterProxyModel
{
public:
    explicit ConferenceableProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent) {
        setSourceModel(parent);
    }

    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
        // Accept all contacts in conversation list filtered with account type, except those in a call
        auto index = sourceModel()->index(source_row, 0, source_parent);
        bool match = filterRegExp().indexIn(index.data(Qt::DisplayRole).toString()) != -1;
        auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
        auto conversation = Utils::getConversationFromUid(convUid, *LRCInstance::getCurrentConversationModel());
        return  match && !LRCInstance::getCurrentCallModel()->hasCall(conversation->callId) && !index.parent().isValid();
    }
};

class ContactPicker : public QDialog
{
    Q_OBJECT

public:
    explicit ContactPicker(QWidget *parent = 0);
    ~ContactPicker();

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void contactWillJoinConference(const std::string& callId1, const std::string& contactUri);

protected slots:
    void accept();

private slots:
    void on_smartList_clicked(const QModelIndex &index);
    void on_ringContactLineEdit_textChanged(const QString &arg1);

private:
    Ui::ContactPicker *ui;

    std::unique_ptr<SmartListModel> smartListModel_;
    ConferenceableProxyModel* conferenceeProxyModel_;

};