/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include "currentaccountcombobox.h"
#include <accountitemdelegate.h>
#include "pixbufmanipulator.h"

#include "utils.h"
#include "ringthemeutils.h"
#include "lrcinstance.h"
#include <QPixmap>

#include "callwidget.h"
#include "ui_callwidget.h"

#undef REGISTERED

CurrentAccountComboBox::CurrentAccountComboBox(QWidget* parent)
{
    Q_UNUSED(parent);

    setMouseTracking(true);
    gearLabel_.setMouseTracking(true);

    accountListUpdate();
    accountItemDelegate_ = new AccountItemDelegate();
    this->setItemDelegate(accountItemDelegate_);

    // combobox index changed and so must the avatar
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](const int& index) {
                setCurrentIndex(index);
            });

    // account added to combobox
    connect(&LRCInstance::accountModel(),
            &lrc::api::NewAccountModel::accountAdded,
            [this](const std::string& accountId) {
                auto accountList = LRCInstance::accountModel().getAccountList();
                auto it = std::find(accountList.begin(), accountList.end(), accountId);
                if (it != accountList.end()) {
                    this->setCurrentIndex(std::distance(accountList.begin(), it));
                }
            });

    gearPixmap_.load(":/images/icons/round-settings-24px.svg");
    gearLabel_.setPixmap(gearPixmap_);
    gearLabel_.setParent(this);
    gearLabel_.setStyleSheet("background: transparent;");
    setupSettingsButton();
}

CurrentAccountComboBox::~CurrentAccountComboBox()
{
    delete accountItemDelegate_;
}

void
CurrentAccountComboBox::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPoint p(12, 2);
    QPainter painter(this);
    painter.setRenderHints((QPainter::Antialiasing | QPainter::TextAntialiasing), true);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter);

    // create box in which to draw avatar and presence indicator
    QRect avatarRect(2, 2, cellHeight_, cellHeight_); // [screen awareness]
    QRect comboBoxRect(cellHeight_ + p.x() + 2, 4, this->width() - cellHeight_ + p.x(), cellHeight_- 10); // [screen awareness]

    // define and set the two fonts
    QFont fontPrimary = painter.font();
    QFont fontSecondary = painter.font();

    fontPrimary.setPointSize(11);
    fontPrimary.setWeight(QFont::ExtraLight);
    fontSecondary.setPointSize(10);
    painter.setFont(fontPrimary);

    QFontMetrics fontMetricPrimary(fontPrimary);
    QFontMetrics fontMetricSecondary(fontSecondary);

    painter.drawPixmap(p, currentAccountAvatarImage_);

    // fill in presence indicator if account is registered
    auto accountStatus = LRCInstance::getCurrentAccountInfo().status;
    if (accountStatus == lrc::api::account::Status::REGISTERED) {
        // paint the presence indicator circle
        QPainterPath outerCircle, innerCircle;
        QPointF presenceCenter(40.0 + p.x(), 40.0);
        qreal outerCircleRadius = cellHeight_/6.5;
        qreal innerCircleRadius = outerCircleRadius - 1;
        outerCircle.addEllipse(presenceCenter, outerCircleRadius, outerCircleRadius);
        innerCircle.addEllipse(presenceCenter, innerCircleRadius, innerCircleRadius);
        painter.fillPath(outerCircle, Qt::white);
        painter.fillPath(innerCircle, RingTheme::presenceGreen_);
    }

    // write primary and secondary account identifiers to combobox label
    QString primaryAccountID = QString::fromStdString(Utils::bestNameForAccount(LRCInstance::getCurrentAccountInfo()));
    painter.setPen(Qt::black);
    primaryAccountID = fontMetricPrimary.elidedText(primaryAccountID, Qt::ElideRight,
                                                    comboBoxRect.width() - elidConst - (popupPresent ? 0 : 2 * gearSize_));
    painter.drawText(comboBoxRect, Qt::AlignLeft, primaryAccountID);

    QString secondaryAccountID = QString::fromStdString(Utils::secondBestNameForAccount(LRCInstance::getCurrentAccountInfo()));
    secondaryAccountID = fontMetricSecondary.elidedText(secondaryAccountID, Qt::ElideRight,
                                                        comboBoxRect.width() - elidConst - 2 - (popupPresent ? 0 : 2 * gearSize_)); // [screen awareness]

    if (secondaryAccountID.length()) { // if secondary accound id exists
        painter.setFont(fontSecondary);
        painter.setPen(Qt::lightGray);
        painter.drawText(comboBoxRect, (Qt::AlignBottom | Qt::AlignLeft), secondaryAccountID);
    }
}

void CurrentAccountComboBox::resizeEvent(QResizeEvent * event)
{
    setupSettingsButton();
}

void
CurrentAccountComboBox::setupSettingsButton()
{
    gearPoint_.setX(this->width() - gearSize_ - 4 * gearBorder_);
    gearPoint_.setY(this->height() / 2 - gearLabel_.height() / 2 - 2 * gearBorder_);
    gearLabel_.setGeometry(gearPoint_.x() - 3, gearPoint_.y(),
        gearSize_ + 2 * gearBorder_, gearSize_ + 2 * gearBorder_);
    gearLabel_.setMargin(gearBorder_);
}

// import account background account pixmap and scale pixmap to fit in label
void
CurrentAccountComboBox::importLabelPhoto(int index)
{
    currentAccountAvatarImage_ = accountListModel_->data(accountListModel_->index(index, 0), // [efficiency improvement]
        AccountListModel::Role::Picture).value<QPixmap>().scaledToHeight(cellHeight_ - 4, Qt::SmoothTransformation);
}

void
CurrentAccountComboBox::setCurrentIndex(int index)
{
    importLabelPhoto(index);
    QComboBox::setCurrentIndex(index);
}

void
CurrentAccountComboBox::accountListUpdate()
{
    accountListModel_.reset(new AccountListModel());
    this->setModel(accountListModel_.get());
}

// if gearLabel is clicked
void
CurrentAccountComboBox::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (!gearLabel_.frameGeometry().contains(mouseEvent->localPos().toPoint())) {
        QComboBox::mousePressEvent(mouseEvent);
    } else {
        emit settingsButtonClicked();
    }
}

// if gear label is hovered over
void
CurrentAccountComboBox::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (gearLabel_.frameGeometry().contains(mouseEvent->x(), mouseEvent->y())) {
        QComboBox::mouseMoveEvent(mouseEvent);
        gearLabel_.setStyleSheet("background: rgb(237, 237, 237); border-width: 0px; border-radius: 8px;");
        return;
    }

    gearLabel_.setStyleSheet("background: transparent;");
}

void
CurrentAccountComboBox::showPopup()
{
    gearLabel_.hide();
    popupPresent = true;
    QComboBox::showPopup();
}

void
CurrentAccountComboBox::hidePopup()
{
    gearLabel_.show();
    popupPresent = false;
    QComboBox::hidePopup();

}

void
CurrentAccountComboBox::leaveEvent(QEvent* event)
{
    gearLabel_.setStyleSheet("background: transparent;");
    QComboBox::leaveEvent(event);
}