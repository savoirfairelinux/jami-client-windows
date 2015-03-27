#include "mainbar.h"
#include "ui_mainbar.h"

#include "callmodel.h"

MainBar::MainBar(QWidget *parent) :
    NavWidget(END, parent),
    ui(new Ui::MainBar)
{
    ui->setupUi(this);
}

MainBar::~MainBar()
{
    delete ui;
}

void MainBar::on_confButton_clicked()
{
    emit NavigationRequested(ScreenEnum::ConfScreen);
}

void MainBar::on_callLineEdit_returnPressed()
{
    callAction();
}

void MainBar::on_callButton_clicked()
{
    callAction();
}

void MainBar::callAction() {
    if (ui->callLineEdit->text().isEmpty())
        return;
    Call* outCall = CallModel::instance()->dialingCall(ui->callLineEdit->text());
    outCall->setDialNumber(ui->callLineEdit->text());
    outCall->performAction(Call::Action::ACCEPT);
}
