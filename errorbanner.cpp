/***************************************************************************
 * Copyright (C) 2016 by Savoir-faire Linux                                *
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


#include "errorbanner.h"
#include "ui_errorbanner.h"

ErrorBanner::ErrorBanner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ErrorBanner)
{
    ui->setupUi(this);
    setAutoFillBackground(true);

    hide();
    connect(ErrorNotifier::instance(), SIGNAL(newErrorAdded()), this, SLOT(onNewError()));
}

ErrorBanner::~ErrorBanner()
{
    delete ui;
}

void
ErrorBanner::on_closeButton_clicked()
{
    if (noMoreError())
        hide();
    else
        displayError();
}

void
ErrorBanner::displayError()
{
    auto err = ErrorNotifier::instance()->getFirstError();
    ui->errorLabel->setText(err->message);
    QPalette pal(palette());
    pal.setColor(QPalette::Background, levelColor[err->level]);
    setPalette(pal);
    show();
    delete err;
}

void
ErrorBanner::onNewError()
{
    if (isVisible())
        return;
    displayError();
}

bool
ErrorBanner::noMoreError()
{
    return ErrorNotifier::instance()->getNumbersOfError() == 0;
}

