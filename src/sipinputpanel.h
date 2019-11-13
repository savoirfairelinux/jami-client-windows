/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include <QDialog>
#include <QSignalMapper>

namespace Ui {
class SIPInputPanelDialog;
}

class SipInputPanel : public QDialog {
    Q_OBJECT

public:
    SipInputPanel(QWidget* parent = nullptr);
    ~SipInputPanel();

signals:
    void sipInputPanelClicked(int id);
    void willClose(QMouseEvent *event);

private:
    Ui::SIPInputPanelDialog* ui;
    QSignalMapper *signalMapper;

private slots:
    void panelButtonClicked(const int&);

protected:
    void mousePressEvent(QMouseEvent *event);
};
