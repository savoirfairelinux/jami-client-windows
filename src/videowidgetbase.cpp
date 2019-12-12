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

    LRCInstance::renderer()->setIsToUseAVFrame(false);
}

GLVideoWidgetBase::GLVideoWidgetBase(QColor bgColor, QWidget* parent)
    : QOpenGLWidget(parent)
    , sizeTexture(16, 9)
    , linesizeWidthScaleFactors(1.0f,1.0f,1.0f)
{
    Q_UNUSED(bgColor);
    vbo_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo_ = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    shaderProgram_  = new QOpenGLShaderProgram(this);
    frameTex_.Ytex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
    frameTex_.Utex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
    frameTex_.Vtex = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
    frameTex_.UVtex_NV12 = new QOpenGLTexture(QOpenGLTexture::Target::Target2D);
}

GLVideoWidgetBase::~GLVideoWidgetBase()
{
    makeCurrent();
    vbo_->destroy();
    ibo_->destroy();
    frameTex_.Ytex->destroy();
    frameTex_.Utex->destroy();
    frameTex_.Vtex->destroy();
    delete frameTex_.Ytex;
    delete frameTex_.Utex;
    delete frameTex_.Vtex;
    delete shaderProgram_;
    delete vbo_;
    delete ibo_;
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

    QWindow* window = this->window()->windowHandle();
    glViewport(0, 0, width() * window->devicePixelRatio(), height() * window->devicePixelRatio());
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
    GLuint posUV_NV12tex = shaderProgram_->uniformLocation("UVtex_NV12");
    shaderProgram_->setUniformValue(posUV_NV12tex, 3);

    shaderProgram_->release();
}

