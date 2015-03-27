#ifndef PIVOTVIEWWIDGET_H
#define PIVOTVIEWWIDGET_H

#include <QWidget>

#include "navwidget.h"
#include "pivotcontrol.h"

namespace Ui {
class PivotViewWidget;
}

class PivotViewWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit PivotViewWidget(QWidget *parent = 0);
    ~PivotViewWidget();
    void atExit();

private:
    Ui::PivotViewWidget *ui;
};

#endif // PIVOTVIEWWIDGET_H
