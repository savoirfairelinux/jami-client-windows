/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "previewwidget.h"

#include "lrcinstance.h"
#include "utils.h"

#include <QPixmap>

PreviewWidget::PreviewWidget(QWidget * parent)
    : VideoWidgetBase(Qt::transparent, parent)
{
    previewFrameUpdatedConnection_ = connect(LRCInstance::renderer(), &RenderManager::previewFrameUpdated,
        [this]() {
            repaint();
        });
    previewRenderingStopped_ = connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            repaint();
        });
}

PreviewWidget::~PreviewWidget()
{
    disconnect(previewFrameUpdatedConnection_);
    disconnect(previewRenderingStopped_);
}

void
PreviewWidget::paintBackground(QPainter* painter)
{
    QBrush brush(Qt::black);
    QPainterPath path;
    path.addRect(this->rect());
    painter->fillPath(path, brush);
}

GLPreviewWidget::GLPreviewWidget(QWidget* parent)
    : GLVideoWidgetBase(Qt::transparent, parent)
{
    connect(LRCInstance::renderer(), &RenderManager::previewD3DFrameUpdated,
        [this]() {
            prepareFrameToDisplay(LRCInstance::renderer()->getPreviewAVFrame());
            update();
        });
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            prepareFrameToDisplay(LRCInstance::renderer()->getPreviewAVFrame());
            update();
        });
}

GLPreviewWidget::~GLPreviewWidget()
{
}

void
PreviewWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    if (previewImage) {
        //QImage scaledPreview;
        auto aspectRatio =
            static_cast<qreal>(previewImage->width()) /
            static_cast<qreal>(previewImage->height());
        auto previewHeight = height();
        auto previewWidth = previewHeight * aspectRatio;

        /* Instead of setting fixed size, we could get an x offset for the preview
         * but this would render the horizontal spacers in the parent widget useless.
         * e.g.
         * auto parent = qobject_cast<QWidget*>(this->parent());
         * auto xPos = (parent->width() - previewWidth) / 2;
         * setGeometry(QRect(QPoint(xPos, this->pos().y()), QSize(previewWidth, previewHeight)));
        */
        setFixedWidth(previewWidth);
        setFixedHeight(previewHeight);

        // OpenGL (shader) scaledPreview = previewImage->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        // OpenGL (draw) painter.drawImage(this->rect(), scaledPreview);
    } else {
        // OpenGL (shouldn't be needed after glClear) paintBackground(&painter);
    }
}

PhotoboothPreviewWidget::PhotoboothPreviewWidget(QWidget* parent)
    : PreviewWidget(parent)
{
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            hide();
        });
}

PhotoboothPreviewWidget::~PhotoboothPreviewWidget()
{}

void
PhotoboothPreviewWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    if (previewImage) {
        QImage scaledPreview;
        scaledPreview = Utils::getCirclePhoto(*previewImage, height());
        this->setGeometry(0, 0, scaledPreview.width(), scaledPreview.height());
        painter.drawImage(this->rect(), scaledPreview);
    } else {
        paintBackground(&painter);
    }
}

void
PhotoboothPreviewWidget::paintBackground(QPainter* painter)
{
    QBrush brush(Qt::black);
    QPainterPath path;
    auto r = this->rect().width() / 2;
    path.addEllipse(QPoint(r, r), r, r);
    painter->fillPath(path, brush);
}

QImage
PhotoboothPreviewWidget::takePhoto()
{
    if (auto previewImage = LRCInstance::renderer()->getPreviewFrame()) {
        return previewImage->copy();
    }
    return QImage();
}

VideoCallPreviewWidget::VideoCallPreviewWidget(QWidget* parent)
    : PreviewWidget(parent)
{}

VideoCallPreviewWidget::~VideoCallPreviewWidget()
{}

void
VideoCallPreviewWidget::setContainerSize(const QSize& size)
{
    containerSize_ = size;
}

void
VideoCallPreviewWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();

    if (!previewImage) {
        return;
    }

    // reset geometry if needed
    auto newSize = getScaledSize(previewImage->width(), previewImage->height());
    if (newSize != this->size()) {
        setupGeometry(newSize);
    }

    QImage scaledPreview;
    scaledPreview = previewImage->scaled(this->size(), Qt::KeepAspectRatio);

    // draw rounded corner image
    QBrush brush(scaledPreview);
    brush.setTransform(QTransform::fromTranslate(this->rect().x(), this->rect().y()));
    QPainterPath previewPath;
    previewPath.addRoundRect(this->rect() - padding_, cornerRadius_);
    painter.fillPath(previewPath, brush);
}

QSize
VideoCallPreviewWidget::getScaledSize(int sourceWidth, int sourceHeight)
{
    auto invAspectRatio =
        static_cast<qreal>(sourceHeight) /
        static_cast<qreal>(sourceWidth);
    int newPreviewWidth = containerSize_.width() * containerRatio_;
    int newPreviewHeight = newPreviewWidth * invAspectRatio;
    return QSize(newPreviewWidth, newPreviewHeight);
}