void
GLVideoWidgetBase::paintGL()
{
    QWindow* window = this->window()->windowHandle();
    glViewport(0, 0, width() * window->devicePixelRatio(), height() * window->devicePixelRatio());
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram_->bind();
    GLuint m_angleToRotate = shaderProgram_->uniformLocation("aAngleToRotate");
    shaderProgram_->setUniformValue(m_angleToRotate,180.0f + angleToRotate);
    GLuint m_sizeOfTexture = shaderProgram_->uniformLocation("aWidthAndHeight");
    shaderProgram_->setUniformValue(m_sizeOfTexture, sizeTexture);
    GLuint m_sizeViewPort = shaderProgram_->uniformLocation("aViewPortWidthAndHeight");
    shaderProgram_->setUniformValue(m_sizeViewPort, QVector2D((GLfloat)width(), (GLfloat)height()));
    GLuint m_widthScalingFactors = shaderProgram_->uniformLocation("vTextureCoordScalingFactors");
    shaderProgram_->setUniformValue(m_widthScalingFactors, linesizeWidthScaleFactors);
    GLuint m_isNV12 = shaderProgram_->uniformLocation("isNV12");
    shaderProgram_->setUniformValue(m_isNV12, isNV12_);
    vbo_->bind();
    ibo_->bind();
    {
        QMutexLocker lock(&textureMutex);

        if(frameTex_.Ytex && frameTex_.Ytex->isCreated() && frameTex_.Utex && frameTex_.Utex->isCreated() && frameTex_.Vtex && frameTex_.Vtex->isCreated()) {
            frameTex_.Ytex->bind(0);
            frameTex_.Utex->bind(1);
            frameTex_.Vtex->bind(2);
        } else if(frameTex_.Ytex && frameTex_.Ytex->isCreated() && frameTex_.UVtex_NV12 && frameTex_.UVtex_NV12->isCreated()) {
            frameTex_.Ytex->bind(0);
            frameTex_.UVtex_NV12->bind(3);
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if(frameTex_.Ytex && frameTex_.Ytex->isCreated() && frameTex_.Utex && frameTex_.Utex->isCreated() && frameTex_.Vtex && frameTex_.Vtex->isCreated()) {
            frameTex_.Ytex->release();
            frameTex_.Utex->release();
            frameTex_.Vtex->release();
        } else if(frameTex_.Ytex && frameTex_.Ytex->isCreated() && frameTex_.UVtex_NV12 && frameTex_.UVtex_NV12->isCreated()) {
            frameTex_.Ytex->release();
            frameTex_.UVtex_NV12->release();
        }
    }
    ibo_->release();
    vbo_->release();
    shaderProgram_->release();
}

void
GLVideoWidgetBase::resizeGL(int w, int h)
{
    QWindow* window = this->window()->windowHandle();
    glViewport(0, 0, w * window->devicePixelRatio(), h * window->devicePixelRatio());
}

void
GLVideoWidgetBase::initializeTexture(QOpenGLTexture* texture, QOpenGLTexture::TextureFormat textureFormat, QOpenGLTexture::PixelFormat pixelFormat,int width, int height, uint8_t* data)
{
    QMutexLocker lock(&textureMutex);

    if(!texture) return;
    if(texture->isCreated()) {
        if((texture->width() == width) && (texture->height() == height)) {
            goto DATASET;
        };
        texture->destroy();
    }

    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setSize(width, height);
    texture->setFormat(textureFormat);
    texture->create();
    texture->allocateStorage();

DATASET:
    texture->setData(0, 0, pixelFormat, QOpenGLTexture::PixelType::UInt8, data);
}

void
GLVideoWidgetBase::initializeTextureWithCUDA(QOpenGLTexture* texture, QOpenGLTexture::TextureFormat textureFormat, int width, int height, int linesize,
                                             CUdeviceptr startPtr, CUcontext cudaContext)
{
    QMutexLocker lock(&textureMutex);

    if(!texture) return;
    if(texture->isCreated()) {
        if((texture->width() == width) && (texture->height() == height) &&(texture->format() == textureFormat)) {
            goto COPYDATA;
        }
        texture->destroy();
    }
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setSize(width, height);
    texture->setFormat(textureFormat);
    texture->create();
    texture->allocateStorage();

COPYDATA:
    CUcontext pctx;
    cuCtxGetCurrent(&pctx);
    if(pctx != cudaContext) cuCtxPushCurrent(cudaContext);
    // copy the frame to cuda graphic resource
    CUgraphicsResource cudaResourceForCurrentTexture;
    cuGraphicsGLRegisterImage(&cudaResourceForCurrentTexture, texture->textureId(), GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_NONE);
    cuGraphicsResourceSetMapFlags(cudaResourceForCurrentTexture, CU_GRAPHICS_MAP_RESOURCE_FLAGS_WRITE_DISCARD);
    cuGraphicsMapResources(1, &cudaResourceForCurrentTexture, 0);
    CUarray dstArray;
    cuGraphicsSubResourceGetMappedArray(&dstArray, cudaResourceForCurrentTexture, 0, 0);
    CUDA_MEMCPY2D memCopyParameter;
    memset(&memCopyParameter, 0, sizeof(memCopyParameter));
    memCopyParameter.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    memCopyParameter.srcDevice = startPtr;
    memCopyParameter.srcPitch = linesize * sizeof(uint8_t);
    memCopyParameter.dstMemoryType = CU_MEMORYTYPE_ARRAY;
    memCopyParameter.dstArray = dstArray;
    memCopyParameter.WidthInBytes = width * sizeof(uint8_t);
    memCopyParameter.Height = height;
    cuMemcpy2D(&memCopyParameter);

    cuGraphicsUnmapResources(1, &cudaResourceForCurrentTexture, 0);
    cuGraphicsUnregisterResource(cudaResourceForCurrentTexture);
}

bool
GLVideoWidgetBase::updateTextureFromCUDA(AVFrame* frame)
{
    bool result = false;
    if((AVPixelFormat)frame->format != AVPixelFormat::AV_PIX_FMT_CUDA) return false;
    // get the cuda context
    CUcontext cudaContext;

    AVHWFramesContext* avHWFramesContext = (AVHWFramesContext*)(frame->hw_frames_ctx->data);
    if(!avHWFramesContext) return false;
    auto device_ctx = static_cast<AVHWDeviceContext*>(avHWFramesContext->device_ctx);
    if(!device_ctx || !static_cast<AVCUDADeviceContext*>(device_ctx->hwctx)) return false;
    AVCUDADeviceContext* avCUDADeviceContext = static_cast<AVCUDADeviceContext*>(device_ctx->hwctx);
    if(!avCUDADeviceContext) return false;
    cudaContext = avCUDADeviceContext->cuda_ctx;
    if(!cudaContext) return false;

    if((AVPixelFormat)avHWFramesContext->sw_format == AVPixelFormat::AV_PIX_FMT_NV12) {
        CUdeviceptr pointerToDataBufferY = (CUdeviceptr)frame->data[0];
        initializeTextureWithCUDA(frameTex_.Ytex, QOpenGLTexture::LuminanceFormat,frame->width,frame->height,frame->linesize[0],
                                  pointerToDataBufferY, cudaContext);
        CUdeviceptr pointerToDataBufferUV = (CUdeviceptr)frame->data[1];
        initializeTextureWithCUDA(frameTex_.UVtex_NV12, QOpenGLTexture::RG8_UNorm, frame->width, frame->height / 2, frame->linesize[1],
                                  pointerToDataBufferUV, cudaContext);

        setIsNV12(true);
        result = true;
    }

    return result;
}

void
GLVideoWidgetBase::initializeShaderProgram()
{
    bool success = true;
    QString vsFile(":/shader/YUVConv.vert");
    QString fsFile(":/shader/YUVConv.frag");
    success = shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, vsFile);
    if(!success) {
        qFatal("adding vertex shaders fails");
    }
    success = shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, fsFile);
    if(!success) {
        qFatal("adding fragment shaders fails");
    }
    success = shaderProgram_->link();
    if (!success) {
        qFatal("linking shaders fails");
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
}

