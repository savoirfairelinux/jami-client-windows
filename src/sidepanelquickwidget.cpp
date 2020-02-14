/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "sidepanelquickwidget.h"

SidePanelQuickWidget::SidePanelQuickWidget(QWidget* parent) :
    QQuickWidget(QUrl(QStringLiteral("")), parent)
{
    // set image provider first, since when adding source, images are needed to be interpretted
    engine()->addImageProvider(QLatin1String("accountImage"), new AccountImageProvider(this));
    accountListUpdate();
    setSource(QUrl(QStringLiteral("qrc:/src/SidePanel.qml")));

    // widget setup
    setMouseTracking(true);
    setMinimumWidth(rootObject()->property("minWidth").toInt());
    setMinimumHeight(rootObject()->property("minHeight").toInt());
}

SidePanelQuickWidget::~SidePanelQuickWidget()
{
}

void
SidePanelQuickWidget::resizeEvent(QResizeEvent* event)
{
    rootObject()->setProperty("minWidth", event->size().width());
    rootObject()->setProperty("minHeight", event->size().height());
}

void
SidePanelQuickWidget::accountListUpdate()
{
    accountListModel_.reset(new AccountListModel());
    QQmlContext* ctxt = engine()->rootContext();
    ctxt->setContextProperty("accountListModel", accountListModel_.get());
}