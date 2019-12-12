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

GLfloat verticesCorTexCor[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f

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

GLVideoWidgetBase::GLVideoWidgetBase(QColor bgColor, QWidget* parent):
                   QOpenGLWidget(parent)
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
    glViewport(0, 0, this->width(), this->height());

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    qDebug() << "GL Initialize";

    initializeShaderProgram();
    setUpBuffers();
    frameTex_.Ytex->create();
    frameTex_.Utex->create();
    frameTex_.Vtex->create();
}

void
GLVideoWidgetBase::paintGL()
{
    glViewport(0, 0, width(), height());
    shaderProgram_->bind();
    vbo_->bind();
    ibo_->bind();
    if(updateFrameTexture) {
        updateFrameTexture = false;
        if (!updateTextures())
            goto StraightDraw;
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
StraightDraw:
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
GLVideoWidgetBase::initializeTexture(QOpenGLTexture* texture, int width, int height, int textureUnit)
{
    if (texture->isCreated()) texture->destroy();
    texture->create();
    texture->bind(textureUnit);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setSize(width, height);
    texture->allocateStorage(QOpenGLTexture::PixelFormat::Luminance,QOpenGLTexture::PixelType::UInt8);


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
    shaderProgram_->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_posAttr);

    GLuint m_textureCor = shaderProgram_->attributeLocation("aTextureCoord");
    shaderProgram_->setAttributeBuffer(m_textureCor, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_textureCor);

    GLuint m_colAttr = shaderProgram_->attributeLocation("aColor");
    shaderProgram_->setAttributeBuffer(m_colAttr, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_colAttr);

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
    frame_ = frame;
    updateFrameTexture = true;
}

bool
GLVideoWidgetBase::updateTextures()
{
    if (!frame_ || !frame_->width || !frame_->height) {
        qWarning()<< "OpenGL Frame update fails!";
        return false;
    }

    initializeTexture(frameTex_.Ytex, frame_->width, frame_->height, 0);
    frameTex_.Ytex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, frame_->data[0]);

    initializeTexture(frameTex_.Utex, frame_->width / 2, frame_->height / 2, 1);
    frameTex_.Utex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, frame_->data[1]);

    initializeTexture(frameTex_.Vtex, frame_->width / 2, frame_->height / 2, 2);
    frameTex_.Vtex->setData(0, 0, QOpenGLTexture::PixelFormat::Luminance, QOpenGLTexture::PixelType::UInt8, frame_->data[2]);

    GLuint posYtex = shaderProgram_->uniformLocation("Ytex");
    shaderProgram_->setUniformValue(posYtex, 0);
    GLuint posUtex = shaderProgram_->uniformLocation("Utex");
    shaderProgram_->setUniformValue(posUtex, 1);
    GLuint posVtex = shaderProgram_->uniformLocation("Vtex");
    shaderProgram_->setUniformValue(posVtex, 2);

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