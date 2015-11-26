#ifndef TESTMESSAGEDIALOG_H
#define TESTMESSAGEDIALOG_H

#include <QDialog>
#include "contactmethod.h"
#include "phonedirectorymodel.h"

namespace Ui {
class TestMessageDialog;
}

class TestMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestMessageDialog(QWidget *parent = 0);
    ~TestMessageDialog();

private slots:
    void on_pushButton_clicked();
public slots:
    void incoming_message_receveid(ContactMethod* cm, QString msg);

private:
    Ui::TestMessageDialog *ui;
};

#endif // TESTMESSAGEDIALOG_H
