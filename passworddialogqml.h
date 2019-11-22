#ifndef PASSWORDDIALOGQML_H
#define PASSWORDDIALOGQML_H

#include <QObject>
#include <QVariant>


class PasswordDialogQml : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int successCode, MEMBER SuccessCode_, NOTIFY successCodeChanged)
    Q_PROPERTY(QVariant purpose, READ getPurpose WRITE setPurpose NOTIFY purposeChanged)

public:
    enum class PasswordEnteringPurpose{
    ChangePassword,
    ExportAccount,
    Other
    };

    PasswordDialogQml(QObject* parent = nullptr, PasswordEnteringPurpose purpose = PasswordEnteringPurpose::ChangePassword);
    ~PasswordDialogQml();

    static const int SuccessCode_ = 200;

    void setExportPath(const std::string& path) { path_ = path; }

    Q_INVOKABLE void cancelBtnClick();
    Q_INVOKABLE QVariant savePassword(QVariant currentPasswordEditPassword, QVariant passwordEditPassword);
    Q_INVOKABLE QVariant exportAccount(QVariant currentPasswordEditPassword);

    QVariant getPurpose();
    void setPurpose(QVariant);


signals:
    void successCodeChanged(int successCode);
    void purposeChanged(PasswordEnteringPurpose purpose);

private:

    PasswordEnteringPurpose purpose_ { PasswordEnteringPurpose::ChangePassword };
    std::string path_;
};

#endif // PASSWORDDIALOGQML_H
