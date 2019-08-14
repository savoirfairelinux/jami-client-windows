/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "videowidget.h"

#include "utils.h"

VideoWidget::VideoWidget(QWidget* parent) :
    QWidget(parent)
  , isPreviewDisplayed_(true)
  , fullPreview_(false)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    previewPlace_ = bottomRight;
}

VideoWidget::~VideoWidget()
{}

void
VideoWidget::slotRendererStarted(const std::string& id)
{
    Q_UNUSED(id);

    QObject::disconnect(rendererConnections_.started);

    this->show();

    resetPreview_ = true;

    QObject::disconnect(rendererConnections_.updated);
    rendererConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        this,
        &VideoWidget::slotUpdateFullView);

    QObject::disconnect(rendererConnections_.stopped);
    rendererConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        this,
        &VideoWidget::slotStopFullView);
}

void
VideoWidget::renderFrame(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    using namespace lrc::api::video;
    auto renderer = &avModel->getRenderer(id);
    if (renderer && renderer->isRendering()) {
        {
            QMutexLocker lock(&mutex_);
            auto tmp  = renderer->currentFrame();
            if (tmp.storage.size()) {
                using namespace lrc::api::video;
                if (id == PREVIEW_RENDERER_ID) {
                    previewFrame_ = tmp;
                } else {
                    distantFrame_ = tmp;
                }
            }
        }
        update();
    }
}

void
VideoWidget::slotToggleFullScreenClicked()
{
    this->update();
}

void
VideoWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);

    if (distantRenderer_) {
        {
            QMutexLocker lock(&mutex_);
            if (distantFrame_.storage.size() != 0
                && distantFrame_.storage.size() ==
                (unsigned int)(distantRenderer_->size().height() * distantRenderer_->size().width()*4)) {
                frameDistant_ = std::move(distantFrame_.storage);
                distantImage_.reset(
                    new QImage((uchar*)frameDistant_.data(),
                        distantRenderer_->size().width(),
                        distantRenderer_->size().height(),
                        QImage::Format_ARGB32_Premultiplied)
                );
            }
        }
        if (distantImage_) {
            auto scaledDistant = distantImage_->scaled(size(), Qt::KeepAspectRatio);
            auto xDiff = (width() - scaledDistant.width()) / 2;
            auto yDiff = (height() - scaledDistant.height()) / 2;
            painter.drawImage(QRect(
                xDiff, yDiff, scaledDistant.width(), scaledDistant.height()), scaledDistant
            );
        }
    }
    if ((previewRenderer_ && isPreviewDisplayed_) || (photoMode_ && hasFrame_)) {
        QMutexLocker lock(&mutex_);
        if (previewFrame_.storage.size() != 0
            && previewFrame_.storage.size() ==
            (unsigned int)(previewRenderer_->size().height() * previewRenderer_->size().width() * 4)) {
            framePreview_ = std::move(previewFrame_.storage);
            previewImage_.reset(
                new QImage((uchar*)framePreview_.data(),
                    previewRenderer_->size().width(),
                    previewRenderer_->size().height(),
                    QImage::Format_ARGB32_Premultiplied)
            );
            hasFrame_ = true;
        }
        if (previewImage_) {
            if (resetPreview_) {
                auto previewHeight = fullPreview_ ? height() : height() / 6;
                auto previewWidth = fullPreview_ ? width() : width() / 6;
                QImage scaledPreview;
                if (photoMode_)
                    scaledPreview = Utils::getCirclePhoto(*previewImage_, previewHeight);
                else
                    scaledPreview = previewImage_->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
                auto xDiff = (previewWidth - scaledPreview.width()) / 2;
                auto yDiff = (previewHeight - scaledPreview.height()) / 2;
                auto xPos = fullPreview_ ? xDiff : width() - scaledPreview.width() - previewMargin_;
                auto yPos = fullPreview_ ? yDiff : height() - scaledPreview.height() - previewMargin_;
                previewGeometry_.setRect(xPos, yPos, scaledPreview.width(), scaledPreview.height());
                updatePreviewPos();
                if (!fullPreview_) {
                    QBrush brush(scaledPreview);
                    brush.setTransform(QTransform::fromTranslate(previewGeometry_.x(), previewGeometry_.y()));
                    QPainterPath previewPath;
                    previewPath.addRoundRect(previewGeometry_, 25);
                    painter.fillPath(previewPath, brush);
                }
                resetPreview_ = false;
            }

            QImage scaledPreview;
            if (photoMode_) {
                scaledPreview = Utils::getCirclePhoto(*previewImage_, previewGeometry_.height());
            } else {
                scaledPreview = previewImage_->scaled(previewGeometry_.width(),
                                                      previewGeometry_.height(),
                                                      Qt::KeepAspectRatio);
            }
            previewGeometry_.setWidth(scaledPreview.width());
            previewGeometry_.setHeight(scaledPreview.height());
            if (!fullPreview_){
                QBrush brush(scaledPreview);
                brush.setTransform(QTransform::fromTranslate(previewGeometry_.x(), previewGeometry_.y()));
                QPainterPath previewPath;
                previewPath.addRoundRect(previewGeometry_, 25);
                painter.fillPath(previewPath, brush);
            } else {
                painter.drawImage(previewGeometry_, scaledPreview);
            }
        }
    } else if (photoMode_) {
        paintBackgroundColor(&painter, Qt::black);
    }
    painter.end();
}

