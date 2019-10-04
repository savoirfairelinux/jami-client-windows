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

PreviewWidget::PreviewWidget(QWidget * parent)
    : VideoWidgetBase(Qt::transparent, parent)
{}

PreviewWidget::~PreviewWidget()
{}

void
PreviewWidget::paintBackgroundColor(QPainter* painter, QColor color, bool rounded)
{
    QBrush brush(color);
    QPainterPath path;
    auto r = this->rect().width() / 2;
    if (rounded) {
        path.addEllipse(QPoint(r, r), r, r);
    } else {
        path.addRect(this->rect());
    }
    painter->fillPath(path, brush);
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

        scaledPreview = previewImage->scaled(previewWidth, previewHeight, Qt::KeepAspectRatio);
        painter.drawImage(this->rect(), scaledPreview);
    } else {
        // clear to black
        paintBackgroundColor(&painter, Qt::black, false);
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

    // adjust geometry based on previewImage
    auto newGeometry = computeGeometry(previewImage->width(), previewImage->height());
    setGeometry(newGeometry);

    QImage scaledPreview;
    scaledPreview = previewImage->scaled(this->size(), Qt::KeepAspectRatio);

    // draw rounded corner image
    QBrush brush(scaledPreview);
    brush.setTransform(QTransform::fromTranslate(this->rect().x(), this->rect().y()));
    QPainterPath previewPath;
    previewPath.addRoundRect(this->rect(), previewCornerRadius_);
    painter.fillPath(previewPath, brush);
}

QRect
VideoCallPreviewWidget::computeGeometry(int w, int h)
{
    auto invAspectRatio = static_cast<qreal>(h) / static_cast<qreal>(w);
    int newPreviewWidth = containerSize_.width() * previewContainerRatio_;
    int newPreviewHeight = newPreviewWidth * invAspectRatio;
    return QRect(this->pos(), QSize(newPreviewWidth, newPreviewHeight));
}
