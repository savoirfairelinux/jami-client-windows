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

#include "videowidgetbase.h"

#include "lrcinstance.h"

extern "C" {
#include "libavutil/frame.h"
#include "libavutil/display.h"
}

const GLfloat verticesCorTexCor[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

const int g_indices[] = {0, 1, 2, 2, 3, 0};

VideoWidgetBase::VideoWidgetBase(QColor bgColor, QWidget* parent)
    : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, bgColor);
    setAutoFillBackground(true);
    setPalette(pal);
}

VideoWidgetBase::~VideoWidgetBase()
{}

void
VideoWidgetBase::forceRepaint()
{
    auto parent = qobject_cast<QWidget*>(this->parent());
    if (parent) parent->setUpdatesEnabled(false);
    repaint();
    if (parent) parent->setUpdatesEnabled(true);
}

void
VideoWidgetBase::hideEvent(QHideEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(false);
}

void
VideoWidgetBase::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(true);
}

GLVideoWidgetBase::GLVideoWidgetBase(QColor bgColor, QWidget* parent)
    : QOpenGLWidget(parent)
    , sizeTexture(16, 9)
{
    Q_UNUSED(bgColor);
    vbo_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo_ = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    shaderProgram_  = new QOpenGLShaderProgram(this);
    frameTex_.Ytex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
    frameTex_.Utex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
    frameTex_.Vtex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
}

GLVideoWidgetBase::~GLVideoWidgetBase()
{
    makeCurrent();
    vbo_->destroy();
    ibo_->destroy();
    frameTex_.Ytex->destroy();
    frameTex_.Utex->destroy();
    frameTex_.Vtex->destroy();
    delete shaderProgram_;
    shaderProgram_ = nullptr;
    doneCurrent();
}

void
GLVideoWidgetBase::forceRepaint()
{
    auto parent = qobject_cast<QWidget*>(this->parent());
    if (parent) parent->setUpdatesEnabled(false);
    update();
    if (parent) parent->setUpdatesEnabled(true);
}

void
GLVideoWidgetBase::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "GL Initialize";

    initializeShaderProgram();
    setUpBuffers();
    shaderProgram_->bind();
    GLuint posYtex = shaderProgram_->uniformLocation("Ytex");
    shaderProgram_->setUniformValue(posYtex, 0);
    GLuint posUtex = shaderProgram_->uniformLocation("Utex");
    shaderProgram_->setUniformValue(posUtex, 1);
    GLuint posVtex = shaderProgram_->uniformLocation("Vtex");
    shaderProgram_->setUniformValue(posVtex, 2);

    shaderProgram_->release();
    frameTex_.Ytex->create();
    frameTex_.Utex->create();
    frameTex_.Vtex->create();
}

void
GLVideoWidgetBase::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (updateFrameTexture) {
        updateFrameTexture = false;
    }
    shaderProgram_->bind();
    GLuint m_angleToRotate = shaderProgram_->uniformLocation("aAngleToRotate");
    shaderProgram_->setUniformValue(m_angleToRotate,180.0f + angleToRotate);
    GLuint m_sizeOfTexture = shaderProgram_->uniformLocation("aWidthAndHeight");
    shaderProgram_->setUniformValue(m_sizeOfTexture, sizeTexture);
    GLuint m_sizeViewPort = shaderProgram_->uniformLocation("aViewPortWidthAndHeight");
    shaderProgram_->setUniformValue(m_sizeViewPort, QVector2D((GLfloat)width(), (GLfloat)height()));
    vbo_->bind();
    ibo_->bind();
    frameTex_.Ytex->bind(0);
    frameTex_.Utex->bind(1);
    frameTex_.Vtex->bind(2);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    frameTex_.Ytex->release();
    frameTex_.Utex->release();
    frameTex_.Vtex->release();
    ibo_->release();
    vbo_->release();
    shaderProgram_->release();
}

void
GLVideoWidgetBase::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void
GLVideoWidgetBase::initializeTexture(QOpenGLTexture* texture, int width, int height)
{
    texture->release();
    texture->destroy();
    texture->create();

    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setSize(width, height);
    texture->setFormat(QOpenGLTexture::TextureFormat::LuminanceFormat);
    texture->allocateStorage();
}

void
GLVideoWidgetBase::initializeShaderProgram()
{
    bool success = true;
    QDir currentPath = QDir(QCoreApplication::applicationDirPath() + "/../../shader");
    auto vsFile = currentPath.filePath("YUVConv.vert");
    auto fsFile = currentPath.filePath("YUVConv.frag");
    success &= shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFile);
    success &= shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFile);
    success &= shaderProgram_->link();
    if (!success) {
        qDebug() << "adding shaders fails";
    }
}

void
GLVideoWidgetBase::setUpBuffers()
{
    vbo_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo_ = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    shaderProgram_->bind();
    vbo_->create();
    vbo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_->bind();
    vbo_->allocate(verticesCorTexCor, sizeof(verticesCorTexCor));

    GLuint m_posAttr = shaderProgram_->attributeLocation("aPosition");
    shaderProgram_->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_posAttr);

    GLuint m_textureCor = shaderProgram_->attributeLocation("aTextureCoord");
    shaderProgram_->setAttributeBuffer(m_textureCor, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_textureCor);

    ibo_->create();
    ibo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_->bind();
    ibo_->allocate(g_indices, sizeof(g_indices));

    vbo_->release();
    ibo_->release();
    shaderProgram_->release();
}

void
GLVideoWidgetBase::prepareFrameToDisplay(AVFrame* frame)
{
    updateTextures(frame);
    updateFrameTexture = true;
}

bool
GLVideoWidgetBase::updateTextures(AVFrame* frame)
{
    if (!frame || !frame->width || !frame->height) {
        return false;
    }

    double rotation = 0;
    if(auto matrix = av_frame_get_side_data(frame, AV_FRAME_DATA_DISPLAYMATRIX)) {
        const int32_t* data = reinterpret_cast<int32_t*>(matrix->data);
        rotation = av_display_rotation_get(data);
    }

    angleToRotate = -rotation;
    sizeTexture = QVector2D(frame->width, frame->height);

    initializeTexture(frameTex_.Ytex, frame->width, frame->height);
    if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV420P) {
        initializeTexture(frameTex_.Utex, frame->width / 2, frame->height / 2);
        initializeTexture(frameTex_.Vtex, frame->width / 2, frame->height / 2);
    } else if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV422P) {
        initializeTexture(frameTex_.Utex, frame->width / 2, frame->height);
        initializeTexture(frameTex_.Vtex, frame->width / 2, frame->height);
    } else if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV444P) {
        initializeTexture(frameTex_.Utex, frame->width, frame->height);
        initializeTexture(frameTex_.Vtex, frame->width, frame->height);
    } else {
        qWarning() << "The format is not supportted, please set it to YUV format in YUV420P, YUV422P or YUV444P!";
        return false;
    }

    frameTex_.Ytex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, (uint8_t*)frame->data[0]);
    frameTex_.Utex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, (uint8_t*)frame->data[1]);
    frameTex_.Vtex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, (uint8_t*)frame->data[2]);

    return true;
}

void
GLVideoWidgetBase::hideEvent(QHideEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(false);
}

void
GLVideoWidgetBase::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    emit visibilityChanged(true);
}