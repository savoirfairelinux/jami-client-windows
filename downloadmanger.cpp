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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   */

#include <windows.h>

#include <QMessageBox>

#include "downloadbar.h"
#include "downloadmanger.h"

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
{
    //Download it into %TEMP%
    downloadpath_ = WinGetEnv("TEMP");
}

void DownloadManager::doDownload(const QUrl& url)
{
    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();

    if (fileName.isEmpty())
        fileName = "download";

    file_.reset(new QFile(downloadpath_ + "/" + fileName));
    if (!file_->open(QIODevice::WriteOnly)) {
        QMessageBox::information(0, "Unable to Open File Path", "Error!");
        file_.reset(nullptr);
        return;
    }

    QNetworkRequest request(url);
    // get() method posts a request
    // to obtain the contents of the target request
    // and returns a new QNetworkReply object
    // opened for reading which emits
    // the readyRead() signal whenever new data arrives.
    currentDownload_ = manager_.get(request);

    downloadTime_.start();
    //downloadProgress() signal is emitted when data is received
    connect(currentDownload_, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    // This signal is emitted when the reply has finished processing.
    // After this signal is emitted,
    // there will be no more updates to the reply's data or metadata.
    connect(currentDownload_, SIGNAL(finished()), this, SLOT(downloadFinished()));
    // Whenever more data is received from the network,
    // this readyRead() signal is emitted
    connect(currentDownload_, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

    probar_.exec();
}

void DownloadManager::downloadFinished()
{
    // donload finished normally
    probar_.setMaximum(0);
    probar_.setValue(0);
    file_->flush();
    file_->close();

    currentDownload_->deleteLater();
    currentDownload_ = nullptr;
    file_.reset(nullptr);
    probar_.close();
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    probar_.setMaximum(bytesTotal);
    probar_.setValue(bytesReceived);

    int presentTime = downloadTime_.elapsed();
    // calculate the download speed
    double speed = (bytesReceived - previousDownloadBytes_) * 1000.0 / (presentTime - previousTime_);
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

    probar_.update(QString::number(speed) + " " + unit);
}

void DownloadManager::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file_)
        file_->write(currentDownload_->readAll());
}

QString DownloadManager::versionOnline()
{
    QString urlstr = "https://dl.jami.net/windows/testver";
    QUrl url = QUrl::fromEncoded(urlstr.toLocal8Bit());
    doDownload(url);

    QFile file(downloadpath_ + "/" + "testver");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "Downloaded Version File Error", file.errorString());
    }
    QTextStream in(&file);
    QString onlineVersion = in.readLine();
    file.close();

    return onlineVersion;
}

const char* DownloadManager::WinGetEnv(const char* name)
{
    const DWORD buffSize = 65535;
    static char buffer[buffSize];
    if (GetEnvironmentVariableA(name, buffer, buffSize)) {
        return buffer;
    } else {
        return 0;
    }
}
