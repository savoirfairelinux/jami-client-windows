#include "configurationwidget.h"
#include "ui_configurationwidget.h"

#include "video/devicemodel.h"
#include "video/channel.h"
#include "video/resolution.h"
#include "video/rate.h"

ConfigurationWidget::ConfigurationWidget(QWidget *parent) :
    NavWidget(Nav, parent),
    ui(new Ui::ConfigurationWidget)
{
    ui->setupUi(this);

    accountModel_ = AccountModel::instance();
    ui->accountView->setModel(accountModel_);

    deviceModel_ = Video::DeviceModel::instance();
    isLoading = true;
    ui->deviceBox->setModel(deviceModel_);
    ui->deviceBox->setCurrentIndex(deviceModel_->activeIndex());

    codecModel_ = accountModel_->ip2ip()->codecModel();
    ui->codecView->setModel(codecModel_);

}

ConfigurationWidget::~ConfigurationWidget()
{
    accountModel_->save();
    codecModel_->save();
    delete ui;
}

void ConfigurationWidget::on_deviceBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (!isLoading)
        deviceModel_->setActive(index);

    Video::Device *device = deviceModel_->activeDevice();

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
    Video::Device *device = deviceModel_->activeDevice();

    ui->rateBox->clear();

    if (index < 0)
        return;
    if (!isLoading)
        device->channelList()[0]->setActiveResolution(device->channelList()[0]->validResolutions()[index]);

    isLoading = true;
    for (Video::Rate *rate : device->channelList()[0]->validResolutions()[index]->validRates()) {
        ui->rateBox->addItem(rate->name());
    }
    isLoading = false;
}

void ConfigurationWidget::on_rateBox_currentIndexChanged(int index)
{
    if (index < 0 || isLoading)
        return;
    Video::Device *device = deviceModel_->activeDevice();
    device->channelList()[0]->activeResolution()->setActiveRate(index);
}
