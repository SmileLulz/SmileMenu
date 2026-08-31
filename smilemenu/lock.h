#pragma once
#include <QLockFile>
#include <QString>

class SingleInstanceLock
{
public:
    explicit SingleInstanceLock(const QString &appName = "smilemenu");
    ~SingleInstanceLock();

    bool tryLock();
    void release();
    bool isRunning() const;
    qint64 getPid() const;

private:
    QString m_lockFilePath;
    QLockFile m_lockFile;
    bool m_locked;
};
