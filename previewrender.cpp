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
    , ui(new Ui::previewRenderWidget)
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
}

PreviewRenderWidget::~PreviewRenderWidget()
{
    delete ui;
}

void
PreviewRenderWidget::slotPreviewRendererStarted(const std::string& id)
{
    Q_UNUSED(id);

    QObject::disconnect(previewRendererConnections_.started);

    this->show();

    QObject::disconnect(previewRendererConnections_.updated);
    previewRendererConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        this,
        &PreviewRenderWidget::slotUpdatePreview);

    QObject::disconnect(previewRendererConnections_.stopped);
    previewRendererConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        this,
        &PreviewRenderWidget::slotStopPreview);
}

void
PreviewRenderWidget::slotUpdatePreview(const std::string& id)
{
    if (id != lrc::api::video::PREVIEW_RENDERER_ID) {
        return;
    }
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    previewRenderer_ = const_cast<lrc::api::video::Renderer*>(renderer);
    renderFrame(id);
}

void
PreviewRenderWidget::slotStopPreview(const std::string& id)
{
    Q_UNUSED(id);
    QObject::disconnect(previewRendererConnections_.updated);
    QObject::disconnect(previewRendererConnections_.stopped);
    previewRenderer_ = nullptr;
    repaint();
}

void
PreviewRenderWidget::renderFrame(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    using namespace lrc::api::video;
    auto renderer = &avModel->getRenderer(id);
    if (renderer && renderer->isRendering()) {
        {
            QMutexLocker lock(&mutex_);
            auto tmp = renderer->currentFrame();
            if (tmp.storage.size()) {
                previewFrame_ = tmp;
            }
        }
        update();
    }
}

void
PreviewRenderWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);

    if (previewRenderer_ || (photoMode_ && hasFrame_)) {
        QMutexLocker lock(&mutex_);
        if (previewFrame_.storage.size() != 0
            && previewFrame_.storage.size() == (unsigned int)(previewRenderer_->size().height() * previewRenderer_->size().width() * 4)) {
            framePreview_ = std::move(previewFrame_.storage);
            previewImage_.reset(
                new QImage((uchar*)framePreview_.data(),
                    previewRenderer_->size().width(),
                    previewRenderer_->size().height(),
                    QImage::Format_ARGB32_Premultiplied));
            hasFrame_ = true;
        }
        if (previewImage_) {
            QImage scaledPreview;
            int previewHeight = resetPreview_ ? videoViewHeight_ / 5 : height();
            int previewWidth = resetPreview_ ? videoViewWidth_ / 5 : width();
            if (photoMode_) {
                scaledPreview = Utils::getCirclePhoto(*previewImage_, previewHeight);
            } else {
                scaledPreview = previewImage_->scaled(previewWidth,
                                                      previewHeight,
                                                      Qt::KeepAspectRatio);
            }
            if (isroundedBoarder_) {
                scaledPreview = Utils::getRoundedEdgePhoto(scaledPreview, scaledPreview.width(), scaledPreview.height(), 25);
            }
            previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());

            QRect resetGeo(previewGeometry_);
            if (resetPreview_) {
                resetGeo.moveTo(videoViewWidth_ - resetGeo.width() - previewMargin_,
                                videoViewHeight_ - resetGeo.height() - previewMargin_);
                resetPreview_ = false;
            } else {
                resetGeo.moveTo(this->x(), this->y());
            }
            this->setGeometry(resetGeo);
            painter.drawImage(previewGeometry_, scaledPreview);
        }
    }
    painter.end();
}

void
PreviewRenderWidget::connectRendering()
{
    previewRendererConnections_.started = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStarted,
        this,
        &PreviewRenderWidget::slotPreviewRendererStarted);
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
PreviewRenderWidget::disconnectRendering()
{
    QObject::disconnect(previewRendererConnections_.started);
    QObject::disconnect(previewRendererConnections_.stopped);
    QObject::disconnect(previewRendererConnections_.updated);
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
