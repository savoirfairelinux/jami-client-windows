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
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 1.0f, 0.0f
};

const int g_indices[] = { 0,1,2,2,3,0 };

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
GLVideoWidgetBase::initializeGL()
{
    initializeOpenGLFunctions();
    glViewport(0, 0, this->width(), this->height());

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "GL Initialize";

    initializeShaderProgram();
    initializeTexture();
    setUpBuffers();
}

void
GLVideoWidgetBase::paintGL()
{
}

void
GLVideoWidgetBase::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void
GLVideoWidgetBase::initializeTexture()
{
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
    GLuint m_posAttr = shaderProgram_->attributeLocation("posAttr");
    shaderProgram_->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_posAttr);

    GLuint m_textureCor = shaderProgram_->attributeLocation("textureCoordinate");
    shaderProgram_->setAttributeBuffer(m_textureCor, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
    shaderProgram_->enableAttributeArray(m_textureCor);

    vbo_->release();

    ibo_->create();
    ibo_->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_->bind();
    ibo_->allocate(g_indices, sizeof(g_indices));
    ibo_->release();

    shaderProgram_->release();
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