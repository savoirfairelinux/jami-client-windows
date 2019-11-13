#ifndef TESTQML_H
#define TESTQML_H

#include <QtWidgets/QDialog>

namespace Ui {
class TestQML;
}

class TestQML : public QDialog
{
    Q_OBJECT

public:
    explicit TestQML(QWidget *parent = nullptr);
    ~TestQML();

private:
    Ui::TestQML *ui;
};

#endif // TESTQML_H
