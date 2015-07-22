#include "summarywidget.h"
#include "ui_summarywidget.h"

#include <QLabel>
#include <QDate>

#include "recentmodel.h"
#include "call.h"
#include "media/recordingmodel.h"
#include "media/textrecording.h"
#include "media/text.h"

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
            auto dir = idx.data(static_cast<int>(Media::TextRecording::Role::Direction));
            QListWidgetItem* item = new QListWidgetItem();
            if (dir.isValid() &&  dir.value<int>() == (int) Media::Text::Direction::OUT) {
                item->setTextAlignment(Qt::AlignRight);
            }
            item->setText(idx.data(Qt::DisplayRole).value<QString>());
            ui->summaryList->addItem(item);
        }
    }
    ui->summaryList->scrollToBottom();
}
