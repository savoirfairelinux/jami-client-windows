#include "quickactcontactrequestwidget.h"
#include "ui_quickactcontactrequestwidget.h"

#include <QFont>

// CLIENT
#include "contactrequestitemdelegate.h"

QuickActContactRequestWidget::QuickActContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuickActContactRequestWidget)
{
    ui->setupUi(this);

    // set symbols in buttons using FontAwsome
    ui->quickValidCRBtn->setText(QChar(0xf00c));
    ui->quickMuteCRBtn->setText(QChar(0xf12d));
    ui->quickBanCRBtn->setText(QChar(0xf00d));

    connect(ui->quickValidCRBtn, &QPushButton::clicked, this, [=](){
        emit quickValidCRBtnClicked();
    });

    connect(ui->quickMuteCRBtn, &QPushButton::clicked, this, [=](){
        emit quickValidCRBtnClicked();
    });

    connect(ui->quickBanCRBtn, &QPushButton::clicked, this, [=](){
        emit quickValidCRBtnClicked();
    });

}

QuickActContactRequestWidget::~QuickActContactRequestWidget()
{
    disconnect(this);
    delete ui;
}
