/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
}

VideoWidget::~VideoWidget()
{}

void
VideoWidget::slotRendererStarted(const std::string& id)
{
    // only one videowidget will be used at the same time
    if (not isVisible())
        return;

    this->show();

    resetPreview_ = true;

    QObject::disconnect(rendererConnections_.started);

    QObject::disconnect(rendererConnections_.updated);
    rendererConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        [this](const std::string& id) {
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
        });

    QObject::disconnect(rendererConnections_.stopped);
    rendererConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        [this](const std::string& id) {
            QObject::disconnect(rendererConnections_.updated);
            QObject::disconnect(rendererConnections_.stopped);
            using namespace lrc::api::video;
            if (id == PREVIEW_RENDERER_ID) {
                previewRenderer_ = nullptr;
            } else {
                distantRenderer_ = nullptr;
            }
            repaint();
        });
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
            if(resetPreview_) {
                auto previewHeight = fullPreview_ ? height() : height() / 4;
                auto previewWidth = fullPreview_  ? width() : width() / 4;
                QImage scaledPreview;
                if (photoMode_)
                    scaledPreview = Utils::getCirclePhoto(*previewImage_, previewHeight);
                else
                    scaledPreview = previewImage_->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
                auto xDiff = (previewWidth - scaledPreview.width()) / 2;
                auto yDiff = (previewHeight - scaledPreview.height()) / 2;
                auto yPos = fullPreview_ ? yDiff : height() - previewHeight - previewMargin_;
                auto xPos = fullPreview_ ? xDiff : width() - scaledPreview.width() - previewMargin_;
                previewGeometry_.setRect(xPos, yPos, scaledPreview.width(), scaledPreview.height());
                painter.drawImage(previewGeometry_, scaledPreview);
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
            painter.drawImage(previewGeometry_, scaledPreview);
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
