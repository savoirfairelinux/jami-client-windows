/***************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "videowidget.h"

#include "video/renderer.h"

///Constructor
VideoWidget::VideoWidget(QWidget* parent , Video::Renderer* renderer) : QWidget(parent),m_Image(nullptr),m_pRenderer(renderer) {
    setMinimumSize(200,200);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

///Set widget renderer
void VideoWidget::setRenderer(Video::Renderer* renderer)
{
    if (m_pRenderer) {
        disconnect(m_pRenderer, SIGNAL(frameUpdated()), this, SLOT(updateFrame()));
        disconnect(m_pRenderer, SIGNAL(stopped()), this, SLOT(stop()));
    }
    m_pRenderer = renderer;
    connect(m_pRenderer, SIGNAL(frameUpdated()), this, SLOT(updateFrame()));
    connect(m_pRenderer, SIGNAL(stopped()), this, SLOT(stop()));
}

///Repaint the widget
void VideoWidget::update() {
    QPainter painter(this);
    if (m_Image && m_pRenderer->isRendering())
        painter.drawImage(QRect(0, 0, this->width(), this->height()), m_Image->scaled(this->size()));
    painter.end();
}

///Called when the widget need repainting
void VideoWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    update();
}

///Called when a new frame is ready
void VideoWidget::updateFrame()
{
    const auto size = m_pRenderer->size();
    if (m_Image)
        delete m_Image;
    m_Image = new QImage((uchar*)m_pRenderer->currentFrame().data() , size.width(), size.height(), QImage::Format_RGBA8888);
    repaint();
}

///Prevent the painter to try to paint an invalid framebuffer
void VideoWidget::stop()
{
    if (m_pRenderer) {
        disconnect(m_pRenderer, SIGNAL(frameUpdated()), this, SLOT(updateFrame()));
        disconnect(m_pRenderer, SIGNAL(stopped()), this, SLOT(stop()));
        m_pRenderer = nullptr;
    }
    if (m_Image) {
        delete m_Image;
        m_Image = nullptr;
    }
}
