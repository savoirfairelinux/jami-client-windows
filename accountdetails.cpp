#include "accountdetails.h"
#include "ui_accountdetails.h"

#include "accountmodel.h"
#include "protocolmodel.h"
#include "accountdetails.h"

AccountDetails::AccountDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountDetails),
    codecModel_(nullptr)
{
    ui->setupUi(this);
}

AccountDetails::~AccountDetails()
{
    delete ui;
}

void AccountDetails::setAccount(Account* currentAccount) {
    //should not save if no change has been made
    if (codecModel_)
        codecModel_->save();
    codecModel_ = currentAccount->codecModel();
    ui->audioCodecView->setModel((QAbstractItemModel*)codecModel_->audioCodecs());
    ui->videoCodecView->setModel((QAbstractItemModel*)codecModel_->videoCodecs());
}
