#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"

// LRC
#include "accountmodel.h"
#include "itemdataroles.h"
#include "account.h"

DeleteAccountDialog::DeleteAccountDialog(const QModelIndex & idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteAccountDialog),
    index_(idx)
{
    ui->setupUi(this);
    ui->accountAliasLabel->setText(AccountModel::instance().data(index_, Qt::DisplayRole).toString());
    auto ac = AccountModel::instance().getAccountByModelIndex(index_);
    if (ac->protocol() == Account::Protocol::RING){
        ui->accountIdLabel->setAlignment(Qt::AlignCenter);
        ui->accountIdLabel->setText((ac->registeredName().isEmpty())? ac->username(): ac->registeredName() + "\n" + ac->username());
    } else {
        ui->warningLabel->hide();
    }
}

DeleteAccountDialog::~DeleteAccountDialog()
{
    delete ui;
}

void DeleteAccountDialog::on_deleteCancelBtn_clicked()
{
    close();
}

void DeleteAccountDialog::on_deleteAcceptBtn_clicked()
{
    auto account = AccountModel::instance().getAccountByModelIndex(index_);
    AccountModel::instance().remove(account);
    AccountModel::instance().save();
    close();
}
