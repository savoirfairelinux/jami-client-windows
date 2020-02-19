/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  *
 **************************************************************************/

#pragma once
#include <QPushButton>
#include <QDialog>

#include "lrcinstance.h"

namespace Ui {
    class LinkDeviceDialog;
}

class LinkDeviceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LinkDeviceDialog(QWidget* parent = nullptr);
    ~LinkDeviceDialog();

private:
    Ui::LinkDeviceDialog* ui;
    const int exportTimeout_ = 20000;

    QTimer* timeout_;

private slots:
    void setGeneratingPage();
    void setExportPage(const QString& accountId,
                       lrc::api::account::ExportOnRingStatus status,
                       const QString& pin);
};
