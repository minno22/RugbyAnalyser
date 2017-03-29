#ifndef DATAANALYSER_H
#define DATAANALYSER_H

#include <QString>
#include <QtSql>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class DataAnalyser
{
public:
    DataAnalyser();
    void analyse(QSqlQuery qry, int idx, int num, QString * result);
    QString compare(QString str1, QString str2);
    void analyseStories(int totMatches, QVector<QString> queries, QString * result);
    void analyseConditions(int num, QVector<QString> queries, QString *result);
    QChartView* getChart();
    void setChart1(QString str);
    void setScoresChart();
    void setPieChartMatches();
    QChartView* getChart1();
    void resetArray(int i);

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
    QVector <int> arr, scoresArr;
    QVector <QString>  comps;
    double homeWinPerc;

    void analyseTries(QSqlQuery qry, int totMatches, QString * result);
    void analyseMatches(QSqlQuery qry, QString * result);
    void analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches, QString * result);
    void analyseBookings(QSqlQuery qry, int totMatches, QString * result);
    QString logMatches(int id, QVector <Score> home, QVector <Score> away);
    void organiseMatches(QVector<Match> *matches);
    QVector <Score> sortMatches(QVector <Score> scores, int left, int right);
    void generateStories(QVector<Match> * matches, QString * output, QVector<QVector<int> > *diffs);
    void groupStories(QString * output, QVector<QVector<int> > *diffs);
    void tryConditions(QSqlQuery qry, QVector <Booking> * bookings);
    void penDropConditions(QSqlQuery qry, int eve, QVector <Booking> * bookings);
    void setPieChart(double perc);
    QString getMatchInfo(int matId);
};

#endif // DATAANALYSER_H
