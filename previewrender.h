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
#include <QPainterPath>
#include <QMutex>

#include <memory>
#include <array>

#include "lrcinstance.h"

namespace Ui {
class previewRenderWidget;
}

class PreviewRenderWidget : public QWidget {
    Q_OBJECT;

public:
    static PreviewRenderWidget* attachPreview(QWidget* parent = 0) {
        static PreviewRenderWidget* instance_ = new PreviewRenderWidget(parent);
        return instance_;
    }
    ~PreviewRenderWidget();

    void setResetPreview(bool reset) { resetPreview_ = reset; hasFrame_ = false; }
    void resetPreview() { resetPreview_ = true; }
    void setPreviewDisplay(bool display) { isPreviewDisplayed_ = display; }
    void setRoundedBoarder(bool rounded) { isroundedBoarder_ = rounded; }
    QImage takePhoto();
    void setPhotoMode(bool isPhotoMode);
    void connectRendering();
    void disconnectRendering();
    void changeToRoundedBoarder();
    void resetBoarder();
    QRect getPreviewRect() { return previewGeometry_; }

    constexpr static int previewMargin_ = 15;
    constexpr static int boarderRadius_ = 39;

public slots:
    void slotPreviewRendererStarted(const std::string& id = {});
    void slotUpdatePreview(const std::string& id = {});
    void slotStopPreview(const std::string& id = {});

private:
    Ui::previewRenderWidget* ui;

    explicit PreviewRenderWidget(QWidget* parent = 0);
    void renderFrame(const std::string& id);

    struct previewRendererConnections {
        QMetaObject::Connection started, stopped, updated;
    } previewRendererConnections_;

    QMutex mutex_;

    bool photoMode_ { false };
    bool resetPreview_ { false };
    bool hasFrame_ { false };
    bool isPreviewDisplayed_;
    bool isroundedBoarder_ { false };
    QRect previewGeometry_;

    lrc::api::video::Renderer* previewRenderer_;
    lrc::api::video::Frame previewFrame_;
    std::unique_ptr<QImage> previewImage_;
    std::vector<uint8_t> framePreview_;

protected:
    void paintEvent(QPaintEvent* e);
};
