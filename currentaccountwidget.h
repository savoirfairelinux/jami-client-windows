#ifndef CURRENTACCOUNTWIDGET_H
#define CURRENTACCOUNTWIDGET_H

#include <QWidget>

namespace Ui {
class CurrentAccountWidget;
}

class CurrentAccountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentAccountWidget(QWidget *parent = 0);
    ~CurrentAccountWidget();

public slots:
    void update();

private slots:
    void on_currentAccountSelector_currentIndexChanged(int index);

private:
    Ui::CurrentAccountWidget *ui;
    void setup();
    void setPhoto();
    void updateAccounts();
};

#endif // CURRENTACCOUNTWIDGET_H
