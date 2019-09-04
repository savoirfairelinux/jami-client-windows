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

FrameWrapper::FrameWrapper(bool isPreview) :
    isPreview_(isPreview)
{
}

FrameWrapper::~FrameWrapper()
{

}

void
FrameWrapper::connectPreviewRendering()
{
    QObject::disconnect(frameWrapperConnections_.started);
    if (isPreview_) {
        frameWrapperConnections_.started = connect(
            &LRCInstance::avModel(),
            &lrc::api::AVModel::rendererStarted,
            this,
            &FrameWrapper::slotPreviewStarted);
    }
}

void
FrameWrapper::slotPreviewStarted(const std::string& id)
{
    if (id != lrc::api::video::PREVIEW_RENDERER_ID)
        return;

    QObject::disconnect(frameWrapperConnections_.started);

    QObject::disconnect(frameWrapperConnections_.updated);
    frameWrapperConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        this,
        &FrameWrapper::slotPreviewUpdated);

    QObject::disconnect(frameWrapperConnections_.stopped);
    frameWrapperConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        this,
        &FrameWrapper::slotPreviewStoped);
}

void
FrameWrapper::slotPreviewUpdated(const std::string& id)
{
    if (id != lrc::api::video::PREVIEW_RENDERER_ID)
        return;
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    previewRenderer_ = const_cast<lrc::api::video::Renderer*>(renderer);
    renderFrame(id);
}

void
FrameWrapper::renderFrame(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    using namespace lrc::api::video;
    auto renderer = &avModel->getRenderer(id);
    if (renderer && renderer->isRendering()) {
        {
            QMutexLocker lock(&mutex_);
            auto tmp = renderer->currentFrame();
            if (tmp.storage.size()) {
                previewFrame_ = tmp;
            }
        }
        emit previewRenderReady();
    }
}

void
FrameWrapper::slotPreviewStoped(const std::string& id)
{
    if (id != lrc::api::video::PREVIEW_RENDERER_ID)
        return;
    QObject::disconnect(frameWrapperConnections_.updated);
    QObject::disconnect(frameWrapperConnections_.stopped);
    previewRenderer_ = nullptr;
    emit previewRenderStopped();
}

RenderDistributer::RenderDistributer()
{
    previewFrameWrapper_ = std::make_unique<FrameWrapper>(true);

    connect(previewFrameWrapper_.get(), &FrameWrapper::previewRenderReady,
        [this]() {
            emit previewRenderReady();
        });
    connect(previewFrameWrapper_.get(), &FrameWrapper::previewRenderStopped,
        [this]() {
            emit previewRenderStopped();
        });
}

RenderDistributer::~RenderDistributer()
{
    previewFrameWrapper_.reset();
}
