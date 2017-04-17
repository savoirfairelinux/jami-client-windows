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

#include <QDialog>
#include <QSortFilterProxyModel>

#include "callmodel.h"
#include "recentmodel.h"

#include "smartlistdelegate.h"

class QPropertyAnimation;
class QGraphicsOpacityEffect;

namespace Ui {
    class CallUtilsDialog;
}

class NotCurrentProxyModel : public QSortFilterProxyModel
{
public:
    explicit NotCurrentProxyModel(QAbstractItemModel* parent) : QSortFilterProxyModel(parent)
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
        auto call = RecentModel::instance().getActiveCall(idx);
        return not call || not (call->state() == Call::State::CURRENT);
    }
};

class CallUtilsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallUtilsDialog(QWidget* parent = 0);
    ~CallUtilsDialog();

    void setConfMode(bool active);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

//UI SLOTS
protected slots:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);
private slots:
    void on_doTransferButton_clicked();
    void on_contactView_doubleClicked(const QModelIndex& index);

private:
    Ui::CallUtilsDialog* ui;
    bool confMode_;
    SmartListDelegate* smartListDelegate_;
    NotCurrentProxyModel* notCurrentProxyModel_;
    QPixmap* spikeMask_;
    QPropertyAnimation* fadeAnim_;
    constexpr static int fadeOverlayTime_ = 250; //msec
    QGraphicsOpacityEffect* effect_;

    void removeProxyModel();

signals:
    void isVisible(bool visible);

};

