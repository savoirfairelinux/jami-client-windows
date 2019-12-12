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

    virtual void paintBackground(QPainter* painter) override;

private:
    QMetaObject::Connection previewFrameUpdatedConnection_;
    QMetaObject::Connection previewRenderingStopped_;
};

class GLPreviewWidget : public GLVideoWidgetBase {
    Q_OBJECT;

public:
    explicit GLPreviewWidget(QWidget* parent = nullptr);
    ~GLPreviewWidget();

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

    void paintBackground(QPainter* painter) override;
};

enum class PreviewSnap { NW, NE, SE, SW };

// rounded corners for video calls
class VideoCallPreviewWidget final : public PreviewWidget {
    Q_OBJECT;

public:
    explicit VideoCallPreviewWidget(QWidget* parent = 0);
    ~VideoCallPreviewWidget();

    void setContainerSize(const QSize& size);
    void setLocation(const PreviewSnap location);

    QPoint getTopLeft();

    QSize getScaledSize(int w, int h);

    /**
     * Computes new widget size with aspect ratio guarded
     * taking into consideration snap location
     */
    void setupGeometry(const QSize& newSize);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    constexpr static qreal containerRatio_ = 0.2f;
    constexpr static qreal cornerRadius_ = 10.0f;
    constexpr static qreal margin_ = 15.0f;
    constexpr static QMargins padding_ = QMargins(2, 2, 2, 2);
    PreviewSnap location_{ PreviewSnap::SE };
    QSize containerSize_{0, 0};
};

// rounded corners for video calls
class GLVideoCallPreviewWidget final : public GLPreviewWidget {
    Q_OBJECT;

public:
    explicit GLVideoCallPreviewWidget(QWidget* parent = nullptr);
    ~GLVideoCallPreviewWidget();

    void setContainerSize(const QSize& size);
    void setLocation(const PreviewSnap location);

    QPoint getTopLeft();

    QSize getScaledSize(int w, int h);

    /**
     * Computes new widget size with aspect ratio guarded
     * taking into consideration snap location
     */
    void setupGeometry(const QSize& newSize);

private:
    constexpr static qreal containerRatio_ = 0.2f;
    constexpr static qreal cornerRadius_ = 10.0f;
    constexpr static qreal margin_ = 15.0f;
    constexpr static QMargins padding_ = QMargins(2, 2, 2, 2);
    PreviewSnap location_{ PreviewSnap::SE };
    QSize containerSize_{ 0, 0 };
};

// rounded corners for video record view
class VideoRecordPreviewWidget final : public PreviewWidget {
    Q_OBJECT;

public:
    explicit VideoRecordPreviewWidget(QWidget* parent = 0);
    ~VideoRecordPreviewWidget();

    void toPaintingBackground(bool state) { paintBackground_ = state; }
    void toDrawLastFrame(bool state) { drawLastFrame_ = state; }
    QImage getScaledPreviewImage() { return scaledPreviewImage_; }

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    int scaledPreviewImageXOffset_{ 0 };
    int scaledPreviewImageYOffset_{ 0 };
    constexpr static qreal cornerRadius_ = 10.0f;

    QImage scaledPreviewImage_;

    bool paintBackground_{ true };
    bool drawLastFrame_{ false };
};
