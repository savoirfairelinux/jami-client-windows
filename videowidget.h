/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QPainter>

#include <video/renderer.h>
#include <video/previewmanager.h>
#include <callmodel.h>
#include <QMutex>
#include <QPixmap>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

    void paintEvent(QPaintEvent* evt);
signals:

public slots:
    void previewStarted(Video::Renderer* renderer);
    void previewStopped(Video::Renderer* renderer);
    void frameFromPreview();
    void callInitiated(Call *call, Video::Renderer *renderer);
    void frameFromDistant();
    void renderingStopped();
private:
    Video::Renderer* previewRenderer_;
    Video::Renderer* renderer_;
    QImage *previewFrame_;
    QImage *distantFrame_;
    QMutex lock_;
    QPixmap *previewPix_;
};

#endif // VIDEOWIDGET_H
