#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include <QWidget>

#include "accountmodel.h"
#include "audio/codecmodel.h"

namespace Ui {
class AccountDetails;
}

class AccountDetails : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetails(QWidget *parent = 0);
    ~AccountDetails();

    void setAccount(Account *currentAccount);
private:
    Ui::AccountDetails *ui;
    CodecModel* codecModel_;

};

#endif // ACCOUNTDETAILS_H
