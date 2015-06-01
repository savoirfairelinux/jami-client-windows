/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#include <QDebug>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent)
  , previewRenderer_(nullptr)
  , renderer_(nullptr)
  , previewFrame_(nullptr)
  , distantFrame_(nullptr)
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
    previewRenderer_ = renderer;
    connect(previewRenderer_, SIGNAL(frameUpdated()),
            this, SLOT(frameFromPreview()));
    connect(previewRenderer_, SIGNAL(stopped()),
            this, SLOT(previewStopped()));
}

void
VideoWidget::previewStopped() {
    QMutexLocker {&lock_};
    disconnect(previewRenderer_, SIGNAL(frameUpdated()),
            this, SLOT(frameFromPreview()));
    disconnect(previewRenderer_, SIGNAL(stopped()),
            this, SLOT(renderingStopped()));
    previewRenderer_ = nullptr;
}

void
VideoWidget::frameFromPreview() {
    if (previewFrame_) {
        delete previewFrame_;
        previewFrame_ = nullptr;
    }
    if (previewRenderer_ && previewRenderer_->isRendering()) {
        const QSize size(previewRenderer_->size());
        previewFrame_ = new QImage(
                    (const uchar*)previewRenderer_->currentFrame().constData(),
                    size.width(), size.height(), QImage::Format_RGBA8888);
        update();
    }
}

void
VideoWidget::paintEvent(QPaintEvent* evt) {
    Q_UNUSED(evt)
    QMutexLocker {&lock_};
    QPainter painter(this);
    //painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);
    if (distantFrame_ && renderer_ && renderer_->isRendering()) {
        auto scaledDistant = distantFrame_->scaled(size(), Qt::KeepAspectRatio);
        auto xDiff = (width() - scaledDistant.width()) / 2;
        painter.drawImage(QRect(xDiff,0,scaledDistant.width(),scaledDistant.height()), scaledDistant);
    }
    if (previewFrame_ && previewRenderer_ && previewRenderer_->isRendering()) {
        auto previewHeight = !renderer_ ? height() : height()/4;
        auto previewWidth = !renderer_  ? width() : width()/4;
        auto scaledPreview = previewFrame_->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        auto xDiff = (previewWidth - scaledPreview.width()) / 2;
        auto yPos = !renderer_ ? 0 : height() - previewHeight - previewMargin_;
        auto xPos = !renderer_ ? xDiff : width() - scaledPreview.width() - previewMargin_;
        painter.drawImage(QRect(xPos,yPos,scaledPreview.width(),scaledPreview.height()),
                          scaledPreview);
    }
    painter.end();
}

void
VideoWidget::callInitiated(Call* call, Video::Renderer *renderer) {
    Q_UNUSED(call)
    renderer_ = renderer;
    connect(renderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromDistant()));
    connect(renderer_, SIGNAL(stopped()),this, SLOT(renderingStopped()),
            Qt::ConnectionType::DirectConnection);
}

void
VideoWidget::frameFromDistant() {
    if (distantFrame_) {
        delete distantFrame_;
        distantFrame_ = nullptr;
    }
    if (renderer_) {
        const QSize size(renderer_->size());
        distantFrame_ = new QImage(
                    (const uchar*) renderer_->currentFrame().constData(),
                    size.width(), size.height(), QImage::Format_RGBA8888);
        update();
    }
}

void
VideoWidget::renderingStopped() {
    QMutexLocker {&lock_};
    if (distantFrame_) {
        delete distantFrame_;
        distantFrame_ = nullptr;
    }
    disconnect(renderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromDistant()));
    disconnect(renderer_, SIGNAL(stopped()),this, SLOT(renderingStopped()));
    renderer_ = nullptr;
}

void
VideoWidget::mouseDoubleClickEvent(QMouseEvent *e) {
    QWidget::mouseDoubleClickEvent(e);
    if(isFullScreen()) {
        this->setParent(oldParent_);
        this->showNormal();
        this->resize(oldSize_.width(), oldSize_.height());
    } else {
        oldSize_ = this->size();
        oldParent_ = static_cast<QWidget*>(this->parent());
        this->setParent(0);
        this->showFullScreen();
        this->show();
    }
}
