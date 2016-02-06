#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>

#include "Log.h"

void createDB();

using namespace QSqliteProfiler;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Log2db");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Export log to the database");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", "Source log");
    parser.addPositionalArgument("destination", "Destination database");

    parser.process(app);

    const auto args = parser.positionalArguments();
    if (args.count() != 2)
    {
        qCritical() << "Incorrect number of arguments";
        return 1;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(args[1]);
    if (!db.open())
    {
        qCritical() << "Can not open database:" << db.lastError().text();
        exit(2);
    }
    else
    {
        createDB();
    }

    QFile log(args[0]);
    if(!log.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can not open log file:" << log.errorString();
        exit(4);
    }

    QDataStream stream(&log);

    QSqlQuery query;
    query.prepare("INSERT INTO query "
            "VALUES (?, ?, ?)");

    db.transaction();
    while(!log.atEnd())
    {
        LogRecord record;
        stream>>record;
        query.bindValue(0, record.timestamp);
        query.bindValue(1, record.duration);
        query.bindValue(2, record.query);

        query.exec();
    }

    db.commit();
    return 0;
}

void createDB()
{
    const QString createTable = "CREATE TABLE IF NOT EXISTS query ("
            "timestamp NUMERIC NOT NULL, "
            "time NUMERIC NOT NULL, "
            "query TEXT NOT NULL)";
    QSqlQuery query;
    if (!query.exec(createTable))
    {
        qCritical() << "Can not create table:" << query.lastError().text();
        exit(3);
    }
}
