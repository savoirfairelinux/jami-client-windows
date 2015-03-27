#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->setFixedSize(this->width(),this->height());
    ui->creditsWidget->hide();
    ui->clientVersionLabel->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">Windows Ring v") + VERSION + "</span></p></body></html>");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_aboutButton_toggled(bool checked)
{
    ui->creditsButton->setChecked(!checked);
    ui->aboutWidget->setVisible(checked);
    ui->creditsWidget->setVisible(!checked);
}

void AboutDialog::on_creditsButton_clicked(bool checked)
{
    ui->aboutButton->setChecked(!checked);
    ui->creditsWidget->setVisible(checked);
    ui->aboutWidget->setVisible(!checked);
}

void AboutDialog::on_toolButton_clicked()
{
    this->close();
}
