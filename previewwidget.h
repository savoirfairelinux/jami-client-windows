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

#pragma once

#include "videowidgetbase.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

// The base for preview widgets, serves as the settings preview
class PreviewWidget : public VideoWidgetBase {
    Q_OBJECT;

public:
    explicit PreviewWidget(QWidget* parent = 0);
    ~PreviewWidget();

protected:
    virtual void paintEvent(QPaintEvent* e) override;

    void paintBackgroundColor(QPainter* painter,
                              QColor color,
                              bool rounded = false);
};

// A rounded image for photobooths
class PhotoboothPreviewWidget final : public PreviewWidget {
    Q_OBJECT;

public:
    explicit PhotoboothPreviewWidget(QWidget* parent = 0);
    ~PhotoboothPreviewWidget();

    QImage takePhoto();

protected:
    void paintEvent(QPaintEvent* e) override;
};

// rounded corners for video calls
class VideoCallPreviewWidget final : public PreviewWidget {
    Q_OBJECT;

public:
    explicit VideoCallPreviewWidget(QWidget* parent = 0);
    ~VideoCallPreviewWidget();

    void setContainerSize(const QSize& size);

    /**
     * Computes new widget size with aspect ratio guarded, and
     * keeps the original position.
     */
    QRect computeGeometry(int w, int h);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    constexpr static qreal previewContainerRatio_ = 0.2f;
    constexpr static qreal previewCornerRadius_ = 10.0f;

    QSize containerSize_{0, 0};
};