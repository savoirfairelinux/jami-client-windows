/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

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
