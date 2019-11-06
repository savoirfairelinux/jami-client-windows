#ifndef FADEOUTABLEREDDOT_H
#define FADEOUTABLEREDDOT_H

#include <QWidget>

namespace Ui {
class FadeOutableRedDot;
}

class FadeOutableRedDot : public QWidget
{
    Q_OBJECT

public:
    explicit FadeOutableRedDot(QWidget *parent = nullptr);
    ~FadeOutableRedDot();

private:
    Ui::FadeOutableRedDot *ui;
};

#endif // FADEOUTABLEREDDOT_H
