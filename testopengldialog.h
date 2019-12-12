#ifndef TESTOPENGLDIALOG_H
#define TESTOPENGLDIALOG_H

#include <QDialog>

class GLSharedContextObject;

namespace Ui {
class TestOpenGLDialog;
}

class TestOpenGLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestOpenGLDialog(GLSharedContextObject* contextObj,QWidget *parent = nullptr);
    ~TestOpenGLDialog();

private:
    Ui::TestOpenGLDialog *ui;
};

#endif // TESTOPENGLDIALOG_H
