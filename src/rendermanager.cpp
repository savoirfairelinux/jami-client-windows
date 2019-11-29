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

#include "rendermanager.h"

#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia//QVideoFrame>

extern "C" {
#include "libavutil/frame.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

using namespace lrc::api;

FrameWrapper::FrameWrapper(AVModel& avModel,
                           const QString& id)
    : avModel_(avModel)
    , id_(id)
    , isRendering_(false)
{}

FrameWrapper::~FrameWrapper()
{
    if (id_ == video::PREVIEW_RENDERER_ID) {
        avModel_.stopPreview();
    }
    sws_freeContext(img_convert_ctx);
    av_free(rgbBuffer);
    av_frame_free(&pFrameRGB);
}

void
FrameWrapper::connectStartRendering()
{
    QObject::disconnect(renderConnections_.started);
    renderConnections_.started = QObject::connect(
        &avModel_,
        &AVModel::rendererStarted,
        this,
        &FrameWrapper::slotRenderingStarted);
}

bool
FrameWrapper::startRendering()
{
    try {
        renderer_ = const_cast<video::Renderer*>(&avModel_.getRenderer(id_));
    } catch (std::out_of_range& e) {
        qWarning() << e.what();
        return false;
    }

    QObject::disconnect(renderConnections_.updated);
    QObject::disconnect(renderConnections_.stopped);

    renderConnections_.updated = QObject::connect(
        &avModel_,
        &AVModel::frameUpdated,
        this,
        &FrameWrapper::slotFrameUpdated);

    renderConnections_.stopped = QObject::connect(
        &avModel_,
        &AVModel::rendererStopped,
        this,
        &FrameWrapper::slotRenderingStopped);

    return true;
}

QImage*
FrameWrapper::getFrame()
{
    return image_.get();
}

bool
FrameWrapper::isRendering()
{
    return isRendering_;
}

void
FrameWrapper::slotRenderingStarted(const QString& id)
{
    if (id != id_) {
        return;
    }

    if (!startRendering()) {
        qWarning() << "Couldn't start rendering for id: " << id_;
        return;
    }

    isRendering_ = true;

    emit renderingStarted(id);
}

void
FrameWrapper::slotFrameUpdated(const QString& id)
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

#ifndef Q_OS_LINUX
        auto avFrame = renderer_->currentAVFrame();
        avFrame_ = avFrame.get();
        if(!avFrame_ || !avFrame_->width || !avFrame_->height) {
            qDebug() << "AVFrame: this frame does not exist!";
            return;
        }
        AVPixelFormat currentFormat = AVPixelFormat(avFrame_->format);
        AVPixelFormat targetFormat = AVPixelFormat::AV_PIX_FMT_RGBA;

        av_frame_free(&pFrameRGB);
        pFrameRGB = av_frame_alloc();
        int numBytes = avpicture_get_size(targetFormat, avFrame_->width, avFrame_->height);
        av_free(rgbBuffer);
        rgbBuffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
        avpicture_fill((AVPicture*) pFrameRGB, rgbBuffer,targetFormat, avFrame_->width, avFrame_->height);

        // set up SWS context, which is used to convert the video format
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = sws_getContext(avFrame_->width, avFrame_->height, currentFormat, avFrame_->width, avFrame_->height, targetFormat, SWS_BICUBIC, NULL, NULL, NULL);


        //convert the format from YUV to RGB with sws_scale
        sws_scale(img_convert_ctx,
            avFrame_->data,
            avFrame_->linesize, 0, avFrame_->height, pFrameRGB->data,
                pFrameRGB->linesize);

        image_.reset(
                new QImage((uchar*) std::move(rgbBuffer),
                    avFrame_->width,
                    avFrame_->height,
                QImage::Format_RGBA8888)
            );
#else
        if (frame_.ptr) {
            image_.reset(new QImage(frame_.ptr, width, height, QImage::Format_ARGB32));

        }
#endif
    }

    emit frameUpdated(id);
}

void
FrameWrapper::slotRenderingStopped(const QString& id)
{
    if (id != id_) {
        return;
    }

    QObject::disconnect(renderConnections_.updated);
    QObject::disconnect(renderConnections_.stopped);
    renderer_ = nullptr;

    /**
     * The object's QImage pointer is reset before renderingStopped
     * is emitted, allowing the listener to invoke specific behavior
     * like clearing the widget or changing the UI entirely.
     */
    image_.reset();

    isRendering_ = false;

    emit renderingStopped(id);
}

RenderManager::RenderManager(AVModel& avModel)
    :avModel_(avModel)
{
    deviceListSize_ = avModel_.getDevices().size();
    connect(&avModel_, &lrc::api::AVModel::deviceEvent,
            this, &RenderManager::slotDeviceEvent);

    previewFrameWrapper_ = std::make_unique<FrameWrapper>(avModel_);

    avModel_.useAVFrame(true);

    QObject::connect(previewFrameWrapper_.get(),
        &FrameWrapper::renderingStarted,
        [this](const QString& id) {
            Q_UNUSED(id);
            emit previewRenderingStarted();
        });
    QObject::connect(previewFrameWrapper_.get(),
        &FrameWrapper::frameUpdated,
        [this](const QString& id) {
            Q_UNUSED(id);
            emit previewFrameUpdated();
        });
    QObject::connect(previewFrameWrapper_.get(),
        &FrameWrapper::renderingStopped,
        [this](const QString& id) {
            Q_UNUSED(id);
            emit previewRenderingStopped();
        });

    previewFrameWrapper_->connectStartRendering();
}

