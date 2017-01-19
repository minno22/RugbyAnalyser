#ifndef DATAANALYSER_H
#define DATAANALYSER_H

#include <QString>
#include <QtSql>

class DataAnalyser
{
public:
    struct dataSet{
        int event;
        QString comp;
        int within;
        int time;
        int cond;
        bool set = false;
    };

    DataAnalyser();
    QString analyse(QSqlQuery qry, int idx, int num);
    QString compare(QString str1, QString str2);
    QString analyseStories(int totMatches, QVector <QString> queries);
    QString analyseConditions(int num, QVector<QString> queries);

private:
    double const scale = 0.01;
    struct Score{
        int value;
        int time;
    };

    struct Match{
        int id;
        QVector <Score> home, away;
    };

    struct Booking{
        int id;
        int time;
        int team;
        int scored;
        int conceded;
    };

    QString analyseTries(QSqlQuery qry, int totMatches);
    QString analyseMatches(QSqlQuery qry);
    QString analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches);
    QString analyseBookings(QSqlQuery qry, int totMatches);
    QString logMatches(int id, QVector <Score> home, QVector <Score> away);
    void organiseMatches(QVector<Match> *matches);
    QVector <Score> sortMatches(QVector <Score> scores, int left, int right);
    void generateStories(QVector<Match> * matches, QString * output, QVector<QVector<int> > *diffs);
    void groupStories(QString * output, QVector<QVector<int> > *diffs);
    QString tryConditions(QSqlQuery qry, int totMatches, QVector <Booking> * bookings);
    QString penDropConditions(QSqlQuery qry, int eve, int totMatches, QVector <Booking> * bookings);
};

#endif // DATAANALYSER_H
