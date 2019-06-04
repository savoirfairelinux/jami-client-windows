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
#include "downloadmanager.h"

#include "updatedownloaddialog.h"
#include "utils.h"

#include <QMessageBox>

#include <windows.h>

DownloadManager::DownloadManager()
{}

void DownloadManager::downloadFile(const QUrl& fileUrl,
                                   const QString& path,
                                   bool withUI,
                                   std::function<void(int)> doneCb)
{
    doneCb_ = doneCb;
    withUI_ = withUI;
    QFileInfo fileInfo(fileUrl.path());
    QString fileName = fileInfo.fileName();

    file_.reset(new QFile(path + "/" + fileName));
    if (!file_->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0,
            tr("Update"),
            tr("Unable to open file for writing"));
        file_.reset(nullptr);
        return;
    }

    downloadTime_.start();

    QNetworkRequest request(fileUrl);
    currentDownload_ = manager_.get(request);

    currentDownload_->disconnect();

    connect(currentDownload_, SIGNAL(finished()), this, SLOT(slotDownloadFinished()), Qt::QueuedConnection);
    connect(currentDownload_, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)), Qt::QueuedConnection);
    connect(currentDownload_, SIGNAL(readyRead()), this, SLOT(slotHttpReadyRead()), Qt::QueuedConnection);

#if QT_CONFIG(ssl)
    connect(currentDownload_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(QList<QSslError>)), Qt::QueuedConnection);
#endif

    if (withUI_) {
        progressBar_.exec();
    }
}

void DownloadManager::slotDownloadFinished()
{
    statusCode_ = currentDownload_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    currentDownload_->deleteLater();
    currentDownload_ = nullptr;

    file_->flush();
    file_->close();
    file_.reset(nullptr);

    if (withUI_) {
        progressBar_.setMaximum(0);
        progressBar_.setValue(0);
        progressBar_.update("0");
        progressBar_.close();
    }

    if (doneCb_)
        doneCb_(statusCode_);

    //emit downloadFinished(statusCode_);
}

void DownloadManager::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar_.setMaximum(bytesTotal);
    progressBar_.setValue(bytesReceived);

    int presentTime = downloadTime_.elapsed();
    // calculate the download speed
    double speed = (bytesReceived - previousDownloadBytes_) * 1000.0 / (presentTime - previousTime_);
    if (speed < 0)
        speed = 0;
    previousTime_ = presentTime;
    previousDownloadBytes_ = bytesReceived;

    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }

    progressBar_.update(QString::number(speed) + " " + unit);
}

void DownloadManager::slotHttpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file_)
        file_->write(currentDownload_->readAll());
}

void DownloadManager::slotSslErrors(const QList<QSslError>& sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError& error : sslErrors)
        QMessageBox::critical(0, tr("Update"), error.errorString());
    return;
#else
    Q_UNUSED(sslErrors);
#endif
}

int DownloadManager::getDownloadStatus()
{
    return statusCode_;
}
