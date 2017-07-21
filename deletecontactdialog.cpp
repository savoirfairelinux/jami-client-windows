#include "deletecontactdialog.h"
#include "ui_deletecontactdialog.h"

// LRC
#include "recentmodel.h"

DeleteContactDialog::DeleteContactDialog(ContactMethod* cm, Account* ac, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteContactDialog),
    contactMethod_(cm)
{
    ui->setupUi(this);
    ui->contactNameLabel->setText(cm->bestName());
    ui->contactIdLabel->setText(cm->bestId());
}

DeleteContactDialog::~DeleteContactDialog()
{
    delete ui;
}

void DeleteContactDialog::on_calcelButton_clicked()
{
    close();
}

void DeleteContactDialog::on_deleteButton_clicked()
{

    accept();
}

void DeleteContactDialog::on_deleteBanButton_clicked()
{
    accept();
}
