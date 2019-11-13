#ifndef TESTQML_H
#define TESTQML_H

#include <QWidget>

namespace Ui {
class TestQML;
}

class TestQML : public QWidget
{
    Q_OBJECT

public:
    explicit TestQML(QWidget *parent = nullptr);
    ~TestQML();

private:
    Ui::TestQML *ui;
};

#endif // TESTQML_H
