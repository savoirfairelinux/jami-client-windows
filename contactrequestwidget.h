#ifndef CONTACTREQUESTWIDGET_H
#define CONTACTREQUESTWIDGET_H

#include <QWidget>
#include "contactrequest.h"

namespace Ui {
class ContactRequestWidget;
}

class ContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactRequestWidget(QWidget *parent = 0);
    ~ContactRequestWidget();
    void setCurrentContactRequest(ContactRequest* cr);

signals:
    void choiceMade();

private slots:
    void on_acceptCRButton_clicked();
    void on_discardCRButton_clicked();

private:
    Ui::ContactRequestWidget *ui;
    ContactRequest* cr_ = nullptr;
};

#endif // CONTACTREQUESTWIDGET_H
