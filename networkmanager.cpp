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
#include "networkmanager.h"

#include "updatedownloaddialog.h"
#include "utils.h"

#include <QMessageBox>

#ifdef Q_OS_WIN
#include <windows.h>
#endif // !Q_OS_WIN

NetWorkManager::NetWorkManager(QObject* parent) :
    QObject(parent)
{}
NetWorkManager::~NetWorkManager()
{}

void
NetWorkManager::getRequestFile(const QUrl& fileUrl,
                               const QString& path,
                               bool withUI,
                               std::function<void(int)> doneCbRequestInFile)
{
    if (reply_ && reply_->isRunning()) {
        qWarning() << "NetworkManager::getRequestFile - currently downloading";
        return;
    }

    doneCbRequestInFile_ = doneCbRequestInFile;
    bool requestInFile = !(path.isEmpty());

    if (requestInFile) {
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
    }

    QNetworkRequest request(fileUrl);
    reply_ = manager_.get(request);

    reply_->disconnect();

    connect(reply_, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
        [this, withUI,requestInFile] (QNetworkReply::NetworkError code) {
            reply_->disconnect();
            refresh(requestInFile);
            if (withUI) {
                progressBar_.setMaximum(0);
                progressBar_.setValue(0);
                progressBar_.update("0");
                progressBar_.done(0);
            }
            if (requestInFile) {
                if (doneCbRequestInFile_)
                    doneCbRequestInFile_(code);
            } else {
                if (doneCbRequest_)
                    doneCbRequest_(code, "");
            }
        });

    if (requestInFile) {
        connect(reply_, SIGNAL(finished()), this, SLOT(slotGetRequestInFileFinished()));
        connect(reply_, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
        connect(reply_, SIGNAL(readyRead()), this, SLOT(slotHttpReadyRead()));

#if QT_CONFIG(ssl)
        connect(reply_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(QList<QSslError>)));
#endif
        connect(&progressBar_, &UpdateDownloadDialog::isCanceled,
            [this] {
                cancelRequest();
            });
    } else {
        connect(reply_, SIGNAL(finished()), this, SLOT(slotGetRequestFinished()));
#if QT_CONFIG(ssl)
        connect(reply_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(QList<QSslError>)));
#endif
    }

    if (withUI) {
        connect(reply_, &QNetworkReply::finished,
            [this] {
                progressBar_.setMaximum(0);
                progressBar_.setValue(0);
                progressBar_.update("0");
                progressBar_.done(0);
            });
        progressBar_.exec();
    }
}

void
NetWorkManager::slotGetRequestInFileFinished()
{
    int statusCode = reply_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    refresh(true);
    if (doneCbRequestInFile_)
        doneCbRequestInFile_(statusCode);
}

void
NetWorkManager::refresh(bool requestInFile)
{
    reply_->deleteLater();
    reply_ = nullptr;

    if (requestInFile) {
        file_->flush();
        file_->close();
        file_.reset(nullptr);
    }
}

void
NetWorkManager::slotGetRequestFinished()
{
    int statusCode = reply_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString response = QString(reply_->readAll());

    refresh(false);

    if (doneCbRequest_)
        doneCbRequest_(statusCode, response);
}

void
NetWorkManager::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
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

void
NetWorkManager::slotHttpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file_)
        file_->write(reply_->readAll());
}

void
NetWorkManager::slotSslErrors(const QList<QSslError>& sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError& error : sslErrors)
        QMessageBox::critical(0, tr("Update"), error.errorString());
    return;
#else
    Q_UNUSED(sslErrors);
#endif
}

void
NetWorkManager::cancelRequest()
{
    if(reply_)
        reply_->abort();
}

void
NetWorkManager::getRequestReply(const QUrl & fileUrl,
                                std::function<void(int, QString)> doneCbRequest)
{
    doneCbRequest_ = doneCbRequest;
    getRequestFile(fileUrl, "", false);
}
