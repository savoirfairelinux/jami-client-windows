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

struct RenderConnections {
    QMetaObject::Connection started, stopped, updated;
};

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

    /**
     * Check if the object is updating actively
     */
    bool isRendering();

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

    /* a pointer to the lrc renderer object */
    video::Renderer* renderer_;

    /* a local copy of the renderer's current frame */
    video::Frame frame_;

    /* a the frame's storage data used to set the image */
    std::vector<uint8_t> buffer_;

    /* the frame's paint ready QImage */
    std::unique_ptr<QImage> image_;

    /* used to protect the buffer during QImage creation routine */
    QMutex mutex_;

    /* true if the object is rendering */
    std::atomic_bool isRendering_;

    /* convenience ref to avmodel */
    AVModel& avModel_;

    /* connections to the underlying renderer signals in avmodel */
    RenderConnections renderConnections_;
};

/**
 * RenderManager filters signals and ecapsulates preview and distant
 * frame wrappers, providing access to QImages for each and simplified
 * start/stop mechanisms for renderers. It should contain as much
 * renderer control logic as possible and prevent ui widgets from directly
 * interfacing the rendering logic.
 */
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
     * Get the most recently rendered distant frame for a given id
     * as a QImage.
     * @return the rendered preview image
     */
    QImage* getFrame(const std::string& id);
    /**
     * Add and connect a distant renderer for a given id
     * to a FrameWrapper object
     * @param id
     */
    void addDistantRenderer(const std::string& id);
    /**
     * Disconnect and remove a FrameWrapper object connected to a
     * distant renderer for a given id
     * @param id
     */
    void removeDistantRenderer(const std::string& id);

signals:
    /* Emitted when the size of the video capture device list changes. */
    void videoDeviceListChanged();

    /* Emitted when the preview is started. */
    void previewRenderingStarted();

    /* Emitted when the preview has a new frame ready. */
    void previewFrameUpdated();

    /* Emitted when the preview is stopped. */
    void previewRenderingStopped();

    /* Emitted when a distant renderer is started for a given id. */
    void distantRenderingStarted(const std::string& id);

    /* Emitted when a distant renderer has a new frame ready for a given id. */
    void distantFrameUpdated(const std::string& id);

    /* Emitted when a distant renderer is stopped for a given id. */
    void distantRenderingStopped(const std::string& id);

private slots:
    /**
     * Used to listen to AVModel::deviceEvent.
     */
    void slotDeviceEvent();

private:
    /* used to classify capture device events */
    enum class DeviceEvent { Added, RemovedCurrent, None };

    /* used to track the capture device count */
    int deviceListSize_;

    /* one preview frame */
    std::unique_ptr<FrameWrapper> previewFrameWrapper_;

    /* distant for each call/conf/conversation */
    std::map<std::string, std::unique_ptr<FrameWrapper>> distantFrameWrapperMap_;
    std::map<std::string, RenderConnections> distantConnectionMap_;

    /* convenience ref to avmodel */
    AVModel& avModel_;
};