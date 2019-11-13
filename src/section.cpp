/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include "section.h"
#include "ui_section.h"

SectionWidget::SectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SectionWidget)
{
    ui->setupUi(this);

    // let the entire widget grow and shrink with its content
    animationGroup_.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    animationGroup_.addAnimation(new QPropertyAnimation(this, "minimumHeight"));

    connect(ui->expandCollapseCheckBox, &QCheckBox::stateChanged, this, &SectionWidget::expandCollapseButtonToggled);
    connect(&animationGroup_, &QParallelAnimationGroup::finished,
        [this] {
            if (animationGroup_.direction() == QAbstractAnimation::Backward) {
                currentExpandwidget_->hide();
            }
        });
}

SectionWidget::~SectionWidget()
{
    delete ui;
}

void
SectionWidget::setLabelText(const QString& label)
{
    ui->expandCollapseCheckBox->setText(label);
}

void
SectionWidget::addExpandWidget(QWidget *widget)
{
    currentExpandwidget_ = widget;
    ui->verticalLayout->addWidget(widget);

    // section itself min = max, minimum expanding size policy, or preferred
    // added widget minimum height should be 0, minimum expanding size policy

    const auto collapsedHeight = minimumHeight();
    auto contentHeight = widget->maximumHeight();
    for (int i = 0; i < animationGroup_.animationCount() - 1; ++i) {
        QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(animationGroup_.animationAt(i));
        spoilerAnimation->setDuration(animationDuration_);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }

    // make sure that currentExpandwidget does not lay on the expandCollapseButton
    currentExpandwidget_->hide();
}

void
SectionWidget::expandCollapseButtonToggled(bool state)
{
    //QAbstractAnimation::Backward
    //The current time of the animation decreases with time (i.e., moves from the end / duration and towards 0).

    if (state) {
        currentExpandwidget_->show();
    }
    animationGroup_.setDirection(state ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    animationGroup_.start();
}
