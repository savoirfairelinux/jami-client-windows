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

#pragma once

#include <QDialog>

namespace Ui {
class PopupDialog;
}

class PopupDialog : public QDialog
{
    Q_OBJECT

public:
    enum class SpikeLabelAlignment
    {
        AlignLeft,
        AlignHCenter,
        AlignRight
    };

    explicit PopupDialog(QWidget *parent = nullptr,
                         QColor spikeColor = Qt::white,
                         SpikeLabelAlignment spikeAlignment = SpikeLabelAlignment::AlignLeft);
    ~PopupDialog();

    void insertWidget(QWidget *widget);

signals:
    void willClose(QMouseEvent* event);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Ui::PopupDialog *ui;
    QColor spikeColor_;
    constexpr static qreal cornerRadius_ = 10.0f;

    void setSpikeLabelAlignment(SpikeLabelAlignment spikeAlignment);
};
