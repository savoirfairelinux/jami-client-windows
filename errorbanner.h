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

#pragma once

#include <QWidget>

#include "errornotifier.h"

namespace Ui {
class ErrorBanner;
}

static QHash<ErrorLevel, QColor> levelColor {
    {ErrorLevel::INFO, Qt::white},
    {ErrorLevel::WARNING, Qt::green},
    {ErrorLevel::CRITICAL, Qt::red}
};

class ErrorBanner : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorBanner(QWidget *parent = 0);
    ~ErrorBanner();

private slots:
    void on_closeButton_clicked();
    void onNewError();

private:
    Ui::ErrorBanner *ui;
    bool noMoreError();
    void displayError();
};

