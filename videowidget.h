/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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
    explicit VideoWidget(QWidget *parent = 0);
    ~VideoWidget();
    void paintEvent(QPaintEvent* evt);

public slots:
    void previewStarted(Video::Renderer* renderer);
    void previewStopped();
    void frameFromPreview();
    void callInitiated(Call *call, Video::Renderer *renderer);
    void frameFromDistant();
    void renderingStopped();

private:
    Video::Renderer* previewRenderer_;
    Video::Renderer* renderer_;
    Video::RendererFrame currentPreviewFrame_;
    Video::RendererFrame currentDistantFrame_;
    QMutex mutex;
    std::unique_ptr<QImage> distantImage;
    std::unique_ptr<QImage> previewImage;
    std::vector<uint8_t> frameDistant;
    std::vector<uint8_t> framePreview;

    constexpr static int previewMargin_ = 15;
};

