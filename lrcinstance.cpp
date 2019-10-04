/**************************************************************************
| Copyright (C) 2019 by Savoir-faire Linux                                |
| Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              |
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

#include "lrcinstance.h"

using namespace lrc::api;

FrameWrapper::FrameWrapper(const std::string& id)
    :id_(id)
{}

FrameWrapper::~FrameWrapper()
{
    if (id_ == video::PREVIEW_RENDERER_ID) {
        auto avModel = &LRCInstance::avModel();
        avModel->stopPreview();
    }
}

void
FrameWrapper::connectRendering()
{
    QObject::disconnect(frameWrapperConnections_.started);
    frameWrapperConnections_.started = connect(
        &LRCInstance::avModel(),
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

    QObject::disconnect(frameWrapperConnections_.started);
    QObject::disconnect(frameWrapperConnections_.updated);
    QObject::disconnect(frameWrapperConnections_.stopped);

    frameWrapperConnections_.updated = connect(
        &LRCInstance::avModel(),
        &AVModel::frameUpdated,
        this,
        &FrameWrapper::slotFrameUpdated);

    frameWrapperConnections_.stopped = connect(
        &LRCInstance::avModel(),
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

    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    renderer_ = const_cast<lrc::api::video::Renderer*>(renderer);
    renderFrame();
}

void
FrameWrapper::renderFrame()
{
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id_);
    if (renderer && renderer->isRendering()) {
        {
            QMutexLocker lock(&mutex_);
            auto tmp = renderer->currentFrame();
            if (tmp.storage.size()) {
                frame_ = tmp;
            }
        }
        emit frameReady();
    }
}

void
FrameWrapper::slotRenderingStopped(const std::string& id)
{
    if (id != id_) {
        return;
    }

    QObject::disconnect(frameWrapperConnections_.updated);
    QObject::disconnect(frameWrapperConnections_.stopped);
    renderer_ = nullptr;
    emit renderingStopped();
}

RenderManager::RenderManager()
{
    previewFrameWrapper_ = std::make_unique<FrameWrapper>();

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
