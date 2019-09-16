/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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
#include <QPainter>
#include <QMutex>

#include <memory>
#include <array>

#include "lrcinstance.h"

using namespace lrc::api;

class DistantRendererWidget : public QWidget
{
    Q_OBJECT;

public:
    explicit DistantRendererWidget(QWidget* parent = 0);
    ~DistantRendererWidget();
    void connectDistantRendering();
    void disconnectRendering();

protected:
    void paintEvent(QPaintEvent* e);

public slots:
    void slotToggleFullScreenClicked();
    void slotDistantRendererStarted(const std::string& id = {});
    void slotUpdateDistantView(const std::string& id = {});
    void slotStopDistantView(const std::string& id = {});
    void renderFrame(const std::string& id);

private:
    struct rendererDistantConnections {
        QMetaObject::Connection started, stopped, updated;
    } rendererDistantConnections_;

    video::Renderer* distantRenderer_;
    video::Frame distantFrame_;
    std::unique_ptr<QImage> distantImage_;
    std::vector<uint8_t> frameDistant_;

    QMutex mutex_;
};
