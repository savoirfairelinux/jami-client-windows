#ifndef DELETECONTACTDIALOG_H
#define DELETECONTACTDIALOG_H

#include <QDialog>

// LRC
#include "contactmethod.h"

namespace Ui {
class DeleteContactDialog;
}

class DeleteContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteContactDialog(ContactMethod* cm, Account *ac, QWidget *parent = 0);
    ~DeleteContactDialog();

private slots:
    void on_calcelButton_clicked();
    void on_deleteButton_clicked();
    void on_deleteBanButton_clicked();

private:
    Ui::DeleteContactDialog *ui;
    ContactMethod* contactMethod_;
    Account* account_;
};

#endif // DELETECONTACTDIALOG_H
