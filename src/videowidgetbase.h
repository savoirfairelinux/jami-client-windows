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

#pragma once

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

extern "C" {
#include <libavutil/frame.h>
}

// The base for video widgets
class VideoWidgetBase : public QWidget {
    Q_OBJECT;

public:
    explicit VideoWidgetBase(QColor bgColor = Qt::transparent,
                             QWidget* parent = 0);
    virtual ~VideoWidgetBase();

    /**
     * Repaints the widget while preventing update/repaint to queue
     * for its parent. This is needed when geometry changes occur,
     * to disable image tearing.
     */
    void forceRepaint();

signals:
    void visibilityChanged(bool visible);

protected:
    virtual void hideEvent(QHideEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;

    virtual void paintBackground(QPainter* painter) = 0;

};

struct YUVFrameTexture
{
    QOpenGLTexture* Ytex;
    QOpenGLTexture* Utex;
    QOpenGLTexture* Vtex;
};

class GLVideoWidgetBase : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT;

public:
    explicit GLVideoWidgetBase(QColor bgColor = Qt::transparent,
        QWidget* parent = 0);
    virtual ~GLVideoWidgetBase();

    /**
     * Repaints the widget while preventing update/repaint to queue
     * for its parent. This is needed when geometry changes occur,
     * to disable image tearing.
     */
    void forceRepaint();

signals:
    void visibilityChanged(bool visible);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual void initializeTexture(QOpenGLTexture* texture, int width, int height, int textureUnit);
    virtual void initializeShaderProgram();
    virtual void setUpBuffers();

    virtual void prepareFrameToDisplay(AVFrame* frame);

    virtual bool updateTextures();

    virtual void hideEvent(QHideEvent* e) override;
    virtual void showEvent(QShowEvent* e) override;

    //virtual void paintBackground(QPainter* painter) = 0;

protected:
    QOpenGLShaderProgram* shaderProgram_;
    QOpenGLBuffer* vbo_;
    QOpenGLBuffer* ibo_;
    YUVFrameTexture frameTex_;
    bool updateFrameTexture = false;
    AVFrame* frame_;
};