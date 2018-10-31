/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "messagewebview.h"

#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include <ciso646>

#include "utils.h"
#include "messagemodel.h"

MessageWebView::MessageWebView(QWidget *parent)
    : QWebEngineView(parent)
{
    //setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    connect(this, &QWebEngineView::renderProcessTerminated,
        [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            QString status;
            switch (termStatus) {
            case QWebEnginePage::NormalTerminationStatus:
                qDebug() << "Render process normal exit";
                break;
            case QWebEnginePage::AbnormalTerminationStatus:
                qDebug() << "Render process abnormal exit";
                break;
            case QWebEnginePage::CrashedTerminationStatus:
                qDebug() << "Render process crashed";
                break;
            case QWebEnginePage::KilledTerminationStatus:
                qDebug() << "Render process killed";
                break;
            }
        });
}

MessageWebView::~MessageWebView()
{
}

void
MessageWebView::insertStyleSheet(const QString &name, const QString &source)
{
    QWebEngineScript script;
    auto simplifiedCSS = source.simplified().replace("'", "\"");
    QString s = QString::fromLatin1("(function() {"\
                                    "    var node = document.createElement('style');"\
                                    "    node.id = '%1';"\
                                    "    node.innerHTML = '%2';"\
                                    "    document.body.appendChild(node);"\
                                    "})()").arg(name).arg(simplifiedCSS);
    page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);
}