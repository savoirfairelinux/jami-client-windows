#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include <QWidget>

namespace Ui {
class SummaryWidget;
}

class SummaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryWidget(QWidget *parent = 0);
    ~SummaryWidget();

    void setPerson(const QModelIndex &parent);
private:
    Ui::SummaryWidget *ui;
};

#endif // SUMMARYWIDGET_H
