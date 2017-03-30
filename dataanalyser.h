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
    void analyse(QSqlQuery qry, int idx, int num, QString * result, QString comp);
    void compare(int type, QString *output);
    void analyseStories(int totMatches, QVector<QString> queries, QString * result);
    void analyseConditions(int num, QVector<QString> queries, QString *result, QString comp);
    QChartView* getChart();
    void setScoresChart();
    void setPieChartMatches();
    QChartView* getChart1();
    void resetArray(int i);
    void setFirstComp();
    bool firstComp;

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

    struct TryStats{
        int no, num, converted, penalty, homeTries;
        double avg, conPercent, penPerc, homePerc, avgHome, avgAway;
        QVector <int> arr;
    } firstCompTry, secondCompTry;

    struct MatchStats{
        int num, homeWins, awayWins, maxScore;
        double homePerc, awayPerc, avg, avgHome, avgAway;
        QVector <QString> comps;
    } firstCompMatch, secondCompMatch;

    struct PenDropStats{
        int no, num, homeScores;
        double avg, avgHome, avgAway, homePerc;
        QVector <int> arr;
    } firstCompPen, secondCompPen, firstCompDrop, secondCompDrop;

    struct BookingStats{
        int no, num, homeBookings, yellows, reds, homeYellows, homeReds;
        double avg, avgHome, avgAway, homePerc, yelPer, redPer, yAvgCon, yAvgScor, yAvgDiff, rAvgCon, rAvgScor, rAvgDiff;
        QVector <int> arr;
    } firstCompBookings, secondCompBookings;

    QChartView *chart1, *chart2;
    QVector <int> homeScoresArr, awayScoresArr;
    QString firstCompName, secondCompName;

    void analyseTries(QSqlQuery qry, int totMatches, QString * result);
    void tryDataCruncher(QSqlQuery qry, int totMatches);
    void analyseMatches(QSqlQuery qry, QString * result);
    void matchDataCruncher(QSqlQuery qry);
    void analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches, QString * result);
    void PenDropDataCruncher(QSqlQuery qry, int totMatches, int eve);
    void analyseBookings(QSqlQuery qry, int totMatches, QString * result);
    void bookingsDataCruncher(QSqlQuery qry, int totMatches);
    QString logMatches(int id, QVector <Score> home, QVector <Score> away);
    void organiseMatches(QVector<Match> *matches);
    QVector <Score> sortMatches(QVector <Score> scores, int left, int right);
    void generateStories(QVector<Match> * matches, QVector<QVector<int> > *diffs);
    void groupStories(QString * output, QVector<QVector<int> > *diffs);
    void tryConditions(QSqlQuery qry, QVector <Booking> * bookings);
    void penDropConditions(QSqlQuery qry, int eve, QVector <Booking> * bookings);
    void setPieChart(double perc);
    void setPieChart2(double a, double b);
    void setChart1(QString str, QVector<int> arr);
    void setChart2(QVector<int> arrA, QVector<int> arrB);
    QString getMatchInfo(int matId);
    void compareMatches(QString * result);
    void compareTries(QString * result);
    void comparePen(QString * result);
    void compareDrop(QString * result);
    void compareBookings(QString * result);
};

#endif // DATAANALYSER_H
