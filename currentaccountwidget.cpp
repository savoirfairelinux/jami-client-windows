#include "currentaccountwidget.h"
#include "ui_currentaccountwidget.h"
#include "globalinstances.h"
#include "accountmodel.h"
#include "utils.h"
#include "pixbufmanipulator.h"


CurrentAccountWidget::CurrentAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentAccountWidget)
{
    ui->setupUi(this);
    setup();
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::setup()
{
    setupAccountSelector();
    setupPhoto();
}

void
CurrentAccountWidget::setupAccountSelector()
{
    ui->currentAccountSelector->setModel(&AccountModel::instance());
}

void
CurrentAccountWidget::setupPhoto()
{
    auto selector = ui->currentAccountSelector;
    ui->currentAccountPixmap->setPixmap(Interfaces::PixbufManipulator::decorationRole(selector->itemData(selector->currentIndex()).data()));
}

void CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    AccountModel::instance().setSelectedAccount(ui->currentAccountSelector->itemData(index).data());
}
