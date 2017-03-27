#include "currentaccountwidget.h"
#include "ui_currentaccountwidget.h"
#include "globalinstances.h"
#include "accountmodel.h"
#include "account.h"
#include "qstandarditemmodel.h"
#include "profilemodel.h"
#include "profile.h"
#include "person.h"
#include "utils.h"


CurrentAccountWidget::CurrentAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentAccountWidget)
{
    ui->setupUi(this);
    setup();
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::setup()
{
    setupAccountSelector();

}

void
CurrentAccountWidget::setupAccountSelector()
{
    ui->currentAccountSelector->setModel(&AccountModel::instance());
    updateAccounts();
    if (ui->currentAccountSelector->count() > 0)
        ui->currentAccountSelector->setCurrentIndex(0);
}

void
CurrentAccountWidget::updateAccounts()
{
    auto selector = ui->currentAccountSelector;
    AccountModel* model = &AccountModel::instance();
    QModelIndex qIndex;
    for (int idx = model->rowCount() - 1; idx >= 0; idx--){
        qIndex = selector->model()->index(idx, 0);
        if (! model->getAccountByModelIndex(qIndex)->isEnabled()) {
            qDebug() << model->getAccountByModelIndex(qIndex)->alias() << " Account is not ready";
            //qobject_cast<QStandardItemModel*>(selector->model())->item(idx)->setEnabled(false);
        }
    }
}



void
CurrentAccountWidget::setPhoto()
{
    auto selector = ui->currentAccountSelector;
    if (selector->count() > 0) {
        qDebug() << "currentAccountWidget height = " << height();
        auto ac = static_cast<Account*>(selector->itemData(selector->currentIndex()).data());
        if(&ProfileModel::instance()){
            if (ProfileModel::instance().selectedProfile()) {
                if (ProfileModel::instance().selectedProfile()->person()){
                    QImage img = Utils::getCirclePhoto(ProfileModel::instance().selectedProfile()->person()->photo().value<QImage>(),
                                                       ui->idDisplayLayout->contentsRect().height());
                    ui->currentAccountPixmap->setPixmap(QPixmap::fromImage(img));
                }
            }
        }
    }
}

void
CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    QModelIndex idx = ui->currentAccountSelector->model()->index(index,0);
    Account* ac = static_cast<AccountModel*>(ui->currentAccountSelector->model())->getAccountByModelIndex(idx);

    if (ac) {
        qDebug() << "current index changed triggered: account " << ac;
        AccountModel::instance().setSelectedAccount(ac);
        setPhoto();
    } else {
        qDebug() << "woooops no account @ idx " << index << ac;
    }
}

void CurrentAccountWidget::on_accountDetailChanged(){
    updateAccounts();
}
