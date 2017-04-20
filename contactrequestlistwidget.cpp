/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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

#include "contactrequestlistwidget.h"

//LRC
#include "contactrequest.h"
#include "pendingcontactrequestmodel.h"
#include "itemdataroles.h"

//CLIENT
#include "quickactcontactrequestwidget.h"
#include <QScrollBar>
#include <QAbstractItemView>
#include <QEvent>

ContactRequestListWidget::ContactRequestListWidget(QWidget *parent) :
    QTreeView(parent)
{
    // for hover detection
    setMouseTracking(true);

    //connects the entered signal to the creation and display of hovering widget
    connect(this, &QAbstractItemView::entered, [this](const QModelIndex & index) {
        //get current hovered row quick action widget
        auto widget = indexWidget(index);
        //build and add hovering quick action widget to row and display, if already built just display
        if (!widget) {
            QuickActContactRequestWidget* quickBtns = new QuickActContactRequestWidget();
            setIndexWidget(index, quickBtns);
            connect(quickBtns, &QuickActContactRequestWidget::quickValidCRBtnClicked,
                    this,
                    [=](){ model()->data(index, static_cast<int>(Ring::Role::Object)).value<ContactRequest*>()->accept(); });

            connect(quickBtns, &QuickActContactRequestWidget::quickMuteCRBtnClicked,
                    this,
                    [=](){ model()->data(index, static_cast<int>(Ring::Role::Object)).value<ContactRequest*>()->discard(); });

            connect(quickBtns, &QuickActContactRequestWidget::quickBanCRBtnClicked,
                    this,
                    [=](){ model()->data(index, static_cast<int>(Ring::Role::Object)).value<ContactRequest*>()->block(); });
        }
        else if (index.isValid()){
            indexWidget(index)->setVisible(true);
        }

        //hide previously shown hover widget (if any)
        if(hoveredCRIndex_.isValid() and indexWidget(hoveredCRIndex_))
            indexWidget(hoveredCRIndex_)->setVisible(false);

        //update current hovered index
        hoveredCRIndex_ = index;
    });
}

ContactRequestListWidget::~ContactRequestListWidget()
{
}

void
ContactRequestListWidget::setItemModel(QAbstractItemModel *model)
{
    setModel(model);

    // Hide unused columns
    for(int column = 1; column < model->columnCount(); column++){
        hideColumn(column);
    }
}


void
ContactRequestListWidget::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    verticalScrollBar()->show();
}

void
ContactRequestListWidget::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);

    hoveredCRIndex_ = QModelIndex();
    verticalScrollBar()->hide();
}

bool
ContactRequestListWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (qobject_cast<QScrollBar*>(watched) && event->type() == QEvent::Enter) {
        hoveredCRIndex_ = QModelIndex();
        return true;
    }

    return QObject::eventFilter(watched, event);
}


void
ContactRequestListWidget::drawRow(QPainter* painter,
                                  const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(index == hoveredCRIndex_ && indexWidget(hoveredCRIndex_))
        indexWidget(index)->setVisible(true);
    else if(indexWidget(index))
        indexWidget(index)->setVisible(false);

    QTreeView::drawRow(painter, option, index);
}
