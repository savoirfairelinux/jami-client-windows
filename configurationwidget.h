#ifndef CONFIGURATIONWIDGET_H
#define CONFIGURATIONWIDGET_H

#include <QWidget>
#include <QItemSelection>

#include "navwidget.h"

#include "accountmodel.h"
#include "video/devicemodel.h"
#include "audio/codecmodel.h"

#include "accountdetails.h"

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

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void accountSelected(QItemSelection itemSel);

private:
    Ui::ConfigurationWidget *ui;
    AccountModel* accountModel_;
    Video::DeviceModel* deviceModel_;
    CodecModel* codecModel_;
    bool isLoading;
    AccountDetails* accountDetails_;
};

#endif // CONFIGURATIONWIDGET_H
