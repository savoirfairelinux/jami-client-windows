#include "pivotcontrol.h"

PivotControl::PivotControl(QWidget *parent) : QGraphicsView(parent)
{
    const int itemCount = 6;

    QGraphicsTextItem *tmp;

    QString text;
    qreal xPos = 0;

    //QParallelAnimationGroup *mGroupAnimHeaderTmp = new QParallelAnimationGroup;

    this->setScene(new QGraphicsScene());

    for(int i = 0; i < itemCount; ++i) {
        tmp = new QGraphicsTextItem();
        text = "loremIpsum";
        text = text.append(QString("%1").arg(i+1));
        tmp->setPlainText(text);
        tmp->setFont(headerFont);
        tmp->adjustSize();
        tmp->setDefaultTextColor(uiTextColor);
        tmp->setPos(xPos,(componentMargin * 2 + bodyTextSize));
        xPos = xPos + tmp->textWidth() + componentMargin;

//        QPropertyAnimation *anim;
//        anim = new QPropertyAnimation(tmp, "pos");
//        anim->setDuration(animationTime);
//        anim->setPropertyName("pos");
//        anim->setEasingCurve(QEasingCurve::OutCirc);
//        mGroupAnimHeaderTmp->addAnimation(anim);
        //mHeaderAnimations.append(anim);

        mHeaderItems.append(tmp);
        scene()->addItem(tmp);
    }
    //mGroupAnimHeader = mGroupAnimHeaderTmp;
}

#include <QDebug>

void PivotControl::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "MOUSE PRESS EVENT";
    //mGroupAnimHeader = new QParallelAnimationGroup;
//    int xDif = event->pos().x() - mMouseXPosition;
//    ...
//    if(xDif < 0) {
//        ...
//        startContentAnimation(ESweepLeft);
//        startHeaderAnimation(ESweepLeft);
//    } else if(xDif > 0) {
//        ...
//        startContentAnimation(ESweepRight);
//        startHeaderAnimation(ESweepRight);
//    }
//    mMouseXPosition = event->pos().x();
}

void PivotControl::startContentAnimation(int direction)
{
//    QPropertyAnimation *anim;
//    QGraphicsTextItem *text;
//    // init animation items
//    for(int i = 0; i < itemCount; ++i) {
//        text = mContentItems.at(i);
//        anim = mContentAnimations.at(i);
//        QPointF start = text->pos();
//        QPointF end = start;
//        if(direction == ESweepLeft)
//            end.setX( end.x() - text->textWidth() - componentMargin);
//        else
//            end.setX( end.x() + text->textWidth() + componentMargin);
//        anim->setStartValue(start);
//        anim->setEndValue(end);
//    }
//    mGroupAnimContent->start();
}

PivotControl::~PivotControl()
{

}

