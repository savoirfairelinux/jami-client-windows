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

#include "bannedcontactswidget.h"
#include "ui_bannedcontactswidget.h"

#include <QAbstractItemModel>
#include <QPainter>

#include "ringthemeutils.h"

// LRC
#include "account.h"
#include "bannedcontactmodel.h"


/* Widget */

BannedContactsWidget::BannedContactsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BannedContactsWidget)
{
    ui->setupUi(this);
    bannedItemDelegate_ = new BannedContactItemDelegate();
    ui->bannedList->setItemDelegate(bannedItemDelegate_);

    connect(ui->bannedList, &QAbstractItemView::clicked, this, &BannedContactsWidget::setCurrentContact);
    connect(ui->bannedList, &QAbstractItemView::clicked, this, [this](){ui->debanStack->setCurrentWidget(ui->debanPage);});
    connect(ui->debanButton, &QPushButton::clicked, this, &BannedContactsWidget::debanContact);
}

BannedContactsWidget::~BannedContactsWidget()
{
    delete ui;
}

void BannedContactsWidget::setAccount(Account *ac)
{
    account_ = ac;

    // Configure banned list to display banned contacts
    ui->bannedList->setModel(account_->bannedContactModel());

    // deban page hidden when model changes
    ui->debanStack->setCurrentWidget(ui->cleanPage);
}

void BannedContactsWidget::setCurrentContact(const QModelIndex index)
{
    ui->bannedIdLabel->setText(index.data().toString());
    bannedContact_ = index.data(static_cast<int>(ContactMethod::Role::Object)).value<ContactMethod*>();
    qDebug() << "banned contact method = " << bannedContact_;
}

void BannedContactsWidget::debanContact()
{
    if (bannedContact_){
        account_->bannedContactModel()->remove(bannedContact_);
        ui->debanStack->setCurrentWidget(ui->cleanPage);
    } else {
        qDebug() << "banned contact: contactMethod not set!";
    }
}


/* List Item Delegate */

BannedContactItemDelegate::BannedContactItemDelegate(QObject *parent):
    QItemDelegate(parent)
{
}

void BannedContactItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    QStyleOptionViewItem opt(option);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    // First, we draw the control itself
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // Then, we print the text
    QFont font(painter->font());
    font.setPointSize(10);
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fontMetrics(font);

    QRect rectText(opt.rect);
    rectText.setLeft(opt.rect.left() + dxText_);
    rectText.setBottom(rectText.top() + dyText_ + fontMetrics.height()/2);
    rectText.setTop(rectText.bottom() - fontMetrics.height());

    QString text(index.data().toString());
    text = fontMetrics.elidedText(text, Qt::ElideRight, rectText.width());

    QPen pen(painter->pen());

    pen.setColor(RingTheme::lightBlack_);
    painter->setPen(pen);

    painter->drawText(rectText,text);

    // Draw separator when item is not selected
    if (not (opt.state & QStyle::State_Selected)) {
        QRect rect(opt.rect);
        pen.setColor(RingTheme::lightGrey_);
        painter->setPen(pen);
        painter->drawLine(rect.left() + separatorYPadding_, rect.bottom(),
                          rect.right() - separatorYPadding_,
                          rect.bottom());
    }
}

QSize BannedContactItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
