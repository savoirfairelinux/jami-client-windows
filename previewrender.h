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

#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QMutex>

#include <memory>
#include <array>

#include "lrcinstance.h"

namespace Ui {
class PreviewRenderWidget;
}

class PreviewRenderWidget : public QWidget {
    Q_OBJECT;

public:
    static PreviewRenderWidget* attachPreview(QWidget* parent = 0) {
        static PreviewRenderWidget* instance_ = new PreviewRenderWidget(parent);
        return instance_;
    }
    ~PreviewRenderWidget();

    // rounded boarder for preview in calls
    void setRoundedBoarder(bool rounded) { isroundedBoarder_ = rounded; }
    // reset preview when in resize event
    void resetPreview() { resetPreview_ = true; }
    // set current container geometry for preview renderer
    void setCurrentConainerGeo(int width, int height) { currentConainerHeight_ = height; currentConainerWidth_ = width; }
    // centre for preview in avsettings
    void setNeedToCentre(bool need) { needToCentre_ = need; }
    // only trigger preview reset when image is reloaded
    void triggerResetPreviewAfterImageReloaded() { triggerResetPreviewAfterImageReloaded_ = true; }
    QImage takePhoto();
    void setPhotoMode(bool isPhotoMode);
    void connectRendering();
    void changeToRoundedBoarder();
    void resetBoarder();
    QRect getPreviewRect() { return previewGeometry_; }
    // force the preview to repaint
    void forceRepaint() { paintEvent(&QPaintEvent(QRect(0, 0, 0, 0))); }

    constexpr static int previewMargin_ = 15;
    constexpr static int boarderRadius_ = 39;

private:
    Ui::PreviewRenderWidget* ui;

    explicit PreviewRenderWidget(QWidget* parent = 0);

    struct previewRendererConnections {
        QMetaObject::Connection started, stopped, updated;
    } previewRendererConnections_;

    QMutex mutex_;

    bool photoMode_ { false };
    bool hasFrame_ { false };
    bool isroundedBoarder_ { false };
    bool resetPreview_ { false };
    bool needToCentre_ { false };
    bool triggerResetPreviewAfterImageReloaded_ { false };
    QRect previewGeometry_;
    int currentConainerHeight_ { 0 };
    int currentConainerWidth_ { 0 };

    lrc::api::video::Renderer* previewRenderer_;
    lrc::api::video::Frame previewFrame_;
    std::unique_ptr<QImage> previewImage_;
    std::vector<uint8_t> framePreview_;

protected:
    void paintEvent(QPaintEvent* e);
};
