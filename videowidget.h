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
