#include "pivotviewwidget.h"
#include "ui_pivotviewwidget.h"
#include <QDebug>



PivotViewWidget::PivotViewWidget(QWidget *parent) :
    NavWidget(CallScreen/*PivotScreen*/ , parent),
    ui(new Ui::PivotViewWidget)
{
    ui->setupUi(this);
}

PivotViewWidget::~PivotViewWidget()
{
    delete ui;
}

void PivotViewWidget::atExit()
{

}
