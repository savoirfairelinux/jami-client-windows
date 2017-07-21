#include "deletecontactdialog.h"
#include "ui_deletecontactdialog.h"

// LRC
#include "person.h"
#include "account.h"
#include "bannedcontactmodel.h"

DeleteContactDialog::DeleteContactDialog(ContactMethod* cm, Account* ac, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteContactDialog),
    contactMethod_(cm),
    account_(ac)
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
    if (contactMethod_->contact()){
        contactMethod_->contact()->remove();
    }
    accept();
}

void DeleteContactDialog::on_deleteBanButton_clicked()
{
    if (contactMethod_->contact()){
        contactMethod_->contact()->remove();
        if (account_){
            account_->bannedContactModel()->add(contactMethod_);
        }
    }
    accept();
}
