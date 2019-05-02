/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#pragma once

#include <memory>

#include <QtCore>
#include <QtNetwork>

#include "downloadbar.h"

class QSslError;

class DownloadManager : public QObject {
    Q_OBJECT

    QNetworkAccessManager manager_;
    QNetworkReply* currentDownload_;
    updateDownloadDialog probar_;
    std::unique_ptr<QFile> file_;
    QTime downloadTime_;
    int previousTime_ = 0;
    qint64 previousDownloadBytes_ = 0;
    QString downloadpath_;
    int statusCode_;

public:
    explicit DownloadManager(QObject* parent = nullptr);
    void doDownload(const QUrl& url);
    QString versionOnline();
    static const char* WinGetEnv(const char* name);
    int getDownloadStatus();

public slots:
    void sslErrors(const QList<QSslError>& sslErrors);
    void downloadFinished();
    void downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void httpReadyRead();
};
