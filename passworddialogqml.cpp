#include "passworddialogqml.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include "lrcinstance.h"

PasswordDialogQml::PasswordDialogQml(QObject* parent, PasswordEnteringPurpose purpose) : QObject(parent)
{
    purpose_ = purpose;
}

PasswordDialogQml::~PasswordDialogQml()
{
}

Q_INVOKABLE QVariant
PasswordDialogQml::savePassword(QVariant currentPasswordEditPassword, QVariant passwordEditPassword)
{
    bool success = false;
    try {
        typedef bool (PasswordDialogQml::* Op)(QString,QString);
        Op op = &PasswordDialogQml::savePassword;
        QFuture<bool> futureSavePassword = QtConcurrent::run(this, op, currentPasswordEditPassword.toString(), passwordEditPassword.toString());
        futureSavePassword.waitForFinished();
        success = futureSavePassword.result();
    } catch (...) {
        qDebug() << "The save password process fails!";
    }
    return QVariant(success);
}

Q_INVOKABLE QVariant
PasswordDialogQml::exportAccount(QVariant currentPasswordEditPassword)
{
    bool success = false;
    try {
        typedef bool (PasswordDialogQml::* Op)(QString);
        Op op = &PasswordDialogQml::exportAccount;
        QFuture<bool> futureSavePassword = QtConcurrent::run(this, op, currentPasswordEditPassword.toString());
        futureSavePassword.waitForFinished();
        success = futureSavePassword.result();
    } catch (...) {
        qDebug() << "The export account process fails!";
    }
    return QVariant(success);
}

Q_INVOKABLE void
PasswordDialogQml::closeAndSuccess(QVariant isSuccess)
{

    if(purpose_ == PasswordEnteringPurpose::ChangePassword) {
        if(isSuccess.toBool()) {
            QMessageBox::information(0, tr("Success"), tr("Password Changed Successfully"));
        } else {
            QMessageBox::critical(0, tr("Error"), tr("Password Changed Failed"));
        }
    } else if(purpose_ == PasswordEnteringPurpose::ExportAccount) {
        if(isSuccess.toBool()) {
            QMessageBox::information(0, tr("Success"), tr("Export Successful"));
        } else {
            QMessageBox::critical(0, tr("Error"), tr("Export Failed"));
        }
    }
}

QVariant
PasswordDialogQml::getPurpose()
{
    QString purposeToSend = "";
    if(purpose_ == PasswordEnteringPurpose::ChangePassword) {
        purposeToSend = "ChangePassword";
    } else if(purpose_ == PasswordEnteringPurpose::ExportAccount) {
        purposeToSend = "ExportAccount";
    }
    return QVariant(purposeToSend);
}

void
PasswordDialogQml::setPurpose(QVariant purpose)
{
    if(purpose.toString() == "ChangePassword") {
        purpose_ = PasswordEnteringPurpose::ChangePassword;
    } else if(purpose.toString() == "ExportAccount") {
        purpose_ = PasswordEnteringPurpose::ExportAccount;
    } else {
        purpose_ = PasswordEnteringPurpose::Other;
    }
}

bool
PasswordDialogQml::savePassword(QString currentPasswordEditPassword, QString passwordEditPassword)
{
    bool success = LRCInstance::accountModel().changeAccountPassword(LRCInstance::getCurrAccId(),
                   currentPasswordEditPassword.toStdString(), passwordEditPassword.toStdString());

    /*ui->spinnerLabel->hide();*/
    /*spinnerMovie_->stop();*/
    if (success) {

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.archiveHasPassword = !passwordEditPassword.isEmpty();
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

       // done(SuccessCode);
    } else {
        /*Utils::whileBlocking<QLineEdit>(ui->currentPasswordEdit)->clear();
        ui->btnChangePasswordConfirm->setEnabled(false);
        ui->wrongPasswordLabel->show();*/
    }

    return success;
}

bool
PasswordDialogQml::exportAccount(QString currentPasswordEditPassword)
{
    bool success = LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(),
                                                              path_,
                                                              currentPasswordEditPassword.toStdString());
    /*ui->spinnerLabel->hide();*/
    /*spinnerMovie_->stop();*/
    if(success) {
        //done(SuccessCode);
    } else {
        /*Utils::whileBlocking<QLineEdit>(ui->currentPasswordEdit)->clear();
        ui->btnChangePasswordConfirm->setEnabled(false);
        ui->wrongPasswordLabel->show();*/
    }

    return success;
}

PasswordDialogQuickView::PasswordDialogQuickView(const QUrl& source, QWindow* parent):
                            QQuickView(source,parent)
{

}

int
PasswordDialogQuickView::PasswordDialogQuickView::whendone(int donecode)
{
}

PasswordDialogQuickView::~PasswordDialogQuickView()
{
}
