#include "configurationwidget.h"
#include "ui_configurationwidget.h"

#include "video/devicemodel.h"
#include "video/channel.h"
#include "video/resolution.h"
#include "video/rate.h"

#include "accountmodel.h"
#include "protocolmodel.h"
#include "accountdetails.h"

ConfigurationWidget::ConfigurationWidget(QWidget *parent) :
    NavWidget(Nav, parent),
    ui(new Ui::ConfigurationWidget)
{
    ui->setupUi(this);

    accountDetails_ = new AccountDetails();
    accountModel_ = AccountModel::instance();
    ui->accountView->setModel(accountModel_);

    deviceModel_ = Video::DeviceModel::instance();

    //FIXME : Seems to set the active device instead of taking the default one
    isLoading = true;
    ui->deviceBox->setModel(deviceModel_);
    ui->deviceBox->setCurrentIndex(deviceModel_->activeIndex());

    connect(ui->accountView->selectionModel(),
         SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
         this, SLOT(accountSelected(QItemSelection)));
    ui->accountView->setCurrentIndex(accountModel_->index(0));

    ui->accountDetailLayout->addWidget(accountDetails_);

    ui->testVideoButton->setCheckable(true);
}

void ConfigurationWidget::atExit() {
    accountModel_->save();
    accountDetails_->save();
}

ConfigurationWidget::~ConfigurationWidget()
{
    delete ui;
}

void ConfigurationWidget::on_deviceBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (!isLoading)
        deviceModel_->setActive(index);

    auto device = deviceModel_->activeDevice();

    ui->sizeBox->clear();

    isLoading = true;
    if (device->channelList().size() > 0) {
        for (Video::Resolution *resolution : device->channelList()[0]->validResolutions()) {
            ui->sizeBox->addItem(resolution->name());
        }
    }
    ui->sizeBox->setCurrentIndex(device->channelList()[0]->activeResolution()->relativeIndex());
    isLoading = false;
}

void ConfigurationWidget::on_sizeBox_currentIndexChanged(int index)
{
    auto device = deviceModel_->activeDevice();

    ui->rateBox->clear();

    if (index < 0)
        return;
    if (!isLoading)
        device->channelList()[0]->setActiveResolution(device->channelList()[0]->validResolutions()[index]);

    isLoading = true;
    for (Video::Rate *rate : device->channelList()[0]->validResolutions()[index]->validRates()) {
        ui->rateBox->addItem(rate->name());
    }
    ui->rateBox->setCurrentIndex(device->channelList()[0]->activeResolution()->activeRate()->relativeIndex());
    isLoading = false;
}

void ConfigurationWidget::on_rateBox_currentIndexChanged(int index)
{
    if (index < 0 || isLoading)
        return;
    auto device = deviceModel_->activeDevice();
    device->channelList()[0]->activeResolution()->setActiveRate(index);
}

//void ConfigurationWidget::on_pushButton_clicked()
//{
    //ui->addAccountWidget->setVisible(true);
//}

//void ConfigurationWidget::on_pushButton_4_clicked()
//{
//    //ui->aliasLineEdit->clear();
//    //ui->protocolComboBox->setCurrentIndex(0);
//    //ui->addAccountWidget->setVisible(false);
//}

//void ConfigurationWidget::on_pushButton_3_clicked()
//{
//    //if (ui->aliasLineEdit->text().isEmpty())
//     //   return;
//    //accountModel_->add(ui->aliasLineEdit->text(), ui->protocolComboBox->model()->index(ui->protocolComboBox->currentIndex(), 0));
//    //accountModel_->save();
//    //on_pushButton_4_clicked();
//    //accountModel_->registerAllAccounts();
//}

void ConfigurationWidget::accountSelected(QItemSelection itemSel) {
    Q_UNUSED(itemSel)
    accountDetails_->setAccount(accountModel_->getAccountByModelIndex(ui->accountView->currentIndex()));
}

void ConfigurationWidget::on_newButton_clicked()
{

}

void ConfigurationWidget::on_deleteButton_clicked()
{

}

void ConfigurationWidget::on_testVideoButton_toggled(bool checked)
{
    if (checked)
        ui->videoWidget->setRenderer(Video::PreviewManager::instance()->previewRenderer());
    checked ? Video::PreviewManager::instance()->startPreview() : Video::PreviewManager::instance()->stopPreview();
}
