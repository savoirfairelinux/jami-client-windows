/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

PreviewWidget::PreviewWidget(QWidget * parent)
    : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(true);
    setPalette(pal);
    setUpdatesEnabled(true);

    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStarted,
        [this]() {});
    connect(LRCInstance::renderer(), &RenderManager::previewFrameReady,
        [this]() {
            repaint();
        });
    connect(LRCInstance::renderer(), &RenderManager::previewRenderingStopped,
        [this]() {
            repaint();
        });
}

PreviewWidget::~PreviewWidget()
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
        QImage scaledPreview;
        scaledPreview = previewImage->scaled(width(), height(), Qt::KeepAspectRatio);
        previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());
        painter.drawImage(previewGeometry_, scaledPreview);
    } else {
        // clear to black
        paintBackgroundColor(&painter, Qt::black, false);
    }
}

void
PreviewWidget::paintBackgroundColor(QPainter* painter, QColor color, bool rounded)
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

PhotoboothPreviewWidget::PhotoboothPreviewWidget(QWidget* parent)
    : PreviewWidget(parent)
{
}

PhotoboothPreviewWidget::~PhotoboothPreviewWidget()
{
}

void
PhotoboothPreviewWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int previewSize = height();

    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    if (previewImage) {
        QImage scaledPreview;
        scaledPreview = Utils::getCirclePhoto(*previewImage, previewSize);
        previewGeometry_.setRect(0, 0, scaledPreview.width(), scaledPreview.height());
        painter.drawImage(previewGeometry_, scaledPreview);
    } else {
        // clear to black
        paintBackgroundColor(&painter, Qt::black, true);
    }
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
{
}

VideoCallPreviewWidget::~VideoCallPreviewWidget()
{
}

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

    if (previewImage) {
        // adjust previewGeometry_ based on previewImage
        auto invAspectRatio =
            static_cast<qreal>(previewImage->height()) /
            static_cast<qreal>(previewImage->width());
        int previewWidth = containerSize_.width() * previewContainerRatio_;
        int previewHeight = previewWidth * invAspectRatio;
        auto previewSize = this->rect().size();
        setGeometry(QRect(this->pos(), QSize(previewWidth, previewHeight)));
        previewGeometry_ = this->rect();

        QImage scaledPreview;
        scaledPreview = previewImage->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);

        // draw rounded corner image
        QBrush brush(scaledPreview);
        brush.setTransform(QTransform::fromTranslate(previewGeometry_.x(), previewGeometry_.y()));
        QPainterPath previewPath;
        previewPath.addRoundRect(previewGeometry_, previewCornerRadius_);
        painter.fillPath(previewPath, brush);
    }
    /*painter.setBrush(Qt::red);
    painter.drawRect(previewGeometry_);*/
}
