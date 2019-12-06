#include "testopenglform.h"
#include "ui_testopenglform.h"

TestOpenGLForm::TestOpenGLForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestOpenGLForm)
{
    ui->setupUi(this);
}

TestOpenGLForm::~TestOpenGLForm()
{
    delete ui;
}
