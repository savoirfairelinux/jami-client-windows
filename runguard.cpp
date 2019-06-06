// From: https://stackoverflow.com/a/28172162

#include "runguard.h"

#include <QCryptographicHash>

#include "mainwindow.h"

namespace
{

QString generateKeyHash(const QString& key, const QString& salt)
{
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}

}

RunGuard::RunGuard(const QString& key)
    : key_(key)
    , memLockKey_(generateKeyHash(key, "_memLockKey"))
    , sharedmemKey_(generateKeyHash(key, "_sharedmemKey"))
    , sharedMem_(sharedmemKey_)
    , memLock_(memLockKey_, 1)
{}

RunGuard::~RunGuard()
{
    release();
}

void
RunGuard::tryRestorePrimaryInstance()
{
    MainWindow::instance().showWindow();
}

bool RunGuard::isAnotherRunning()
{
    if (sharedMem_.isAttached())
        return false;

    memLock_.acquire();
    const bool isRunning = sharedMem_.attach();
    if (isRunning)
        sharedMem_.detach();
    memLock_.release();

    return isRunning;
}

bool RunGuard::tryToRun()
{
    if (isAnotherRunning()) {
        // This is a secondary instance,
        // connect to the primary instance to trigger a restore
        // then fail.
        if (socket_ == nullptr) {
            socket_ = new QLocalSocket();
        }
        if (socket_->state() == QLocalSocket::UnconnectedState ||
            socket_->state() == QLocalSocket::ClosingState) {
            socket_->connectToServer(key_);
        }
        if (socket_->state() == QLocalSocket::ConnectingState) {
            socket_->waitForConnected();
        }
        return false;
    }

    memLock_.acquire();
    const bool result = sharedMem_.create(sizeof(quint64));
    memLock_.release();
    if (!result) {
        release();
        return false;
    }

    // This is the primary instance,
    // listen for subsequent instances.
    QLocalServer::removeServer(key_);
    server_ = new QLocalServer();
    server_->setSocketOptions(QLocalServer::UserAccessOption);
    server_->listen(key_);
    QObject::connect(
        server_,
        &QLocalServer::newConnection,
        this,
        &RunGuard::tryRestorePrimaryInstance
    );

    return true;
}

void RunGuard::release()
{
    memLock_.acquire();
    if (sharedMem_.isAttached())
        sharedMem_.detach();
    memLock_.release();
}