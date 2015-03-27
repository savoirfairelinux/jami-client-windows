#ifndef CONFIGURATIONWIDGET_H
#define CONFIGURATIONWIDGET_H

#include <QWidget>

#include "navwidget.h"

#include "accountmodel.h"
#include "video/devicemodel.h"
#include "audio/codecmodel.h"

namespace Ui {
class ConfigurationWidget;
}

class ConfigurationWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit ConfigurationWidget(QWidget *parent = 0);
    ~ConfigurationWidget();

private slots:
    void on_deviceBox_currentIndexChanged(int index);

    void on_sizeBox_currentIndexChanged(int index);

    void on_rateBox_currentIndexChanged(int index);

private:
    Ui::ConfigurationWidget *ui;
    AccountModel* accountModel_;
    Video::DeviceModel* deviceModel_;
    CodecModel* codecModel_;
    bool isLoading;
};

#endif // CONFIGURATIONWIDGET_H
