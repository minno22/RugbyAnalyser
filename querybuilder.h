#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QString>

class QueryBuilder
{
public:
    QueryBuilder();
    QString getQuery(int event, QString comp, int within, int time, int condition);
    QString maxId();
    QString getUnique(int idx);
    QString getMatchQuery(int id);

private:
    QString query;
    void DatePlusDays( struct tm* date, int days );
    QString setDate(int idx);
    QString setTime(int idx, int tme);
};

#endif // QUERYBUILDER_H
