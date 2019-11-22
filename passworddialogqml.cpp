#include "passworddialogqml.h"

PasswordDialogQml::PasswordDialogQml(QObject* parent, PasswordEnteringPurpose purpose) : QObject(parent)
{
    purpose_ = purpose;
}

void
PasswordDialogQml::successCodeChanged(int successCode)
{
}
