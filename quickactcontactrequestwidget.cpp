#include "quickactcontactrequestwidget.h"
#include "ui_quickactcontactrequestwidget.h"

// CLIENT
#include "contactrequestitemdelegate.h"

QuickActContactRequestWidget::QuickActContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuickActContactRequestWidget)
{
    ui->setupUi(this);

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
    delete ui;
}
