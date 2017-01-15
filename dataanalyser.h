#ifndef DATAANALYSER_H
#define DATAANALYSER_H

#include <QString>
#include <QtSql>

class DataAnalyser
{
public:
    DataAnalyser();
    QString analyse(QSqlQuery qry, int idx, int num);

private:
    double const scale = 0.01;
    QString analyseTries(QSqlQuery qry, int totMatches);
    QString analyseMatches(QSqlQuery qry);
    QString analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches);
    QString analyseBookings(QSqlQuery qry, int totMatches);

};

#endif // DATAANALYSER_H
