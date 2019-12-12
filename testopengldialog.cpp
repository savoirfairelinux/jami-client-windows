#include "testopengldialog.h"
#include "ui_testopengldialog.h"

TestOpenGLDialog::TestOpenGLDialog(GLSharedContextObject* contextObj,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestOpenGLDialog)
{
    ui->setupUi(this);
    ui->glWidget->setContextObj(contextObj);
}

TestOpenGLDialog::~TestOpenGLDialog()
{
    delete ui;
}
