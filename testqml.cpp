#include "testqml.h"
#include "ui_testqml.h"

TestQML::TestQML(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestQML)
{
    ui->setupUi(this);
}

TestQML::~TestQML()
{
    delete ui;
}
