#include "testmessagedialog.h"
#include "ui_testmessagedialog.h"

TestMessageDialog::TestMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestMessageDialog)
{
    ui->setupUi(this);
    ui->comboBox->setModel(&PhoneDirectoryModel::instance());
    connect(&PhoneDirectoryModel::instance(), SIGNAL(incomingMessage(ContactMethod*,QString)),
            this, SLOT(incoming_message_receveid(ContactMethod*,QString)));
}

TestMessageDialog::~TestMessageDialog()
{
    delete ui;
}

void TestMessageDialog::on_pushButton_clicked()
{
    if (auto cm = ui->comboBox->model()->index(ui->comboBox->currentIndex(), 0).data((int)PhoneDirectoryModel::Role::Object).value<ContactMethod*>()) {
        cm->sendOfflineTextMessage(ui->lineEdit->text());
    }
}

void TestMessageDialog::incoming_message_receveid(ContactMethod *cm, QString msg)
{
    Q_UNUSED(cm)
    ui->label->setText(ui->label->text()+" "+msg);
}

