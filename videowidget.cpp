/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent)
  , previewRenderer_(nullptr)
  , renderer_(nullptr)
  , currentPreviewFrame_(nullptr)
  , currentDistantFrame_(nullptr)
{
    connect(Video::PreviewManager::instance(),
            SIGNAL(previewStarted(Video::Renderer*)),
            this, SLOT(previewStarted(Video::Renderer*)));
    connect(CallModel::instance(),
            SIGNAL(rendererAdded(Call*,Video::Renderer*)),
            this, SLOT(callInitiated(Call*, Video::Renderer*)),
            Qt::ConnectionType::DirectConnection);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

VideoWidget::~VideoWidget()
{}

void
VideoWidget::previewStarted(Video::Renderer *renderer) {
    //Enforce that only one videowidget we'll be used at the same time
    if (not isVisible())
        return;
    previewRenderer_ = renderer;
    connect(previewRenderer_, SIGNAL(frameUpdated()),
            this, SLOT(frameFromPreview()));
    connect(previewRenderer_, SIGNAL(stopped()),
            this, SLOT(previewStopped()));
}

void
VideoWidget::previewStopped() {
    disconnect(previewRenderer_, SIGNAL(frameUpdated()),
               this, SLOT(frameFromPreview()));
    disconnect(previewRenderer_, SIGNAL(stopped()),
               this, SLOT(renderingStopped()));
    previewRenderer_ = nullptr;
    if (currentPreviewFrame_) {
        delete currentPreviewFrame_;
        currentPreviewFrame_ = nullptr;
    }
}

void
VideoWidget::frameFromPreview() {
    if (previewRenderer_ && previewRenderer_->isRendering()) {
        if (currentPreviewFrame_) {
            delete currentPreviewFrame_;
            currentPreviewFrame_ = nullptr;
        }
        const QSize size(previewRenderer_->size());
        currentPreviewFrame_ = new QImage(
                    (const uchar*)previewRenderer_->currentFrame().constData(),
                    size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
        repaint();
    }
}

void
VideoWidget::paintEvent(QPaintEvent *evt) {
    Q_UNUSED(evt)
    QPainter painter(this);
    if (renderer_ && currentDistantFrame_) {
        auto scaledDistant = currentDistantFrame_->scaled(size(), Qt::KeepAspectRatio);
        auto xDiff = (width() - scaledDistant.width()) / 2;
        auto yDiff = (height() - scaledDistant.height()) /2;
        painter.drawImage(QRect(xDiff,yDiff,scaledDistant.width(),scaledDistant.height()), scaledDistant);
    }
    if (previewRenderer_ && currentPreviewFrame_) {
        auto previewHeight = !renderer_ ? height() : height()/4;
        auto previewWidth = !renderer_  ? width() : width()/4;
        auto scaledPreview = currentPreviewFrame_->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        auto xDiff = (previewWidth - scaledPreview.width()) / 2;
        auto yDiff = (previewHeight - scaledPreview.height()) / 2;
        auto yPos = !renderer_ ? yDiff : height() - previewHeight - previewMargin_;
        auto xPos = !renderer_ ? xDiff : width() - scaledPreview.width() - previewMargin_;
        painter.drawImage(QRect(xPos,yPos,scaledPreview.width(),scaledPreview.height()),
                          scaledPreview);
    }
    painter.end();
}

void
VideoWidget::callInitiated(Call* call, Video::Renderer *renderer) {
    Q_UNUSED(call)
    //Enforce that only one videowidget we'll be used at the same time
    if (not isVisible())
        return;
    renderer_ = renderer;
    connect(renderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromDistant()));
    connect(renderer_, SIGNAL(stopped()),this, SLOT(renderingStopped()),
            Qt::ConnectionType::DirectConnection);
}

void
VideoWidget::frameFromDistant() {
    if (renderer_ && renderer_->isRendering()) {
        if (currentDistantFrame_) {
            delete currentDistantFrame_;
            currentDistantFrame_ = nullptr;
        }
        const QSize size(renderer_->size());
        currentDistantFrame_ = new QImage(
                            (const uchar*)renderer_->currentFrame().constData(),
                            size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
        repaint();
    }
}

void
VideoWidget::renderingStopped() {
    disconnect(renderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromDistant()));
    disconnect(renderer_, SIGNAL(stopped()),this, SLOT(renderingStopped()));
    renderer_ = nullptr;
    if (currentDistantFrame_) {
        delete currentDistantFrame_;
        currentDistantFrame_ = nullptr;
    }
}
