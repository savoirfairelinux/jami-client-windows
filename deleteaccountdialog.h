#ifndef DELETEACCOUNTDIALOG_H
#define DELETEACCOUNTDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class DeleteAccountDialog;
}

class DeleteAccountDialog : public QDialog
{
    Q_OBJECT

public:
    DeleteAccountDialog(const QModelIndex & idx, QWidget *parent = 0);
    ~DeleteAccountDialog();

signals:
    deleteAcceptClicked();

private slots:
    void on_deleteCancelBtn_clicked();

    void on_deleteAcceptBtn_clicked();

private:
    Ui::DeleteAccountDialog *ui;
    QModelIndex index_;
};

#endif // DELETEACCOUNTDIALOG_H
