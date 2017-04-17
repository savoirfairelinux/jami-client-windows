/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include <memory>

#include "video/renderer.h"
#include "video/previewmanager.h"
#include "callmodel.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = 0);
    ~VideoWidget();
    void paintEvent(QPaintEvent* evt);
    void setPreviewDisplay(bool display);
    void setDistantRenderer(Video::Renderer* renderer);
    void setIsFullPreview(bool full);
    inline void setResetPreview(bool reset){ resetPreview_ = reset; }
    void setPhotoMode(bool isPhotoMode);
    QImage takePhoto();

public slots:
    void previewStarted(Video::Renderer* renderer);
    void previewStopped();
    void frameFromPreview();
    void frameFromDistant();
    void renderingStopped();
    inline QRect& getPreviewRect(){ return previewGeometry_; }

private:
    Video::Renderer* previewRenderer_;
    Video::Renderer* renderer_;
    Video::Frame currentPreviewFrame_;
    Video::Frame currentDistantFrame_;
    QMutex mutex_;
    std::unique_ptr<QImage> distantImage_;
    std::unique_ptr<QImage> previewImage_;
    std::vector<uint8_t> frameDistant_;
    std::vector<uint8_t> framePreview_;
    bool isPreviewDisplayed_;
    bool fullPreview_;
    QRect previewGeometry_;
    bool resetPreview_ = false;
    bool photoMode_ = false;

    constexpr static int previewMargin_ = 15;
};

