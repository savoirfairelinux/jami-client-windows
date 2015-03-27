#include "navbar.h"
#include "ui_navbar.h"

NavBar::NavBar(QWidget *parent) :
    NavWidget(END, parent),
    ui(new Ui::NavBar)
{
    ui->setupUi(this);
}

NavBar::~NavBar()
{
    delete ui;
}

void NavBar::on_backButton_clicked()
{
    emit BackRequested();
}
