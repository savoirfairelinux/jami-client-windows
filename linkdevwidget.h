/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 **************************************************************************/

#pragma once
#include <QPushButton>

#include "lrcinstance.h"
#include "ui_linkdevwidget.h"


namespace Ui {
    class LinkDevWidget;
}

class LinkDevWidget : public QWidget
{
    Q_OBJECT
    LinkDevWidget(const LinkDevWidget& cpy);

public:
    explicit LinkDevWidget(QWidget* parent = nullptr);
    ~LinkDevWidget();
    inline const QPushButton* cancelBtn() const { return ui->cancelBtn; };
    inline const QPushButton* enterBtn() const { return ui->enterBtn; };
    inline const QPushButton* endCancelBtn() const { return ui->closePushButton; };


private:
    Ui::LinkDevWidget* ui;
    const int exportTimeout = 10000;

    QTimer* timeout_;

private slots:
    void setGeneratingPage();
    void setExportPage(const std::string& accountId,
        lrc::api::account::ExportOnRingStatus status, const std::string& pin);
};