RenderManager::~RenderManager()
{
    previewFrameWrapper_.reset();

    for (auto& dfw : distantFrameWrapperMap_) {
        dfw.second.reset();
    }
}

bool
RenderManager::isPreviewing()
{
    return previewFrameWrapper_->isRendering();
}

QImage*
RenderManager::getPreviewFrame()
{
    return previewFrameWrapper_->getFrame();
}

void RenderManager::stopPreviewing(bool async)
{
    if (!previewFrameWrapper_->isRendering()) {
        return;
    }

    if (async) {
        QtConcurrent::run([this] { avModel_.stopPreview(); });
    } else {
        avModel_.stopPreview();
    }
}

void RenderManager::startPreviewing(bool force, bool async)
{
    if (previewFrameWrapper_->isRendering() && !force) {
        return;
    }

    auto restart = [this] {
        if (previewFrameWrapper_->isRendering()) {
            avModel_.stopPreview();
        }
        avModel_.startPreview();
    };
    if (async) {
        QtConcurrent::run(restart);
    } else {
        restart();
    }
}

QImage*
RenderManager::getFrame(const QString& id)
{
    auto dfwIt = distantFrameWrapperMap_.find(id);
    if (dfwIt != distantFrameWrapperMap_.end()) {
        return dfwIt->second->getFrame();
    }
    return nullptr;
}

void
RenderManager::addDistantRenderer(const QString& id)
{
    // check if a FrameWrapper with this id exists
    auto dfwIt = distantFrameWrapperMap_.find(id);
    if ( dfwIt != distantFrameWrapperMap_.end()) {
        if (!dfwIt->second->startRendering()) {
            qWarning() << "Couldn't start rendering for id: " << id;
        }
    } else {
        auto dfw = std::make_unique<FrameWrapper>(avModel_, id);

        // connect this to the FrameWrapper
        distantConnectionMap_[id].started = QObject::connect(
            dfw.get(), &FrameWrapper::renderingStarted,
            [this](const QString& id) {
                emit distantRenderingStarted(id);
            });
        distantConnectionMap_[id].updated = QObject::connect(
            dfw.get(), &FrameWrapper::frameUpdated,
            [this](const QString& id) {
                emit distantFrameUpdated(id);
            });
        distantConnectionMap_[id].stopped = QObject::connect(
            dfw.get(), &FrameWrapper::renderingStopped,
            [this](const QString& id) {
                emit distantRenderingStopped(id);
            });

        // connect FrameWrapper to avmodel
        dfw->connectStartRendering();

        // add to map
        distantFrameWrapperMap_.insert(std::make_pair(id, std::move(dfw)));
    }
}

void
RenderManager::removeDistantRenderer(const QString& id)
{
    auto dfwIt = distantFrameWrapperMap_.find(id);
    if (dfwIt != distantFrameWrapperMap_.end()) {
        // disconnect FrameWrapper from this
        auto dcIt = distantConnectionMap_.find(id);
        if (dcIt != distantConnectionMap_.end()) {
            QObject::disconnect(dcIt->second.started);
            QObject::disconnect(dcIt->second.updated);
            QObject::disconnect(dcIt->second.stopped);
        }

        // erase
        distantFrameWrapperMap_.erase(dfwIt);
    }
}

void
RenderManager::slotDeviceEvent()
{
    auto defaultDevice = avModel_.getDefaultDevice();
    auto currentCaptureDevice = avModel_.getCurrentVideoCaptureDevice();
    // decide whether a device has plugged, unplugged, or nothing has changed
    auto deviceList = avModel_.getDevices();
    auto currentDeviceListSize = deviceList.size();

    DeviceEvent deviceEvent {DeviceEvent::None};
    if (currentDeviceListSize > deviceListSize_) {
        deviceEvent = DeviceEvent::Added;
    } else if (currentDeviceListSize < deviceListSize_) {
        // check if the currentCaptureDevice is still in the device list
        if (std::find(std::begin(deviceList), std::end(deviceList), currentCaptureDevice)
            == std::end(deviceList)) {
            deviceEvent = DeviceEvent::RemovedCurrent;
        }
    }

    if (previewFrameWrapper_->isRendering()) {
        if (currentDeviceListSize == 0) {
            avModel_.clearCurrentVideoCaptureDevice();
            avModel_.switchInputTo({});
            stopPreviewing();
        } else if (deviceEvent == DeviceEvent::RemovedCurrent &&
                   currentDeviceListSize > 0) {
            avModel_.setCurrentVideoCaptureDevice(defaultDevice);
            startPreviewing(true);
        } else {
            startPreviewing();
        }
    } else if (deviceEvent == DeviceEvent::Added &&
               currentDeviceListSize == 1) {
        avModel_.setCurrentVideoCaptureDevice(defaultDevice);
    }

    emit videoDeviceListChanged();

    deviceListSize_ = currentDeviceListSize;
}
