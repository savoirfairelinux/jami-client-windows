#include "testopengldialog.h"
#include "ui_testopengldialog.h"

TestOpenGLDialog::TestOpenGLDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestOpenGLDialog)
{
    ui->setupUi(this);
}

TestOpenGLDialog::~TestOpenGLDialog()
{
    delete ui;
}
