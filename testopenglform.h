#ifndef TESTOPENGLFORM_H
#define TESTOPENGLFORM_H

#include <QWidget>

namespace Ui {
class TestOpenGLForm;
}

class TestOpenGLForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestOpenGLForm(QWidget *parent = nullptr);
    ~TestOpenGLForm();

private:
    Ui::TestOpenGLForm *ui;
};

#endif // TESTOPENGLFORM_H
