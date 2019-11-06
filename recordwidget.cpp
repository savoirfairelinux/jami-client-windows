#include "recordwidget.h"
#include "ui_recordwidget.h"

RecordWidget::RecordWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordWidget)
{
    ui->setupUi(this);
}

RecordWidget::~RecordWidget()
{
    delete ui;
}
