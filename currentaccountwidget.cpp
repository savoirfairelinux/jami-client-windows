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
    qDebug() << "CurrentAccount : setup begin";
    setup();
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::setup()
{
    updateAccounts();
    if (ui->currentAccountSelector->count() > 0) {
        ui->currentAccountSelector->setCurrentIndex(0);
        emit ui->currentAccountSelector->currentIndexChanged(0);
        qDebug() << "CurrentAccount : setup over";
    } else {
        qDebug() << "CurrentAccount : No account available";
    }
}

void
CurrentAccountWidget::update()
{
    updateAccounts();

}

void
CurrentAccountWidget::updateAccounts()
{
    auto selector = ui->currentAccountSelector;
    AccountModel* model = &(AccountModel::instance());
    QModelIndex qIndex;

    selector->clear();

    for (int idx = 0; idx <=  model->rowCount() - 1; idx++){
        qIndex = model->index(idx);
        if (! model->getAccountByModelIndex(qIndex)->isEnabled()) {
            qDebug() << model->getAccountByModelIndex(qIndex)->alias() << " Account is not enabled";
        } else {
            selector->addItem(model->getAccountByModelIndex(qIndex)->alias());
        }
    }
}



void
CurrentAccountWidget::setPhoto()
{
    auto selector = ui->currentAccountSelector;
    if (selector->count() > 0) {
        if(&ProfileModel::instance()){
            if (ProfileModel::instance().selectedProfile()) {
                if (ProfileModel::instance().selectedProfile()->person()){
                    QImage img = Utils::getCirclePhoto(ProfileModel::instance().selectedProfile()->person()->photo().value<QImage>(),
                                                       ui->idDisplayLayout->contentsRect().height());
                    ui->currentAccountPixmap->setPixmap(QPixmap::fromImage(img));
                    qDebug() << "CurrentAccount : Photo set";
                } else {
                    qDebug() << "CurrentAccount : selected profile has no person";
                }
            } else {
                qDebug() << "CurrentAccount : Profilemodel: no selected profile";
            }
        } else {
            qDebug() << "CurrentAccount : Profilemodel not set";
        }
    } else {
        qDebug() << "CurrentAccount : account not set";
    }
}

void
CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    QModelIndex idx = ui->currentAccountSelector->model()->index(index,0);
    Account* ac = AccountModel::instance().getAccountByModelIndex(idx);

    if (ac) {
        AccountModel::instance().setUserChosenAccount(ac);
        setPhoto();
    } else {
        qDebug() << "CurrentAccount : account not referenced correctly";
        //null for now
    }
}

