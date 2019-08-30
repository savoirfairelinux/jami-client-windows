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

#include "videowidget.h"

#include "utils.h"

VideoWidget::VideoWidget(QWidget* parent) :
    QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //previewWidget_ = new VideoPreviewWidget(this);
}

VideoWidget::~VideoWidget()
{}

void
VideoWidget::slotRendererStarted(const std::string& id)
{
    Q_UNUSED(id);

    QObject::disconnect(rendererConnections_.started);

    // only one videowidget will be used at the same time
    if (not isVisible())
        return;

    this->show();

    QObject::disconnect(rendererConnections_.updated);
    rendererConnections_.updated = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::frameUpdated,
        [this](const std::string& id) {
            auto avModel = &LRCInstance::avModel();
            auto renderer = &avModel->getRenderer(id);
            if (!renderer->isRendering()) {
                return;
            }
            using namespace lrc::api::video;
            if (id != PREVIEW_RENDERER_ID) {
                distantRenderer_ = const_cast<Renderer*>(renderer);
            }
            renderFrame(id);
        });

    QObject::disconnect(rendererConnections_.stopped);
    rendererConnections_.stopped = connect(
        &LRCInstance::avModel(),
        &lrc::api::AVModel::rendererStopped,
        [this](const std::string& id) {
            QObject::disconnect(rendererConnections_.updated);
            QObject::disconnect(rendererConnections_.stopped);
            using namespace lrc::api::video;
            if (id != PREVIEW_RENDERER_ID) {
                distantRenderer_ = nullptr;
            }
            repaint();
        });
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
                using namespace lrc::api::video;
                if (id != PREVIEW_RENDERER_ID) {
                    distantFrame_ = tmp;
                }
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
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (distantRenderer_) {
        {
            QMutexLocker lock(&mutex_);
            if (distantFrame_.storage.size() != 0
                && distantFrame_.storage.size() == (unsigned int)(distantRenderer_->size().height() * distantRenderer_->size().width() * 4)) {
                frameDistant_ = std::move(distantFrame_.storage);
                distantImage_.reset(
                    new QImage((uchar*)frameDistant_.data(),
                        distantRenderer_->size().width(),
                        distantRenderer_->size().height(),
                        QImage::Format_ARGB32_Premultiplied));
            }
        }
        if (distantImage_) {
            auto scaledDistant = distantImage_->scaled(size(), Qt::KeepAspectRatio);
            auto xDiff = (width() - scaledDistant.width()) / 2;
            auto yDiff = (height() - scaledDistant.height()) / 2;
            painter.drawImage(QRect(
                                  xDiff, yDiff, scaledDistant.width(), scaledDistant.height()),
                scaledDistant);
        }
    }
    painter.end();
}

void
VideoWidget::connectRendering()
{
    rendererConnections_.started = connect(
        &LRCInstance::avModel(),
        SIGNAL(rendererStarted(const std::string&)),
        this,
        SLOT(slotRendererStarted(const std::string&))
    );
}
