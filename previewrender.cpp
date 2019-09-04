/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include "previewrender.h"
#include "ui_previewrender.h"

#include "lrcinstance.h"
#include "utils.h"

PreviewRenderWidget::PreviewRenderWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PreviewRenderWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(true);
    setPalette(pal);

    connect(LRCInstance::renderer(),
            &RenderManager::previewFrameReady,
            [this]() {
                repaint();
            });
    connect(LRCInstance::renderer(),
            &RenderManager::previewRenderingStopped,
            [this]() {
                // hide items here
            });
}

PreviewRenderWidget::~PreviewRenderWidget()
{
    delete ui;
}

void
PreviewRenderWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QImage scaledPreview;
    auto renderManager = LRCInstance::renderer();
    auto previewImage_ = renderManager->getPreviewFrame();
    if (!previewImage_)
        return;
    int previewHeight = resetPreview_ ? currentConainerHeight_ / 5 : (needToCentre_ ? currentConainerHeight_ : height());
    int previewWidth = resetPreview_ ? currentConainerWidth_ / 5 : (needToCentre_ ? currentConainerWidth_ : width());
    if (photoMode_) {
        scaledPreview = Utils::getCirclePhoto(*previewImage_, previewHeight);
    } else {
        scaledPreview = previewImage_->scaled(previewWidth,
            previewHeight,
            Qt::KeepAspectRatio);
    }
    if (isroundedBoarder_) {
        scaledPreview = Utils::getRoundedEdgePhoto(scaledPreview, scaledPreview.width(), scaledPreview.height(), 10);
    }
    previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());

    QRect resetGeo(previewGeometry_);
    if (resetPreview_) {
        resetGeo.moveTo(currentConainerWidth_ - resetGeo.width() - previewMargin_,
            currentConainerHeight_ - resetGeo.height() - previewMargin_);
        resetPreview_ = false;
    } else if (needToCentre_) {
        auto centreX = (currentConainerWidth_ - resetGeo.width()) / 2;
        auto centreY = (currentConainerHeight_ - resetGeo.height()) / 2;
        resetGeo.moveTo(centreX, centreY);
    } else {
        resetGeo.moveTo(this->x(), this->y());
    }
    this->setGeometry(resetGeo);
    //painter.drawPixmap(previewGeometry_, scaledPreview);
    painter.drawImage(previewGeometry_, scaledPreview);

    /*if (previewRenderer_ || (photoMode_ && hasFrame_)) {
        QMutexLocker lock(&mutex_);
        if (previewRenderer_
            && previewFrame_.storage.size() != 0
            && previewFrame_.storage.size() == (unsigned int)(previewRenderer_->size().height() * previewRenderer_->size().width() * 4)) {
            framePreview_ = std::move(previewFrame_.storage);
            previewImage_.reset(
                new QImage((uchar*)framePreview_.data(),
                    previewRenderer_->size().width(),
                    previewRenderer_->size().height(),
                    QImage::Format_ARGB32_Premultiplied));
            hasFrame_ = true;
            if (triggerResetPreviewAfterImageReloaded_) {
                resetPreview_ = true;
                triggerResetPreviewAfterImageReloaded_ = false;
            }
        } else {
            hasFrame_ = false;
        }
        if (previewImage_) {
            QImage scaledPreview;
            int previewHeight = resetPreview_ ? currentConainerHeight_ / 5 : (needToCentre_ ? currentConainerHeight_ : height());
            int previewWidth = resetPreview_ ? currentConainerWidth_ / 5 : (needToCentre_ ? currentConainerWidth_ : width());
            if (photoMode_) {
                scaledPreview = Utils::getCirclePhoto(*previewImage_, previewHeight);
            } else {
                scaledPreview = previewImage_->scaled(previewWidth,
                                                      previewHeight,
                                                      Qt::KeepAspectRatio);
            }
            if (isroundedBoarder_) {
                scaledPreview = Utils::getRoundedEdgePhoto(scaledPreview, scaledPreview.width(), scaledPreview.height(), 10);
            }
            previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());

            QRect resetGeo(previewGeometry_);
            if (resetPreview_) {
                resetGeo.moveTo(currentConainerWidth_ - resetGeo.width() - previewMargin_,
                                currentConainerHeight_ - resetGeo.height() - previewMargin_);
                resetPreview_ = false;
            } else if (needToCentre_) {
                auto centreX = (currentConainerWidth_ - resetGeo.width()) / 2;
                auto centreY = (currentConainerHeight_ - resetGeo.height()) / 2;
                resetGeo.moveTo(centreX, centreY);
            } else {
                resetGeo.moveTo(this->x(), this->y());
            }
            this->setGeometry(resetGeo);
            painter.drawImage(previewGeometry_, scaledPreview);
        }
    }*/
    painter.end();
}

void
PreviewRenderWidget::connectRendering()
{
    LRCInstance::renderer()->connectPreviewRendering();
}

QImage
PreviewRenderWidget::takePhoto()
{
    if (previewImage_) {
        return previewImage_.get()->copy();
    }
    return QImage();
}

void
PreviewRenderWidget::setPhotoMode(bool isPhotoMode)
{
    photoMode_ = isPhotoMode;
}

void
PreviewRenderWidget::changeToRoundedBoarder() {
    isroundedBoarder_ = true;
}

void
PreviewRenderWidget::resetBoarder()
{
    isroundedBoarder_ = false;
}