bool
GLVideoWidgetBase::updateTextures(AVFrame* frame)
{
    bool result = false;
    if (!frame || !frame->width || !frame->height) {
        return false;
    }

    double rotation = 0;
    if(auto matrix = av_frame_get_side_data(frame, AV_FRAME_DATA_DISPLAYMATRIX)) {
        const int32_t* data = reinterpret_cast<int32_t*>(matrix->data);
        rotation = av_display_rotation_get(data);
    }

    angleToRotate = rotation;
    sizeTexture = QVector2D(frame->width, frame->height);

    if(frame->linesize[0] && frame->linesize[1] && frame->linesize[2]) {
        setIsNV12(false);
        if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV420P) {
            initializeTexture(frameTex_.Ytex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[0], frame->height, frame->data[0]);
            initializeTexture(frameTex_.Utex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance,frame->linesize[1], frame->height / 2, frame->data[1]);
            initializeTexture(frameTex_.Vtex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[2], frame->height / 2, frame->data[2]);
            linesizeWidthScaleFactors.setX((GLfloat)frame->width / (GLfloat)frame->linesize[0]);
            linesizeWidthScaleFactors.setY((GLfloat)frame->width / 2 / (GLfloat)frame->linesize[1]);
            linesizeWidthScaleFactors.setZ((GLfloat)frame->width / 2 / (GLfloat)frame->linesize[2]);
            result = true;
        } else if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV422P) {
            initializeTexture(frameTex_.Ytex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[0], frame->height, frame->data[0]);
            initializeTexture(frameTex_.Utex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[1], frame->height, frame->data[1]);
            initializeTexture(frameTex_.Vtex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[2], frame->height, frame->data[2]);
            linesizeWidthScaleFactors.setX((GLfloat)frame->width / (GLfloat)frame->linesize[0]);
            linesizeWidthScaleFactors.setY((GLfloat)frame->width / 2 / (GLfloat)frame->linesize[1]);
            linesizeWidthScaleFactors.setZ((GLfloat)frame->width / 2 / (GLfloat)frame->linesize[2]);
            result = true;
        } else if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_YUV444P) {
            initializeTexture(frameTex_.Ytex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[0], frame->height, frame->data[0]);
            initializeTexture(frameTex_.Utex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[1], frame->height, frame->data[1]);
            initializeTexture(frameTex_.Vtex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[2], frame->height, frame->data[2]);
            linesizeWidthScaleFactors.setX((GLfloat)frame->width / (GLfloat)frame->linesize[0]);
            linesizeWidthScaleFactors.setY((GLfloat)frame->width / (GLfloat)frame->linesize[1]);
            linesizeWidthScaleFactors.setZ((GLfloat)frame->width / (GLfloat)frame->linesize[2]);
            result = true;
        }
        return result;
    }

    if(frame->linesize[0] && frame->linesize[1]) {
        // the format is NV12
        if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_NV12) {
            initializeTexture(frameTex_.Ytex, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, frame->linesize[0], frame->height, frame->data[0]);
            initializeTexture(frameTex_.UVtex_NV12, QOpenGLTexture::RG8_UNorm, QOpenGLTexture::RG, frame->linesize[1], frame->height / 2, frame->data[1]);
            setIsNV12(true);
            result = true;
        }
    }

    if(frame->linesize[0] && frame->linesize[1]) {
        // all hardware accelerated formats are NV12
        if(AVPixelFormat(frame->format) == AVPixelFormat::AV_PIX_FMT_CUDA) {
            // support the CUDA hardware accel frame
            result = updateTextureFromCUDA(frame);
        } else {
            result = false;
        }
    }

    return result;
}

void
GLVideoWidgetBase::hideEvent(QHideEvent* e)
{
    QOpenGLWidget::hideEvent(e);
    emit visibilityChanged(false);

    clearFrameTextures();
}

void
GLVideoWidgetBase::showEvent(QShowEvent* e)
{
    QOpenGLWidget::showEvent(e);
    emit visibilityChanged(true);
    LRCInstance::renderer()->setIsToUseAVFrame(true);

    clearFrameTextures();
}

void
GLVideoWidgetBase::closeEvent(QCloseEvent* e)
{
    QOpenGLWidget::closeEvent(e);
    clearFrameTextures();
}

void
GLVideoWidgetBase::setIsNV12(bool isNV12)
{
    if(isNV12 != isNV12_) {
        clearFrameTextures();
        isNV12_ = isNV12;
    }
}

void
GLVideoWidgetBase::clearFrameTextures()
{
    {
        QMutexLocker lock(&textureMutex);
        makeCurrent();
        if(frameTex_.Ytex && frameTex_.Ytex->isCreated()) {
            frameTex_.Ytex->destroy();
        }
        if(frameTex_.UVtex_NV12 && frameTex_.UVtex_NV12->isCreated()) {
            frameTex_.UVtex_NV12->destroy();
        }
        if(frameTex_.Utex && frameTex_.Utex->isCreated()) {
            frameTex_.Utex->destroy();
        }
        if(frameTex_.Vtex && frameTex_.Vtex->isCreated()) {
            frameTex_.Vtex->destroy();
        }
    }
}
