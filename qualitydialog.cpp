/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Stepan Salenikovich <stepan.salenikovich@savoirfairelinux.com>  *
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

#include "qualitydialog.h"
#include "ui_qualitydialog.h"

#include <QSortFilterProxyModel>
#include <QBitmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include "callmodel.h"
#include "account.h"
#include "codecmodel.h"

QualityDialog::QualityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QualityDialog),
    spikeMask_(new QPixmap(":/images/spikeMask.png"))
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);

    ui->spike->setMask(spikeMask_->mask());

    effect_ = new QGraphicsOpacityEffect(this);
    effect_->setOpacity(1.0);
    setGraphicsEffect(effect_);
    fadeAnim_ = new QPropertyAnimation(effect_, "opacity");
    fadeAnim_->setDuration(fadeOverlayTime_);
    fadeAnim_->setStartValue(0.0);
    fadeAnim_->setEndValue(1.0);
    fadeAnim_->setEasingCurve(QEasingCurve::InExpo);
}

QualityDialog::~QualityDialog()
{
    delete effect_;
    delete spikeMask_;
    delete fadeAnim_;
    delete ui;
}

void
QualityDialog::on_autoCheckBox_toggled(bool checked)
{
    setQuality();
    ui->qualitySlider->setEnabled(not checked);
}


void
QualityDialog::on_qualitySlider_sliderReleased()
{
    setQuality();
}

void QualityDialog::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    ui->autoCheckBox->blockSignals(true);
    const auto& call = CallModel::instance().selectedCall();
    if (const auto& codecModel = call->account()->codecModel()) {
        const auto& videoCodecs = codecModel->videoCodecs();
        if (videoCodecs->rowCount() > 0) {
            /* we only need to check the first codec since by default it is ON for all, and the
             *  client sets its ON or OFF for all codecs as well */
            const auto& idx = videoCodecs->index(0,0);
            auto auto_quality_enabled = idx.data(static_cast<int>(CodecModel::Role::AUTO_QUALITY_ENABLED)).toString() == "true";
            ui->autoCheckBox->setChecked(auto_quality_enabled);
            ui->qualitySlider->setEnabled(not auto_quality_enabled);

            // TODO: save the manual quality setting in the client and set the slider to that value here;
            //       the daemon resets the bitrate/quality between each call, and the default may be
            //       different for each codec, so there is no reason to check it here
        }
    }
    ui->autoCheckBox->blockSignals(false);

    emit(isVisible(true));

    fadeAnim_->setDirection(QAbstractAnimation::Forward);
    fadeAnim_->start();
}

void
QualityDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
    emit(isVisible(false));
}

void
QualityDialog::setQuality()
{
    /* set auto quality true or false, also set the bitrate and quality values;
     * the slider is from 0 to 100, use the min and max vals to scale each value accordingly */
    const auto& call = CallModel::instance().selectedCall();
    if (const auto& codecModel = call->account()->codecModel()) {
        const auto& videoCodecs = codecModel->videoCodecs();

        for (int i=0; i < videoCodecs->rowCount();i++) {
            const auto& idx = videoCodecs->index(i,0);

            if (ui->autoCheckBox->isChecked()) {
                videoCodecs->setData(idx, "true", CodecModel::Role::AUTO_QUALITY_ENABLED);
            } else {
                auto min_bitrate = idx.data(static_cast<int>(CodecModel::Role::MIN_BITRATE)).toInt();
                auto max_bitrate = idx.data(static_cast<int>(CodecModel::Role::MAX_BITRATE)).toInt();
                auto min_quality = idx.data(static_cast<int>(CodecModel::Role::MIN_QUALITY)).toInt();
                auto max_quality = idx.data(static_cast<int>(CodecModel::Role::MAX_QUALITY)).toInt();

                double bitrate;
                bitrate = min_bitrate + (double)(max_bitrate - min_bitrate)*(ui->qualitySlider->value()/100.0);
                if (bitrate < 0) bitrate = 0;

                double quality;
                // note: a lower value means higher quality
                quality = (double)min_quality - (min_quality - max_quality)*(ui->qualitySlider->value()/100.0);
                if (quality < 0) quality = 0;

                videoCodecs->setData(idx, "false", CodecModel::Role::AUTO_QUALITY_ENABLED);
                videoCodecs->setData(idx, QString::number((int)bitrate), CodecModel::Role::BITRATE);
                videoCodecs->setData(idx, QString::number((int)quality), CodecModel::Role::QUALITY);
            }
        }
        codecModel << CodecModel::EditAction::SAVE;
    }
}
