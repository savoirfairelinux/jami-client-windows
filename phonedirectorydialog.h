#ifndef PHONEDIRECTORYDIALOG_H
#define PHONEDIRECTORYDIALOG_H

#include <QDialog>

namespace Ui {
class PhoneDirectoryDialog;
}

class PhoneDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhoneDirectoryDialog(QWidget *parent = 0);
    ~PhoneDirectoryDialog();

private:
    Ui::PhoneDirectoryDialog *ui;
};

#endif // PHONEDIRECTORYDIALOG_H
