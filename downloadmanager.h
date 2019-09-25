/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "updatedownloaddialog.h"

class QSslError;

class DownloadManager : public QObject {
    Q_OBJECT
public:
    static DownloadManager& instance() {
        static DownloadManager* instance_ = new DownloadManager();
        return *instance_;
    }

    void downloadFile(const QUrl& fileUrl,
                      const QString& path,
                      bool withUI,
                      std::function<void(int)> doneCb = {});
    int getDownloadStatus();
    void cancelDownload();

public slots:
    void slotSslErrors(const QList<QSslError>& sslErrors);
    void slotDownloadFinished();
    void slotDownloadProgress(qint64 bytesRead, qint64 totalBytes);
    void slotHttpReadyRead();

private:
    DownloadManager();

    QNetworkAccessManager manager_;
    QNetworkReply* currentDownload_;
    UpdateDownloadDialog progressBar_;
    std::unique_ptr<QFile> file_;
    int statusCode_;
    bool withUI_;
    bool httpRequestAborted_ { false };

    std::function<void(int)> doneCb_;

};
