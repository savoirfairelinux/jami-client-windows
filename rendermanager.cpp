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

#include "rendermanager.h"

using namespace lrc::api;

FrameWrapper::FrameWrapper(AVModel& avModel, const std::string& id)
    : avModel_(avModel),
    id_(id)
{}

FrameWrapper::~FrameWrapper()
{
    if (id_ == video::PREVIEW_RENDERER_ID) {
        avModel_.stopPreview();
    }
}

void
FrameWrapper::connectRendering()
{
    QObject::disconnect(renderConnections_.started);
    renderConnections_.started = connect(
        &avModel_,
        &AVModel::rendererStarted,
        this,
        &FrameWrapper::slotRenderingStarted);
}

void
FrameWrapper::slotRenderingStarted(const std::string& id)
{
    if (id != id_) {
        return;
    }

    QObject::disconnect(renderConnections_.started);
    QObject::disconnect(renderConnections_.updated);
    QObject::disconnect(renderConnections_.stopped);

    renderer_ = const_cast<video::Renderer*>(&avModel_.getRenderer(id_));

    renderConnections_.updated = connect(
        &avModel_,
        &AVModel::frameUpdated,
        this,
        &FrameWrapper::slotFrameUpdated);

    renderConnections_.stopped = connect(
        &avModel_,
        &AVModel::rendererStopped,
        this,
        &FrameWrapper::slotRenderingStopped);
}

void
FrameWrapper::slotFrameUpdated(const std::string& id)
{
    if (id != id_) {
        return;
    }

    if (!renderer_ || !renderer_->isRendering()) {
        return;
    }

    {
        QMutexLocker lock(&mutex_);

        frame_ = renderer_->currentFrame();

        unsigned int width = renderer_->size().width();
        unsigned int height = renderer_->size().height();
        unsigned int size = frame_.storage.size();

        // If the frame is empty or not the expected size,
        // don't publish it.
        if (size == 0 || size != width * height * 4) {
            return;
        }

        buffer_ = std::move(frame_.storage);
        /*pixmap_.convertFromImage(
            QImage((uint8_t*) buffer_.data(),
                   width,
                   height,
                   QImage::Format_ARGB32_Premultiplied)
        );*/
        image_.reset(
            new QImage((uint8_t*) buffer_.data(),
            width,
            height,
            QImage::Format_ARGB32_Premultiplied)
        );
    }
    emit frameReady();
}

void
FrameWrapper::slotRenderingStopped(const std::string& id)
{
    if (id != id_) {
        return;
    }

    QObject::disconnect(renderConnections_.updated);
    QObject::disconnect(renderConnections_.stopped);
    renderer_ = nullptr;

    emit renderingStopped();
}

RenderManager::RenderManager(AVModel& avModel)
    :avModel_(avModel)
{
    previewFrameWrapper_ = std::make_unique<FrameWrapper>(avModel_);

    connect(previewFrameWrapper_.get(), &FrameWrapper::frameReady,
        [this]() {
            emit previewFrameReady();
        });
    connect(previewFrameWrapper_.get(), &FrameWrapper::renderingStopped,
        [this]() {
            emit previewRenderingStopped();
        });
}

RenderManager::~RenderManager()
{
    previewFrameWrapper_.reset();
    for (auto& dfw : distantFrameWrapper_) {
        dfw.second.reset();
    }
}
