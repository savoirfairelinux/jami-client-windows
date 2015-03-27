#include "videowidget.h"

#include <QDebug>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent)
  , previewRenderer_(nullptr)
  , renderer_(nullptr)
  , previewFrame_(nullptr)
  , distantFrame_(nullptr)
{
    connect(Video::PreviewManager::instance(), SIGNAL(previewStarted(Video::Renderer*)), this, SLOT(previewStarted(Video::Renderer*)), Qt::ConnectionType::DirectConnection);
    connect(Video::PreviewManager::instance(), SIGNAL(previewStopped(Video::Renderer*)), this, SLOT(previewStopped(Video::Renderer*)), Qt::ConnectionType::DirectConnection);
    connect(CallModel::instance(), SIGNAL(rendererAdded(Call*,Video::Renderer*)), this, SLOT(callInitiated(Call*, Video::Renderer*)), Qt::ConnectionType::DirectConnection);
}

VideoWidget::~VideoWidget()
{

}

void VideoWidget::previewStarted(Video::Renderer *renderer) {
    if (previewRenderer_)
        disconnect(previewRenderer_, 0,0,0);
    previewRenderer_ = renderer;
    connect(previewRenderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromPreview()));
    connect(previewRenderer_, SIGNAL(stopped()), this, SLOT(renderingStopped()), Qt::ConnectionType::DirectConnection);
}

void VideoWidget::previewStopped(Video::Renderer* renderer) {
    Q_UNUSED(renderer)
    disconnect(previewRenderer_, 0,0,0);
    previewRenderer_ = nullptr;
}

void VideoWidget::frameFromPreview() {
    if (previewFrame_) {
        delete previewFrame_;
        previewFrame_ = nullptr;
    }
    if (previewRenderer_ && previewRenderer_->isRendering()) {

        const QSize size(previewRenderer_->size());

        previewFrame_ = new QImage((const uchar*)previewRenderer_->currentFrame().constData(), size.width(), size.height(), QImage::Format_RGBA8888);
    }
    update();
}

void VideoWidget::paintEvent(QPaintEvent* evt) {
    QMutexLocker {&lock_};
    QPainter painter(this);
    //painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);
    if (distantFrame_ && renderer_ && renderer_->isRendering())
          painter.drawImage(QRect(0,0,width(),height()),*(distantFrame_));
    if (previewFrame_ && previewRenderer_ && previewRenderer_->isRendering()) {
        int previewHeight = !renderer_ ? height() : height()/4;
        int previewWidth = !renderer_  ? width() : width()/4;
        int yPos = !renderer_ ? 0 : height() - previewHeight;
        int xPos = !renderer_ ? 0 : width() - previewWidth;
        painter.drawImage(QRect(xPos,yPos,previewWidth,previewHeight), *(previewFrame_));
    }
    painter.end();
}

void VideoWidget::callInitiated(Call* call, Video::Renderer *renderer) {
    Q_UNUSED(call)
    renderer_ = renderer;
    connect(renderer_, SIGNAL(frameUpdated()), this, SLOT(frameFromDistant()));
    connect(renderer_, SIGNAL(stopped()), this, SLOT(renderingStopped()), Qt::ConnectionType::DirectConnection);
}

void VideoWidget::frameFromDistant() {
    QMutexLocker {&lock_};
    if (distantFrame_) {
        delete distantFrame_;
        distantFrame_ = nullptr;
    }
    if (renderer_) {
        const QSize size(renderer_->size());
        distantFrame_ = new QImage((const uchar*) renderer_->currentFrame().constData(), size.width(), size.height(), QImage::Format_RGBA8888);
    }
    update();
}

void VideoWidget::renderingStopped() {
    QMutexLocker {&lock_};
    if (distantFrame_) {
        delete distantFrame_;
        distantFrame_ = nullptr;
    }
    disconnect(renderer_, 0,0,0);
    disconnect(previewRenderer_, 0,0,0);
    previewRenderer_ = nullptr;
    renderer_ = nullptr;
}
