#ifndef PIVOTCONTROL_H
#define PIVOTCONTROL_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QList>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class PivotControl : public QGraphicsView
{
    const int componentMargin = 24;
    const int bodyTextSize = 24;
    const int headerTextSize = 16;
    const QFont headerFont = QFont("Segoe UI", headerTextSize);
    const QFont bodyFont = QFont("Segoe UI", bodyTextSize);
    const QColor uiTextColor = Qt::black;
    const QString backgroundStyle = "background-color: rgba(26,26,26)";
    const int animationTime = 400;
public:
    PivotControl(QWidget* parent = 0);
    ~PivotControl();
protected:
    void startContentAnimation(int direction);
    void mousePressEvent(QMouseEvent *event);
private:
    QList<QGraphicsTextItem*> mHeaderItems;
    QParallelAnimationGroup *mGroupAnimHeader;
};

#endif // PIVOTCONTROL_H
