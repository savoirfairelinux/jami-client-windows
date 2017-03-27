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

private slots:
    void on_currentAccountSelector_currentIndexChanged(int index);

private:
    Ui::CurrentAccountWidget *ui;
    void setupPhoto();
    void setup();
    void setupAccountSelector();
};

#endif // CURRENTACCOUNTWIDGET_H
