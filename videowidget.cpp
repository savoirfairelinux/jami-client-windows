/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "videowidget.h"

#include "utils.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

VideoWidget::~VideoWidget()
{}

void
VideoWidget::slotDistantRendererStarted(const std::string& id)
{
    Q_UNUSED(id);

    QObject::disconnect(rendererDistantConnections_.started);

    this->show();

    QObject::disconnect(rendererDistantConnections_.updated);
    rendererDistantConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        this,
        &VideoWidget::slotUpdateDistantView);

    QObject::disconnect(rendererDistantConnections_.stopped);
    rendererDistantConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        this,
        &VideoWidget::slotStopDistantView);
}

void
VideoWidget::renderFrame(const std::string& id)
{
    auto avModel = &LRCInstance::avModel();
    using namespace lrc::api::video;
    auto renderer = &avModel->getRenderer(id);
    if (renderer && renderer->isRendering()) {
        {
            QMutexLocker lock(&mutex_);
            auto tmp  = renderer->currentFrame();
            if (tmp.storage.size()) {
                distantFrame_ = tmp;
            }
        }
        update();
    }
}

void
VideoWidget::slotToggleFullScreenClicked()
{
    this->update();
}

void
VideoWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);

    if (distantRenderer_) {
        {
            QMutexLocker lock(&mutex_);
            if (distantFrame_.storage.size() != 0
                && distantFrame_.storage.size() ==
                (unsigned int)(distantRenderer_->size().height() * distantRenderer_->size().width()*4)) {
                frameDistant_ = std::move(distantFrame_.storage);
                distantImage_.reset(
                    new QImage((uchar*)frameDistant_.data(),
                        distantRenderer_->size().width(),
                        distantRenderer_->size().height(),
                        QImage::Format_ARGB32_Premultiplied)
                );
            }
        }
        if (distantImage_) {
            auto scaledDistant = distantImage_->scaled(size(), Qt::KeepAspectRatio);
            auto xDiff = (width() - scaledDistant.width()) / 2;
            auto yDiff = (height() - scaledDistant.height()) / 2;
            painter.drawImage(QRect(
                xDiff, yDiff, scaledDistant.width(), scaledDistant.height()), scaledDistant
            );
        }
    }
    painter.end();
}

void
VideoWidget::connectDistantRendering()
{
    rendererDistantConnections_.started = connect(
        &LRCInstance::avModel(),
        SIGNAL(rendererStarted(const std::string&)),
        this,
        SLOT(slotDistantRendererStarted(const std::string&))
    );
}

void
VideoWidget::disconnectRendering()
{
    QObject::disconnect(rendererDistantConnections_.started);
    QObject::disconnect(rendererDistantConnections_.stopped);
    QObject::disconnect(rendererDistantConnections_.updated);
}

void
VideoWidget::slotUpdateDistantView(const std::string& id)
{
    if (id == lrc::api::video::PREVIEW_RENDERER_ID) {
        return;
    }
    auto avModel = &LRCInstance::avModel();
    auto renderer = &avModel->getRenderer(id);
    if (!renderer->isRendering()) {
        return;
    }
    distantRenderer_ = const_cast<lrc::api::video::Renderer*>(renderer);
    renderFrame(id);
}

void
VideoWidget::slotStopDistantView(const std::string& id)
{
    QObject::disconnect(rendererDistantConnections_.updated);
    QObject::disconnect(rendererDistantConnections_.stopped);
    using namespace lrc::api::video;
    distantRenderer_ = nullptr;
    repaint();
}