void
VideoWidget::paintBackgroundColor(QPainter* painter, QColor color)
{
    QImage black(1, 1, QImage::Format_ARGB32);
    black.fill(color);
    QImage scaledPreview = Utils::getCirclePhoto(black, height());
    previewGeometry_.setWidth(scaledPreview.width());
    previewGeometry_.setHeight(scaledPreview.height());
    painter->drawImage(previewGeometry_, scaledPreview);
}

void
VideoWidget::updatePreviewPos()
{
    if (fullPreview_)
        return;
    switch (previewPlace_) {
    case topRight:
        previewGeometry_.moveTopRight(QPoint(width() - previewMargin_, previewMargin_));
        break;
    case topLeft:
        previewGeometry_.moveTopLeft(QPoint(previewMargin_, previewMargin_));
        break;
    case bottomRight:
        previewGeometry_.moveBottomRight(QPoint(width() - previewMargin_, height() - previewMargin_));
        break;
    case bottomLeft:
        previewGeometry_.moveBottomLeft(QPoint(previewMargin_, height() - previewMargin_));
        break;
    case top:
        previewGeometry_.moveTop(previewMargin_);
        break;
    case right:
        previewGeometry_.moveRight(previewMargin_);
        break;
    case bottom:
        previewGeometry_.moveBottom(previewMargin_);
        break;
    case left:
        previewGeometry_.moveLeft(previewMargin_);
        break;

    default:
        break;
    }
}

void
VideoWidget::movePreview(TargetPointPreview typeOfMove)
{
    resetPreview();
    switch (typeOfMove)
    {
    case topRight:
        previewPlace_ = topRight;
        break;
    case topLeft:
        previewPlace_ = topLeft;
        break;
    case bottomRight:
        previewPlace_ = bottomRight;
        break;
    case bottomLeft:
        previewPlace_ = bottomLeft;
        break;
    case top:
        previewPlace_ = top;
        break;
    case right:
        previewPlace_ = right;
        break;
    case bottom:
        previewPlace_ = bottom;
        break;
    case left:
        previewPlace_ = left;
        break;

        default:
        break;

    }
}

void
VideoWidget::connectRendering()
{
    rendererConnections_.started = connect(
        &LRCInstance::avModel(),
        SIGNAL(rendererStarted(const std::string&)),
        this,
        SLOT(slotRendererStarted(const std::string&))
    );
}

void
VideoWidget::connectPreviewOnlyRendering()
{
    rendererConnections_.started = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStarted,
        [this]() {
            this->rendererStartedWithoutDistantRender();
        });
}

void
VideoWidget::setPreviewDisplay(bool display)
{
    isPreviewDisplayed_ = display;
}

void
VideoWidget::setIsFullPreview(bool full)
{
    fullPreview_ = full;
}

QImage
VideoWidget::takePhoto()
{
    if (previewImage_) {
        return previewImage_.get()->copy();
    }
    return QImage();
}

void
VideoWidget::setPhotoMode(bool isPhotoMode)
{

    photoMode_ = isPhotoMode;
    auto color = isPhotoMode ? Qt::transparent : Qt::black;

    QPalette pal(palette());
    pal.setColor(QPalette::Background, color);
    setAutoFillBackground(true);
    setPalette(pal);
}

void
VideoWidget::disconnectRendering()
{
    QObject::disconnect(rendererConnections_.started);
    QObject::disconnect(rendererConnections_.stopped);
    QObject::disconnect(rendererConnections_.updated);
}

void
VideoWidget::rendererStartedWithoutDistantRender()
{
    // connect only local preview rendering
    QObject::disconnect(rendererConnections_.started);

    this->show();

    resetPreview_ = true;

    QObject::disconnect(rendererConnections_.updated);
    rendererConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        this,
        &VideoWidget::slotUpdatePreview);

    QObject::disconnect(rendererConnections_.stopped);
    rendererConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        this,
        &VideoWidget::slotStopFullView);
}

void
VideoWidget::slotUpdatePreview(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    using namespace lrc::api::video;
    if (id == PREVIEW_RENDERER_ID) {
        previewRenderer_ = const_cast<Renderer*>(renderer);
        renderFrame(id);
    }
}

void
VideoWidget::slotUpdateFullView(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    using namespace lrc::api::video;
    if (id == PREVIEW_RENDERER_ID) {
        previewRenderer_ = const_cast<Renderer*>(renderer);
    } else {
        distantRenderer_ = const_cast<Renderer*>(renderer);
    }
    renderFrame(id);
}

void
VideoWidget::slotStopFullView(const std::string& id)
{
    QObject::disconnect(rendererConnections_.updated);
    QObject::disconnect(rendererConnections_.stopped);
    using namespace lrc::api::video;
    if (id == PREVIEW_RENDERER_ID) {
        previewRenderer_ = nullptr;
    } else {
        distantRenderer_ = nullptr;
    }
    repaint();
}
