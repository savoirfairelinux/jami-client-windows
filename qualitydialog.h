/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
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

#include <QDialog>

class QPropertyAnimation;
class QGraphicsOpacityEffect;

namespace Ui {
class QualityDialog;
}

class QualityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QualityDialog(QWidget *parent = 0);
    ~QualityDialog();

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);

private slots:
    void on_autoCheckBox_toggled(bool checked);
    void on_qualitySlider_sliderReleased();

private:
    Ui::QualityDialog *ui;
    void setQuality();
    QPixmap* spikeMask_;
    QPropertyAnimation* fadeAnim_;
    constexpr static int fadeOverlayTime_ = 250; //msec
    QGraphicsOpacityEffect* effect_;

signals:
    void isVisible(bool visible);

};
