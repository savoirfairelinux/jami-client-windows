#include "bannedcontactswidget.h"
#include "ui_bannedcontactswidget.h"

BannedContactsWidget::BannedContactsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BannedContactsWidget)
{
    ui->setupUi(this);

    // Configure banned list to display banned contacts
    //ui->bannedList->setModel();

    // Configure banned widget connections
}

BannedContactsWidget::~BannedContactsWidget()
{
    delete ui;
}
