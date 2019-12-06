#ifndef TESTOPENGLDIALOG_H
#define TESTOPENGLDIALOG_H

#include <QDialog>

namespace Ui {
class TestOpenGLDialog;
}

class TestOpenGLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestOpenGLDialog(QWidget *parent = nullptr);
    ~TestOpenGLDialog();

private:
    Ui::TestOpenGLDialog *ui;
};

#endif // TESTOPENGLDIALOG_H
