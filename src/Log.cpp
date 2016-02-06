#include <mutex>
#include <sqlite3.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QMetaType>
#include <QtCore/QDataStream>
#include <QtCore/QCoreApplication>

#include <QtSql/QSqlDriver>

Q_DECLARE_OPAQUE_POINTER(sqlite3*);
Q_DECLARE_METATYPE(sqlite3*);

#include "Log.h"

typedef std::lock_guard<std::mutex> LockGuard;

namespace
{

void profile(void* userData, const char* sql, sqlite3_uint64 time)
{
    using namespace QSqliteProfiler;

    const quint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    LogRecord record{timestamp, time, sql};

    auto log = static_cast<Log*>(userData);
    log->write(record);
}

}

namespace QSqliteProfiler
{

Log& Log::instance()
{
    static Log log;
    return log;
}

struct Log::LogImpl
{
    ~LogImpl()
    {
        LockGuard lock(fileMutex);

        if(file.isOpen())
        {
            file.flush();
            file.close();
        }
    }

    QFile file;
    QDataStream stream;
    std::mutex fileMutex;
};

Log::Log() :
        m_impl(new LogImpl)
{

}

Log::~Log() = default;

void Log::setProfile(QSqlDriver* driver)
{
    QVariant v = driver->handle();
    const QString tpName = v.typeName();

    if (v.isValid() && (tpName == "sqlite3*"))
    {
        sqlite3* handle = v.value<sqlite3*>();
        if (handle != nullptr)
        {
            sqlite3_profile(handle, profile, this);
        }
    }
}

void Log::setLogFile(const QString& file)
{
    LockGuard lock(m_impl->fileMutex);

    if(m_impl->file.isOpen())
    {
        m_impl->file.close();
    }

    m_impl->file.setFileName(file);
    auto isOpen = m_impl->file.open(QIODevice::WriteOnly);
    if(isOpen)
    {
        m_impl->stream.setDevice(&m_impl->file);
    }
    else
    {
        qCritical()<<"Can not open file for writing, file"<<file;
        qDebug()<<m_impl->file.errorString();
        exit(1);
    }
}

void Log::write(const LogRecord& record)
{
    LockGuard lock(m_impl->fileMutex);
    if(m_impl->file.isOpen())
    {
        m_impl->stream<<record;
    }
}
}
