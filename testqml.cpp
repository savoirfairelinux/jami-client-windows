#include "testqml.h"
#include "ui_testqml.h"

#include <QtQml/QQmlEngine>
TestQML::TestQML(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestQML)
{
    ui->setupUi(this);
}

TestQML::~TestQML()
{
    delete ui;
}
