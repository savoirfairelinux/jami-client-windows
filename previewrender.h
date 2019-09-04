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

    void connectRendering();

    // rounded boarder for preview in calls
    void setRoundedBorder(bool rounded) { isroundedBoarder_ = rounded; }
    // reset preview when in resize event
    void resetPreview() { resetPreview_ = true; }
    // set current container geometry for preview renderer
    void setCurrentConainerGeo(int width, int height) { currentContainerHeight_ = height; currentContainerWidth_ = width; }

    void paintBackgroundColor(QPainter* painter, QColor color, bool rounded = false);

    // for settings
    void setNeedToCenter(bool need) { needToCenter_ = need; }

    // for photobooth
    QImage takePhoto();
    void setPhotoMode(bool isPhotoMode);

    // for in-call preview
    void changeToRoundedBorder();
    void resetBorder();

    QRect getPreviewRect() { return previewGeometry_; }

    constexpr static int previewMargin_ = 15;
    constexpr static int boarderRadius_ = 39;

private:
    Ui::PreviewRenderWidget* ui;

    explicit PreviewRenderWidget(QWidget* parent = 0);

    struct previewRendererConnections {
        QMetaObject::Connection started, stopped, updated;
    } previewRendererConnections_;

    bool photoMode_ { false };
    bool hasFrame_ { false };
    bool isroundedBoarder_ { false };
    bool resetPreview_ { false };
    bool needToCenter_ { false };
    QRect previewGeometry_;
    int currentContainerHeight_ { 0 };
    int currentContainerWidth_ { 0 };

protected:
    void paintEvent(QPaintEvent* e);
};
