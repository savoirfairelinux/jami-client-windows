/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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

#include <QWidget>
#include <QMutex>

#include "lrcinstance.h"

namespace Ui {
class VideoPreviewWidget;
}

class VideoPreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoPreviewWidget(QWidget* parent = nullptr);
    ~VideoPreviewWidget();

    void connectRendering();

    // Geters and Setters
    video::Frame getPreviewFrame() const { return previewFrame_; }
    void setPreviewFrame(video::Frame previewFrame) { previewFrame_ = previewFrame; }

    bool isPreviewDisplayed() { return isPreviewDisplayed_; }

    void setPreviewDisplay(bool display) { isPreviewDisplayed_ = display; }
    void setIsFullPreview(bool full) { fullPreview_ = full; }
    inline void setResetPreview(bool reset)
    {
        resetPreview_ = reset;
        hasFrame_ = false;
    }
    void setPhotoMode(bool isPhotoMode);
    QImage takePhoto();
    int getPreviewMargin() { return previewMargin_; }
    void resetPreview() { resetPreview_ = true; }

public:
    enum TargetPointPreview {
        topRight,
        topLeft,
        bottomRight,
        bottomLeft,
        left,
        right,
        top,
        bottom
    };
    void movePreview(TargetPointPreview typeOfMove);
    void drawPreview();

private:
    void updatePreview();
    void paintBackgroundColor(QPainter* painter, QColor color);


public slots:
    void slotToggleFullScreenClicked();
    void slotRendererStarted(const std::string& id);
    void renderFrame(const std::string& id);
    inline QRect& getPreviewRect() { return previewGeometry_; }

protected:
    void paintEvent(QPaintEvent* e);


private:
    Ui::VideoPreviewWidget* ui;

    struct rendererConnections {
        QMetaObject::Connection started, stopped, updated;
    } rendererConnections_;

    video::Renderer* previewRenderer_;
    video::Frame previewFrame_;
    std::unique_ptr<QImage> previewImage_;
    std::vector<uint8_t> framePreview_;

    QMutex mutex_;

    bool isPreviewDisplayed_;
    bool fullPreview_;
    QRect previewGeometry_;
    bool resetPreview_ = false;
    bool photoMode_ = false;
    bool hasFrame_ = false;

    constexpr static int previewMargin_ = 15;
};
