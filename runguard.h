// From: https://stackoverflow.com/a/28172162

#pragma once

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class RunGuard
{

public:
    RunGuard(const QString& key);
    ~RunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY(RunGuard)
};