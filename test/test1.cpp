#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlDatabase>

#include "Log.h"

int main(int argc, char *argv[])
{

    using namespace QSqliteProfiler;

    QCoreApplication app(argc, argv);
    Log::instance().setLogFile("sqlite.profile");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");

    if (!db.open()) {
           qDebug()<<"Test data base not open";
           return 1;
    }

    Log::instance().setProfile(db.driver());

    QSqlQuery query;
    query.exec( "CREATE TABLE my_table ("
            "number integer PRIMARY KEY NOT NULL, "
            "address VARCHAR(255), "
            "age integer"
            ");");

    QString str;
    //db.transaction();
    for(int i = 0; i < 100; ++i)
    {
        QSqlQuery query1(db);
        query1.prepare("INSERT INTO my_table(number, address, age) VALUES (?, ?, ?)");
        query1.bindValue(0, i);
        query1.bindValue(1, "hello world str.");
        query1.bindValue(2, 37);
        query1.exec();
    }
    //db.commit();

    return 0;
}
