#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H

#include <QDialog>

namespace Ui {
class WizardDialog;
}

class WizardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WizardDialog(QWidget *parent = 0);
    ~WizardDialog();

private slots:
    void on_buttonBox_accepted();

    void on_usernameEdit_textChanged(const QString &arg1);

private:
    Ui::WizardDialog *ui;
};

#endif // WIZARDDIALOG_H
