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

    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStarted,
        [this]() {});
    connect(LRCInstance::renderer(), &RenderManager::previewFrameReady,
        [this]() {
            repaint();
        });
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            // frame should be null and draw black
            repaint();
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

    int previewHeight = resetPreview_ ? currentContainerHeight_ / 5 : (needToCenter_ ? currentContainerHeight_ : height());
    int previewWidth = resetPreview_ ? currentContainerWidth_ / 5 : (needToCenter_ ? currentContainerWidth_ : width());

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    if (previewImage) {
        QImage scaledPreview;
        if (photoMode_) {
            scaledPreview = Utils::getCirclePhoto(*previewImage, previewHeight);
        } else {
            scaledPreview = previewImage->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        }
        if (isroundedBoarder_) {
            scaledPreview = Utils::getRoundedEdgePhoto(scaledPreview, scaledPreview.width(), scaledPreview.height(), 10);
        }
        previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());

        QRect resetGeo(previewGeometry_);
        if (resetPreview_) {
            resetGeo.moveTo(currentContainerWidth_ - resetGeo.width() - previewMargin_,
                currentContainerHeight_ - resetGeo.height() - previewMargin_);
            resetPreview_ = false;
        } else if (needToCenter_) {
            auto centreX = (currentContainerWidth_ - resetGeo.width()) / 2;
            auto centreY = (currentContainerHeight_ - resetGeo.height()) / 2;
            resetGeo.moveTo(centreX, centreY);
        } else {
            resetGeo.moveTo(this->x(), this->y());
        }
        this->setGeometry(resetGeo);
        painter.drawImage(previewGeometry_, scaledPreview);
    } else {
        // clear to black
        paintBackgroundColor(&painter, Qt::black, photoMode_);
    }
    painter.end();
}

void
PreviewRenderWidget::paintBackgroundColor(QPainter* painter, QColor color, bool rounded)
{
    QBrush brush(color);
    QPainterPath path;
    auto r = previewGeometry_.width() / 2;
    if (rounded) {
        path.addEllipse(QPoint(r, r), r, r);
    } else {
        path.addRect(previewGeometry_);
    }
    painter->fillPath(path, brush);
}

void
PreviewRenderWidget::connectRendering()
{
    LRCInstance::renderer()->connectPreviewRendering();
}

QImage
PreviewRenderWidget::takePhoto()
{
    if (auto previewImage = LRCInstance::renderer()->getPreviewFrame()) {
        return previewImage->copy();
    }
    return QImage();
}

void
PreviewRenderWidget::setPhotoMode(bool isPhotoMode)
{
    photoMode_ = isPhotoMode;
}

void
PreviewRenderWidget::changeToRoundedBorder() {
    isroundedBoarder_ = true;
}

void
PreviewRenderWidget::resetBorder()
{
    isroundedBoarder_ = false;
}
