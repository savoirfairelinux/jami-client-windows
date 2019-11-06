#ifndef RECORDWIDGET_H
#define RECORDWIDGET_H

#include <QWidget>

namespace Ui {
class RecordWidget;
}

class RecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecordWidget(QWidget *parent = nullptr);
    ~RecordWidget();

private:
    Ui::RecordWidget *ui;
};

#endif // RECORDWIDGET_H
