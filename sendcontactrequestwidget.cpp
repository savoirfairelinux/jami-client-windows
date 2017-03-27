#include <QDebug>

#include "sendcontactrequestwidget.h"
#include "ui_sendcontactrequestwidget.h"
#include "account.h"
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
    sendCRClickedConnection_ = connect(ui->sendContactRequestButton, &QPushButton::clicked, [number]() {
        qDebug() << "number : " << number;
        auto cm = PhoneDirectoryModel::instance().getNumber(number);
        qDebug() << "cm ptr : " << cm;
        qDebug() << "cm->account() ptr : " << cm->account();
        if(cm->account() != nullptr){
            cm->account()->requestTrust(cm);
        } else {
            qDebug() << "no account linked to contact method";
        }
    });
}
