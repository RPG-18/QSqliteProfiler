#pragma once

#include <memory>

#include <QtCore/QString>

class QSqlDriver;

namespace QSqliteProfiler
{

struct LogRecord
{
    quint64 timestamp;
    quint64 duration;
    QString query;
};

class Log
{
public:

    static Log& instance();

    void setProfile(QSqlDriver* driver);

    void setLogFile(const QString& file);
    void write(const LogRecord& record);

private:

    Log();
    ~Log();

private:

    struct LogImpl;
    std::unique_ptr<LogImpl> m_impl;
};
}

inline QDataStream & operator<< (QDataStream& stream, const QSqliteProfiler::LogRecord record)
{
    stream<<record.timestamp<<record.duration<<record.query;

}

inline QDataStream & operator>> (QDataStream& stream, QSqliteProfiler::LogRecord& record)
{
    stream>>record.timestamp>>record.duration>>record.query;
}
