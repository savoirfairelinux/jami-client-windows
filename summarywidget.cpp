#include "summarywidget.h"
#include "ui_summarywidget.h"

#include <QLabel>
#include <QDate>

#include "recentmodel.h"
#include "call.h"
#include "media/recordingmodel.h"
#include "media/textrecording.h"

SummaryWidget::SummaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SummaryWidget)
{
    ui->setupUi(this);
}

SummaryWidget::~SummaryWidget()
{
    delete ui;
}

void
SummaryWidget::setPerson(const QModelIndex& parent)
{
    if (RecentModel::instance()->hasActiveCall(parent))
        return this->hide();

    this->show();
    ui->summaryList->clear();
    int row = RecentModel::instance()->rowCount(parent);
    for (int i=0; i < row; i++) {
        auto idx = RecentModel::instance()->index(i, 0, parent);
        auto call = idx.data(static_cast<int>(Call::Role::Object)).value<Call*>();
        if (call) {
            QListWidgetItem *item = new QListWidgetItem(call->formattedName()
                                                        + " / "
                                                        + idx.data(static_cast<int>(Call::Role::FormattedDate)).toString()
                                                        + " / "
                                                        + call->length()
                                                        + (call->isMissed() == true ? "Missed" : ""));
            if (call->direction() == Call::Direction::OUTGOING)
                item->setTextAlignment(Qt::AlignRight);
            ui->summaryList->addItem(item);
        } else {
            ui->summaryList->addItem(idx.data(Qt::DisplayRole).value<QString>());
        }
    }
    ui->summaryList->scrollToBottom();
}
