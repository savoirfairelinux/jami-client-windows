#ifndef CONTACTREQUESTLISTWIDGET_H
#define CONTACTREQUESTLISTWIDGET_H

#include <QTreeView>

namespace Ui {
class ContactRequestListWidget;
}

class ContactRequestListWidget : public QTreeView
{
    Q_OBJECT

public:
    explicit ContactRequestListWidget(QWidget *parent = 0);
    ~ContactRequestListWidget();
    void setItemModel(QAbstractItemModel *model);

private:
    Ui::ContactRequestListWidget *ui;
    QPersistentModelIndex hoveredCRIndex_;
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CONTACTREQUESTLISTWIDGET_H