void
VideoCallPreviewWidget::setupGeometry(const QSize& newSize)
{
    auto currentRect = geometry();
    currentRect.setSize(newSize);
    switch (location_) {
    case PreviewSnap::NW:
        currentRect.moveTopLeft(QPoint(margin_, margin_));
        break;
    case PreviewSnap::NE:
        currentRect.moveTopRight(QPoint(containerSize_.width() - margin_,
                                        margin_));
        break;
    case PreviewSnap::SW:
        currentRect.moveBottomLeft(QPoint(margin_,
                                          containerSize_.height() - margin_));
        break;
    case PreviewSnap::SE:
        currentRect.moveBottomRight(QPoint(containerSize_.width() - margin_,
                                           containerSize_.height() - margin_));
        break;
    }
    setGeometry(currentRect);
}

void
VideoCallPreviewWidget::setLocation(const PreviewSnap location)
{
    location_ = location;
}

QPoint
VideoCallPreviewWidget::getTopLeft()
{
    switch (location_) {
    case PreviewSnap::NW:
        return QPoint(
            margin_,
            margin_);
    case PreviewSnap::NE:
        return QPoint(
            containerSize_.width() - margin_ - this->width(),
            margin_);
    case PreviewSnap::SW:
        return QPoint(
            margin_,
            containerSize_.height() - margin_ - this->height());
    case PreviewSnap::SE:
        return QPoint(
            containerSize_.width() - margin_ - this->width(),
            containerSize_.height() - margin_ - this->height());
    }
}

GLVideoCallPreviewWidget::GLVideoCallPreviewWidget(QWidget* parent)
    : GLPreviewWidget(parent)
{
}

GLVideoCallPreviewWidget::~GLVideoCallPreviewWidget()
{
}

void
GLVideoCallPreviewWidget::setContainerSize(const QSize& size)
{
    containerSize_ = size;
}

void
GLVideoCallPreviewWidget::setLocation(const PreviewSnap location)
{
    location_ = location;
}

QPoint
GLVideoCallPreviewWidget::getTopLeft()
{
    switch (location_) {
    case PreviewSnap::NW:
        return QPoint(
            margin_,
            margin_);
    case PreviewSnap::NE:
        return QPoint(
            containerSize_.width() - margin_ - this->width(),
            margin_);
    case PreviewSnap::SW:
        return QPoint(
            margin_,
            containerSize_.height() - margin_ - this->height());
    case PreviewSnap::SE:
        return QPoint(
            containerSize_.width() - margin_ - this->width(),
            containerSize_.height() - margin_ - this->height());
    }
}

QSize
GLVideoCallPreviewWidget::getScaledSize(int sourceWidth, int sourceHeight)
{
    auto invAspectRatio =
        static_cast<qreal>(sourceHeight) /
        static_cast<qreal>(sourceWidth);
    int newPreviewWidth = containerSize_.width() * containerRatio_;
    int newPreviewHeight = newPreviewWidth * invAspectRatio;
    return QSize(newPreviewWidth, newPreviewHeight);
}

void
GLVideoCallPreviewWidget::setupGeometry(const QSize& newSize)
{
    auto currentRect = geometry();
    currentRect.setSize(newSize);
    switch (location_) {
    case PreviewSnap::NW:
        currentRect.moveTopLeft(QPoint(margin_, margin_));
        break;
    case PreviewSnap::NE:
        currentRect.moveTopRight(QPoint(containerSize_.width() - margin_,
            margin_));
        break;
    case PreviewSnap::SW:
        currentRect.moveBottomLeft(QPoint(margin_,
            containerSize_.height() - margin_));
        break;
    case PreviewSnap::SE:
        currentRect.moveBottomRight(QPoint(containerSize_.width() - margin_,
            containerSize_.height() - margin_));
        break;
    }
    setGeometry(currentRect);
}

VideoRecordPreviewWidget::VideoRecordPreviewWidget(QWidget* parent)
    : PreviewWidget(parent)
{}

VideoRecordPreviewWidget::~VideoRecordPreviewWidget()
{}

void
VideoRecordPreviewWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto parentRect = qobject_cast<QWidget*>(this->parent());
    if (!parentRect) {
        return;
    }

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    if (previewImage) {
        QImage scaledPreview;
        auto parentAspectRatio =
            static_cast<qreal>(parentRect->width()) /
            static_cast<qreal>(parentRect->height());
        auto aspectRatio =
            static_cast<qreal>(previewImage->width()) /
            static_cast<qreal>(previewImage->height());
        int previewHeight;
        int previewWidth;
        int xOffset = 0;
        int yOffset = 0;
        if (parentAspectRatio < aspectRatio) {
            previewHeight = parentRect->height();
            previewWidth = previewHeight * aspectRatio;
            xOffset = (parentRect->width() - previewWidth) / 2;
        } else {
            previewWidth = parentRect->width();
            previewHeight = previewWidth / aspectRatio;
            yOffset = (parentRect->height() - previewHeight) / 2;
        }

        scaledPreview = previewImage->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        scaledPreviewImage_ = scaledPreview;
        scaledPreviewImageXOffset_ = xOffset;
        scaledPreviewImageYOffset_ = yOffset;

        // draw rounded corner image
        Utils::fillRoundRectPath(painter, scaledPreview, rect(), cornerRadius_, xOffset, yOffset);
    } else {
        if (paintBackground_) {
            Utils::fillRoundRectPath(painter, QColor(Qt::black), rect(), cornerRadius_);
            scaledPreviewImage_ = QImage();
        } else if (drawLastFrame_) {
            Utils::fillRoundRectPath(painter, scaledPreviewImage_, rect(), cornerRadius_, scaledPreviewImageXOffset_, scaledPreviewImageYOffset_);
        }
    }
}
