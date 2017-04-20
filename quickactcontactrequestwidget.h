#ifndef QUICKACTCONTACTREQUESTWIDGET_H
#define QUICKACTCONTACTREQUESTWIDGET_H

#include <QWidget>


namespace Ui {
class QuickActContactRequestWidget;
}

class QuickActContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickActContactRequestWidget(QWidget *parent = 0);
    ~QuickActContactRequestWidget();

signals:
    void quickValidCRBtnClicked();
    void quickMuteCRBtnClicked();
    void quickBanCRBtnClicked();

private:
    Ui::QuickActContactRequestWidget *ui;
};

#endif // QUICKACTCONTACTREQUESTWIDGET_H
