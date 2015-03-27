#include "mainbar.h"
#include "ui_mainbar.h"

#include "callmodel.h"

MainBar::MainBar(QWidget *parent) :
    NavWidget(END, parent),
    ui(new Ui::MainBar)
{
    ui->setupUi(this);

    menu_ = new QMenu();

    auto aboutAction = new QAction("About", this);
    menu_->addAction(aboutAction);

    auto exitAction = new QAction("Exit", this);
    menu_->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(on_exitButton_clicked()));

    ui->logoToolButton->setMenu(menu_);
    ui->logoToolButton->setPopupMode(QToolButton::InstantPopup);
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
    auto outCall = CallModel::instance()->dialingCall(ui->callLineEdit->text());
    outCall->setDialNumber(ui->callLineEdit->text());
    outCall->performAction(Call::Action::ACCEPT);
}

void MainBar::on_exitButton_clicked()
{
    QCoreApplication::exit();
}

void MainBar::on_toolButton_2_clicked()
{
    emit minimize();
}

void MainBar::atExit() {

}
