/**************************************************************************
| Copyright (C) 2019 by Savoir-faire Linux                                |
| Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              |
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          |
|                                                                         |
| This program is free software; you can redistribute it and/or modify    |
| it under the terms of the GNU General Public License as published by    |
| the Free Software Foundation; either version 3 of the License, or       |
| (at your option) any later version.                                     |
|                                                                         |
| This program is distributed in the hope that it will be useful,         |
| but WITHOUT ANY WARRANTY; without even the implied warranty of          |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
| GNU General Public License for more details.                            |
|                                                                         |
| You should have received a copy of the GNU General Public License       |
| along with this program.  If not, see <https://www.gnu.org/licenses/>.  |
**************************************************************************/

#include "api/lrc.h"
#include "api/avmodel.h"

#include <QObject>
#include <QMutex>
#include <QPixmap>

using namespace lrc::api;

class FrameWrapper : public QObject
{
    Q_OBJECT;
public:
    FrameWrapper(AVModel& avModel, const std::string& id = video::PREVIEW_RENDERER_ID);
    ~FrameWrapper();

    void connectRendering();
    QImage* getFrame() { return image_.get(); }

signals:
    void renderingStarted(const std::string& id);
    void frameReady(const std::string& id);
    void renderingStopped(const std::string& id);

protected slots:
    void slotRenderingStarted(const std::string& id = video::PREVIEW_RENDERER_ID);
    void slotFrameUpdated(const std::string& id = video::PREVIEW_RENDERER_ID);
    void slotRenderingStopped(const std::string& id = video::PREVIEW_RENDERER_ID);

private:
    std::string id_;
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

class RenderManager : public QObject
{
    Q_OBJECT;

public:
    RenderManager(AVModel& avModel);
    ~RenderManager();
    QImage* getPreviewFrame() { return previewFrameWrapper_->getFrame(); }
    void connectPreviewRendering() { previewFrameWrapper_->connectRendering(); }

signals:
    void previewRenderingStarted();
    void previewFrameReady();
    void previewRenderingStopped();

    void distantRenderingStarted(const std::string& id);
    void distantFrameReady(const std::string& id);
    void distantRenderingStopped(const std::string& id);

private:
    // one preview to rule them all
    std::unique_ptr<FrameWrapper> previewFrameWrapper_;

    // distant for each call/conf/conversation
    std::map<std::string, std::unique_ptr<FrameWrapper>> distantFrameWrapper_;

    AVModel& avModel_;
};