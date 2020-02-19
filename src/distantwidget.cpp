/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "distantwidget.h"

#include "lrcinstance.h"
#include "utils.h"

DistantWidget::DistantWidget(QWidget* parent)
    : VideoWidgetBase(Qt::black, parent)
{
    connect(LRCInstance::renderer(), &RenderManager::distantFrameUpdated,
        [this](const QString& id) {
            if (id_ == id) repaint();
        });
    connect(LRCInstance::renderer(), &RenderManager::distantRenderingStopped,
        [this](const QString& id) {
            if (id_ == id) repaint();
        });
}

DistantWidget::~DistantWidget()
{
}

void
DistantWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto distantImage = LRCInstance::renderer()->getFrame(id_);
    if (distantImage) {
        auto scaledDistant = distantImage->scaled(size(), Qt::KeepAspectRatio);
        auto xDiff = (width() - scaledDistant.width()) / 2;
        auto yDiff = (height() - scaledDistant.height()) / 2;
        painter.drawImage(QRect(xDiff, yDiff,
                                scaledDistant.width(), scaledDistant.height()),
                          scaledDistant);
    } else {
        paintBackground(&painter);
    }
    painter.end();
}

void
DistantWidget::paintBackground(QPainter* painter)
{
    QBrush brush(Qt::black);
    QPainterPath path;
    path.addRect(this->rect());
    painter->fillPath(path, brush);
}

void
DistantWidget::setRendererId(const QString& id)
{
    id_ = id;
    update();
}
