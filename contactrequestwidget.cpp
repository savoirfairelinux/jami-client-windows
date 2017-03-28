#include "contactrequestwidget.h"
#include "ui_contactrequestwidget.h"

ContactRequestWidget::ContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactRequestWidget)
{
    ui->setupUi(this);
}

ContactRequestWidget::~ContactRequestWidget()
{
    delete ui;
}

void
ContactRequestWidget::setCurrentContactRequest(ContactRequest *cr)
{
    cr_ = cr;
    if (cr_ != nullptr) {
        ui->remoteIdLabel->setText(QString("Current ContactRequest: %1").arg(QString::fromLocal8Bit(cr_->roleData(Qt::DisplayRole).value<QByteArray>())));
    }
}

void
ContactRequestWidget::on_acceptCRButton_clicked()
{
    if (cr_ != nullptr) {
        cr_->accept();
    }
    setCurrentContactRequest(nullptr);
    emit choiceMade();
}

void ContactRequestWidget::on_discardCRButton_clicked()
{
    if (cr_ != nullptr) {
        cr_->discard();
    }
    setCurrentContactRequest(nullptr);
    emit choiceMade();
}
