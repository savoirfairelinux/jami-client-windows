#include "phonedirectorydialog.h"
#include "ui_phonedirectorydialog.h"

#include "phonedirectorymodel.h"

PhoneDirectoryDialog::PhoneDirectoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhoneDirectoryDialog)
{
    ui->setupUi(this);
    ui->phoneDirectoryView->setModel(PhoneDirectoryModel::instance());
}

PhoneDirectoryDialog::~PhoneDirectoryDialog()
{
    delete ui;
}
