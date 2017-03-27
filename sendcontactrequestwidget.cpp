#include <QDebug>

#include "sendcontactrequestwidget.h"
#include "ui_sendcontactrequestwidget.h"
#include "account.h"
#include "accountmodel.h"
#include "recentmodel.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"

SendContactRequestWidget::SendContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendContactRequestWidget)
{
    ui->setupUi(this);
    ui->contactRequestInfo->setText("Contact Request Bruh!!!!\n");
    ui->peerContactID->setText("ContactID\n");
}

SendContactRequestWidget::~SendContactRequestWidget()
{
    disconnect(sendCRClickedConnection_);
    delete ui;
}

void
SendContactRequestWidget::setup(const QModelIndex& nodeIdx)
{
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    disconnect(sendCRClickedConnection_);
    QString number = cmVector[0]->uri();
    ui->peerContactID->setText(number);
    sendCRClickedConnection_ = connect(ui->sendContactRequestButton, &QPushButton::clicked, [this,nodeIdx]() {
        sendCR(nodeIdx);
    });
}

void SendContactRequestWidget::sendCR(const QModelIndex& nodeIdx)
{
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    QString number = cmVector[0]->uri();
    auto cm = PhoneDirectoryModel::instance().getNumber(number);

    if(cm->account() != nullptr){
        cm->account()->sendContactRequest(cm);
    } else {
        qDebug() << "no account linked to contact method";
        cm->setAccount(AccountModel::instance().userChosenAccount());
        cm->account()->sendContactRequest(cm);
    }
    //emit sendCRclicked();
}
