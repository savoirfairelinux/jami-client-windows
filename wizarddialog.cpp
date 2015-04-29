#include "wizarddialog.h"
#include "ui_wizarddialog.h"

#include "accountmodel.h"

#include "utils.h"

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(),this->height());
    ui->buttonBox->setEnabled(false);
}

WizardDialog::~WizardDialog()
{
    delete ui;
}

void WizardDialog::on_buttonBox_accepted()
{
    AccountModel::instance()->add(ui->usernameEdit->text(), Account::Protocol::RING);
    AccountModel::instance()->save();
    Utils::CreateStartupLink();
}

void WizardDialog::on_usernameEdit_textChanged(const QString &arg1)
{
     ui->buttonBox->setEnabled(!arg1.isEmpty());
}
