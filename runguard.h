// From: https://stackoverflow.com/a/28172162

#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class RunGuard : public QObject
{
    Q_OBJECT;
public:
    RunGuard(const QString& key);
    ~RunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private slots:
    void tryRestorePrimaryInstance();

private:
    const QString key_;
    const QString memLockKey_;
    const QString sharedmemKey_;

    QSharedMemory sharedMem_;
    QSystemSemaphore memLock_;

    QLocalSocket *socket_;
    QLocalServer *server_;

    Q_DISABLE_COPY(RunGuard)
};