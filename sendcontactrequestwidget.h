#ifndef SENDCONTACTREQUESTWIDGET_H
#define SENDCONTACTREQUESTWIDGET_H

#include <QWidget>

namespace Ui {
class SendContactRequestWidget;
}

class SendContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendContactRequestWidget(QWidget *parent = 0);
    ~SendContactRequestWidget();
    void setup(const QModelIndex& nodeIdx);

private:
    QMetaObject::Connection sendCRClickedConnection_;
    Ui::SendContactRequestWidget *ui;

signals:
    void sendCRclicked();
};

#endif // SENDCONTACTREQUESTWIDGET_H
