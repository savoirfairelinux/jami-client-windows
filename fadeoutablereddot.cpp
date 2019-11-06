#include "fadeoutablereddot.h"
#include "ui_fadeoutablereddot.h"

FadeOutableRedDot::FadeOutableRedDot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FadeOutableRedDot)
{
    ui->setupUi(this);
}

FadeOutableRedDot::~FadeOutableRedDot()
{
    delete ui;
}
