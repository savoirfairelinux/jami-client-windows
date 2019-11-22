#ifndef PASSWORDDIALOGQML_H
#define PASSWORDDIALOGQML_H

#include <QObject>
#include <QVariant>
#include <QtQuick>
#include <QQuickView>


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

    Q_INVOKABLE QVariant savePassword(QVariant currentPasswordEditPassword, QVariant passwordEditPassword);
    Q_INVOKABLE QVariant exportAccount(QVariant currentPasswordEditPassword);
    Q_INVOKABLE void closeAndSuccess(QVariant isSuccess);


    QVariant getPurpose();
    void setPurpose(QVariant);


signals:
    void successCodeChanged(int successCode);
    void purposeChanged(QVariant purpose);

private:
    bool savePassword(QString currentPasswordEditPassword, QString passwordEditPassword);
    bool exportAccount(QString currentPasswordEditPassword);

private:
    PasswordEnteringPurpose purpose_ { PasswordEnteringPurpose::ChangePassword };
    std::string path_;
};

class PasswordDialogQuickView : public QQuickView
{
    Q_OBJECT

public:
    PasswordDialogQuickView(const QUrl &source, QWindow *parent = nullptr);
    virtual ~PasswordDialogQuickView();

public slots:
    int whendone(int donecode);
};

#endif // PASSWORDDIALOGQML_H
