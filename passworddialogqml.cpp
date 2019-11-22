#include "passworddialogqml.h"

PasswordDialogQml::PasswordDialogQml(QObject* parent, PasswordEnteringPurpose purpose) : QObject(parent)
{
    purpose_ = purpose;
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
