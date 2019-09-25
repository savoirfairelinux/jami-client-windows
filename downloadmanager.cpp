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

#ifdef Q_OS_WIN
#include <windows.h>
#endif // !Q_OS_WIN

DownloadManager::DownloadManager()
{}

void DownloadManager::downloadFile(const QUrl& fileUrl,
                                   const QString& path,
                                   bool withUI,
                                   std::function<void(int)> doneCb)
{
    if (currentDownload_ && currentDownload_->isRunning()) {
        qWarning() << "DownloadManager::downloadFile - currently downloading";
        return;
    }

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

    QNetworkRequest request(fileUrl);
    currentDownload_ = manager_.get(request);

    currentDownload_->disconnect();

    connect(currentDownload_, SIGNAL(finished()), this, SLOT(slotDownloadFinished()));
    connect(currentDownload_, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
    connect(currentDownload_, SIGNAL(readyRead()), this, SLOT(slotHttpReadyRead()));

#if QT_CONFIG(ssl)
    connect(currentDownload_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(QList<QSslError>)));
#endif
    connect(&progressBar_, &updateDownloadDialog::isCanceled,
        [this] {
            cancelDownload();
        });

    if (withUI_) {
        progressBar_.exec();
    }
}

void DownloadManager::slotDownloadFinished()
{
    statusCode_ = currentDownload_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpRequestAborted_) {
        // request aborted
        statusCode_ = 0;
    }

    currentDownload_->deleteLater();
    currentDownload_ = nullptr;

    file_->flush();
    file_->close();
    file_.reset(nullptr);

    if (withUI_) {
        progressBar_.setMaximum(0);
        progressBar_.setValue(0);
        progressBar_.update("0");
        progressBar_.done(0);
    }

    if (doneCb_)
        doneCb_(statusCode_);

}

void DownloadManager::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // If the number of bytes to be downloaded is not known, bytesTotal will be -1.
    if (bytesTotal < 0) {
        qDebug() << "Download File Size is Unknown";
        progressBar_.setMaximum(0);
        progressBar_.setValue(0);
        progressBar_.update(QString("0, File Size is Unknown"));
        return;
    }
    progressBar_.setMaximum(bytesTotal);
    progressBar_.setValue(bytesReceived);

    progressBar_.update(Utils::humanFileSize(bytesReceived) + " / " + Utils::humanFileSize(bytesTotal));
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

void DownloadManager::cancelDownload()
{
    httpRequestAborted_ = true;
    if(currentDownload_)
        currentDownload_->abort();
}
