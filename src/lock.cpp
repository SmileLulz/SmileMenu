#include "lock.h"
#include <QDir>
#include <QDebug>

SingleInstanceLock::SingleInstanceLock(const QString &appName)
    : m_lockFilePath(QDir::tempPath() + "/" + appName + "-lock/lock.pid"),
      m_lockFile(m_lockFilePath),
      m_locked(false)
{
    QDir().mkpath(QFileInfo(m_lockFilePath).path());
}

SingleInstanceLock::~SingleInstanceLock()
{
    release();
}

bool SingleInstanceLock::tryLock()
{
    if (m_locked)
        return true;
    m_locked = m_lockFile.tryLock();
    return m_locked;
}

void SingleInstanceLock::release()
{
    if (m_locked) {
        m_lockFile.unlock();
        m_locked = false;
        QFile::remove(m_lockFilePath);
    }
}

bool SingleInstanceLock::isRunning() const
{
    return m_lockFile.isLocked();
}

qint64 SingleInstanceLock::getPid() const
{
    qint64 pid;
    if (m_lockFile.getLockInfo(&pid, nullptr, nullptr))
        return pid;
    return -1;
}
