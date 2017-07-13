#ifndef PHOTOBOOTHWIDGET_H
#define PHOTOBOOTHWIDGET_H

#include <QWidget>

namespace Ui {
class PhotoboothWidget;
}

class PhotoboothWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhotoboothWidget(QWidget *parent = 0);
    ~PhotoboothWidget();
    void startBooth();
    void stopBooth();

private slots:
    void on_importButton_clicked();
    void on_takePhotoButton_clicked();

private:
    QString fileName_;
    Ui::PhotoboothWidget *ui;

signals:
    void photoTaken(QString fileName);
};

#endif // PHOTOBOOTHWIDGET_H
