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

    //make a button where text is at centre, label at left
    /*ui->expandCollapseButton->setLayout(new QVBoxLayout);
    textLabel_ = new QLabel(ui->expandCollapseButton);
    textLabel_->setText("Section");
    textLabel_->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
    textLabel_->setStyleSheet("QLabel{ text-align:right; }");
    ui->expandCollapseButton->layout()->addWidget(textLabel_);*/

    // start out collapsed
    contentArea_.setMaximumHeight(0);
    contentArea_.setMinimumHeight(0);
    // let the entire widget grow and shrink with its content
    animationGroup_.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    animationGroup_.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    animationGroup_.addAnimation(new QPropertyAnimation(&contentArea_, "maximumHeight"));

    connect(ui->expandCollapseButton, &QAbstractButton::clicked, this, &SectionWidget::expandCollapseButtonClicked);
}

SectionWidget::~SectionWidget()
{
    delete ui;
}

void
SectionWidget::setSectionText(const QString& label)
{
    textLabel_->setText(label);
}

void
SectionWidget::addExpandWidget(QWidget *widget)
{
    ui->verticalLayout->addWidget(widget);
}

void
SectionWidget::expandCollapseButtonClicked()
{

}
