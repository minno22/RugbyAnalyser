#ifndef QUERYDB_H
#define QUERYDB_H

#include <QtSql>

class QueryDB
{
public:
    QueryDB();
    void setQuery(QString qryString);
    QSqlQuery executeQuery();

private:
    QString qryString;
};

#endif // QUERYDB_H
