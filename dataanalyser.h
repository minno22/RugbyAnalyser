#ifndef DATAANALYSER_H
#define DATAANALYSER_H

#include <QString>
#include <QtSql>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class DataAnalyser
{
public:
    struct dataSet{
        int event;
        QString comp;
        int within;
        int time;
        bool set = false;
    };

    DataAnalyser();
    QString analyse(QSqlQuery qry, int idx, int num);
    QString compare(QString str1, QString str2);
    QString analyseStories(int totMatches, QVector <QString> queries);
    QString analyseConditions(int num, QVector<QString> queries);
    QChartView* getChart();
    void setPieChart1();
    QChartView* getChart1();
    void resetArray();

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
        QString colour;
        int time;
        int team;
        int scored;
        int conceded;
    };

    struct Story{
        int idx, max = 0, min = 0, last = 0, pos = 0, neg = 0;
        double slope, yInt;
    };

    QChartView *chart1, *chart2;
    QVector<int> arr;

    QString analyseTries(QSqlQuery qry, int totMatches);
    QString analyseMatches(QSqlQuery qry);
    QString analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches);
    QString analyseBookings(QSqlQuery qry, int totMatches);
    QString logMatches(int id, QVector <Score> home, QVector <Score> away);
    void organiseMatches(QVector<Match> *matches);
    QVector <Score> sortMatches(QVector <Score> scores, int left, int right);
    void generateStories(QVector<Match> * matches, QString * output, QVector<QVector<int> > *diffs);
    void groupStories(QString * output, QVector<QVector<int> > *diffs);
    QString tryConditions(QSqlQuery qry, QVector <Booking> * bookings);
    QString penDropConditions(QSqlQuery qry, int eve, QVector <Booking> * bookings);
    void setPieChart(double perc);
};

#endif // DATAANALYSER_H
