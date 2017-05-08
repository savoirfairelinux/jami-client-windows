#ifndef BANNEDCONTACTSWIDGET_H
#define BANNEDCONTACTSWIDGET_H

#include <QWidget>

namespace Ui {
class BannedContactsWidget;
}

class BannedContactsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BannedContactsWidget(QWidget *parent = 0);
    ~BannedContactsWidget();

private:
    Ui::BannedContactsWidget *ui;
};

#endif // BANNEDCONTACTSWIDGET_H
