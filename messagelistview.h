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

#include <QTreeView>
#include <QDebug>

#include "selectablemessagewidget.h"

class MessageListView : public QTreeView
{
    Q_OBJECT
public:
    explicit MessageListView(QWidget* parent = 0);
    ~MessageListView();

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);

private:
    void removeMsgWidgets();
    void removeMsgWidgetTracking(int row);
    void removeMsgWidget(const QModelIndex& index);
    void setRemoveOnExitPolicy(SelectableMessageWidget* msgWidget, const QModelIndex & index);
    std::vector<int> disposeBag_;

};
