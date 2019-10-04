/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "api/lrc.h"
#include "api/avmodel.h"

#include <QObject>
#include <QMutex>
#include <QImage>

using namespace lrc::api;

/**
 * This class acts as a QImage rendering sink and manages
 * signal/slot connections to it's underlying (AVModel) renderer
 * corresponding to the object's renderer id.
 * A QImage pointer is provisioned and updated once rendering
 * starts.
 */
class FrameWrapper final : public QObject
{
    Q_OBJECT;
public:
    FrameWrapper(AVModel& avModel,
                 const std::string& id = video::PREVIEW_RENDERER_ID);
    ~FrameWrapper();

    /**
     * Reconnect the started rendering connection for this object.
     */
    void connectRendering();
    /**
     * Get the most recently rendered frame as a QImage.
     * @return the rendered image of this object's id
     */
    QImage* getFrame();

signals:
    /**
     * Emitted once in slotRenderingStarted.
     * @param id of the renderer
     */
    void renderingStarted(const std::string& id);
    /**
     * Emitted each time a frame is ready to be displayed.
     * @param id of the renderer
     */
    void frameUpdated(const std::string& id);
    /**
     * Emitted once in slotRenderingStopped.
     * @param id of the renderer
     */
    void renderingStopped(const std::string& id);

private slots:
    /**
     * Used to listen to AVModel::rendererStarted.
     * @param id of the renderer
     */
    void slotRenderingStarted(const std::string& id = video::PREVIEW_RENDERER_ID);
    /**
     * Used to listen to AVModel::frameUpdated.
     * @param id of the renderer
     */
    void slotFrameUpdated(const std::string& id = video::PREVIEW_RENDERER_ID);
    /**
     * Used to listen to AVModel::renderingStopped.
     * @param id of the renderer
     */
    void slotRenderingStopped(const std::string& id = video::PREVIEW_RENDERER_ID);

private:
    /* the id of the renderer */
    std::string id_;

    /* the id of the renderer */
    video::Renderer* renderer_;
    video::Frame frame_;
    std::vector<uint8_t> buffer_;
    std::unique_ptr<QImage> image_;
    QMutex mutex_;

    AVModel& avModel_;

    struct renderConnections {
        QMetaObject::Connection started, stopped, updated;
    } renderConnections_;
};

class RenderManager final : public QObject
{
    Q_OBJECT;

public:
    explicit RenderManager(AVModel& avModel);
    ~RenderManager();

    /**
     * Get the most recently rendered preview frame as a QImage.
     * @return the rendered preview image
     */
    QImage* getPreviewFrame();
    /**
     * Start capturing and rendering preview frames.
     * @param force if the capture device should be started
     * @param async
     */
    void startPreviewing(bool force = false, bool async = true);
    /**
     * Stop capturing.
     * @param async
     */
    void stopPreviewing(bool async = true);
    /**
     * Connect the preview frame wrapper without starting the
     * capture device.
     */
    void connectPreviewRendering();

signals:
    void videoDeviceListChanged();

    void previewRenderingStarted();
    void previewFrameUpdated();
    void previewRenderingStopped();

    void distantRenderingStarted(const std::string& id);
    void distantFrameReady(const std::string& id);
    void distantRenderingStopped(const std::string& id);

private slots:
    void slotDeviceEvent();

private:
    enum class DeviceEvent { Added, RemovedCurrent, None };

    int deviceListSize_;

    std::unique_ptr<FrameWrapper> previewFrameWrapper_;
    std::atomic_bool isPreviewing_;

    /* distant for each call/conf/conversation */
    std::map<std::string, std::unique_ptr<FrameWrapper>> distantFrameWrapperMap_;

    /* convenience ref to avmodel */
    AVModel& avModel_;
};