#ifndef PASSWORDDIALOGQML_H
#define PASSWORDDIALOGQML_H

#include <QObject>
#include <QVariant>


class PasswordDialogQml : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int successCode, MEMBER SuccessCode_, NOTIFY successCodeChanged)


public:
    enum class PasswordEnteringPurpose{
    ChangePassword,
    ExportAccount
};

    Q_ENUMS(PasswordEnteringPurpose)

    PasswordDialogQml(QObject* parent = nullptr, PasswordEnteringPurpose purpose = PasswordEnteringPurpose::ChangePassword);
    ~PasswordDialogQml();

    static const int SuccessCode_ = 200;

    void setExportPath(const std::string& path) { path_ = path; }

    Q_INVOKABLE void cancelBtnClick();
    Q_INVOKABLE QVariant savePassword(QVariant currentPasswordEditPassword, QVariant passwordEditPassword);
    Q_INVOKABLE QVariant exportAccount(QVariant currentPasswordEditPassword);

signals:
    void successCodeChanged(int successCode);

private:

    PasswordEnteringPurpose purpose_ { PasswordEnteringPurpose::ChangePassword };
    std::string path_;
};

#endif // PASSWORDDIALOGQML_H
