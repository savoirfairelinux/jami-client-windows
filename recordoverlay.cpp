#include "recordoverlay.h"
#include "ui_recordoverlay.h"

RecordOverlay::RecordOverlay(QWidget *parent, QWidget *containerWidget) :
    QWidget(parent),
    ui(new Ui::RecordOverlay)
{
    ui->setupUi(this);

}


RecordOverlay::~RecordOverlay()
{
    delete ui;
}
