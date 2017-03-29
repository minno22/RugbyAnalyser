#include "dataanalyser.h"
#include "querydb.h"
#include "querybuilder.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

//#include <Eigen/Dense>
//#include <iostream>
//#include <iomanip>

//using namespace Eigen;

DataAnalyser::DataAnalyser()
{
    for (int i = 0; i < 10; i++){
        arr.append(0);
        scoresArr.append(0);
    }
}

void DataAnalyser::analyse(QSqlQuery qry, int idx, int num, QString * result) //selects which analysis should be done
{
    switch (idx) {
    case 0: analyseMatches(qry, result);
        break;
    case 1: analyseTries(qry, num, result);
        break;
    case 2: analysePenaltiesDropGoals(qry, 0, num, result);
        break;
    case 3: analysePenaltiesDropGoals(qry, 1, num, result);
        break;
    case 4: analyseBookings(qry, num, result);
        break;
    default:
        break;
    }
}

void DataAnalyser::analyseTries(QSqlQuery qry, int totMatches, QString * result)
{
    //qDebug() << "DEBUG!!!!!!" << endl;
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeTries = 0, converted = 0, penalty = 0, no = 0, oldNo, newNo;
    while (qry.next()) {
        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr.at(b) + 1;
        int y = scoresArr.at(b) + 1;
        arr.replace(b, x);
        scoresArr.replace(b, y);
        if (b > 8){
            int x = arr.at(8) + 1;
            int y = scoresArr.at(8) + 1;
            arr.replace(8, x);
            scoresArr.replace(8, y);
        }
        num++;

        matchIdNew = qry.value(11).toInt();
        if (matchIdNew == 0){
            matchIdOld = matchIdNew;
            noMatches++;
        }
        else if (matchIdNew != matchIdOld){
            matchIdOld = matchIdNew;
            noMatches++;
        }

        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            no++;
            oldNo = newNo;
        }

        if (qry.value(13).toInt() == 0)
            homeTries++;
        if (qry.value(14).toInt() == 1)
            converted++;
        if (qry.value(15).toInt() == 1)
            penalty++;
    }
    result->append("Analysis of Tries Scored\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches) +
                ".\nTotal number of matches satisfying search criteria:\t" + QString::number(no) +
                ".\nTotal number of tries scored:\t\t\t\t" + QString::number(num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = arr.at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result->append("Tries scored between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes:\t" + QString::number(n) + ", " + QString::number(y) + "% of total tries.\n");
        }
        else{
            result->append("Tries scored after 80 minutes:\t\t\t" + QString::number(n) + ", " + QString::number(y)
                        + "% of total tries.\n\n");
        }
    }
    //qDebug() << noMatches << endl;
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeTries/(totMatches))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeTries)/(totMatches))/scale + 0.5) * scale;
    double conPercent = floor(((double)converted/num * 100) / scale + 0.5) * scale;
    double penPerc = floor(((double)penalty/num * 100) / scale + 0.5) * scale;
    double homePerc = floor(((double)homeTries/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    result->append("Average number of tries per match:\t" + QString::number(avg) + ".\nConverted tries:\t\t\t" +
                QString::number(converted) + ", " + QString::number(conPercent) + "% of total tries.\nPenalty tries:\t\t\t" +
                QString::number(penalty) + ", " + QString::number(penPerc) + "% of total tries.\nHome team tries:\t\t\t" +
                QString::number(homeTries) + ", " + QString::number(homePerc) + "% of total tries.\nAverage home tries:\t\t"
                + QString::number(avgHome) + ".\nAway team tries:\t\t\t" + QString::number(num - homeTries) + ", " +
                QString::number(100.00 - homePerc) + "% of total tries.\nAverage away tries:\t\t\t" + QString::number(avgAway)
                + ".");
}

void DataAnalyser::analyseMatches(QSqlQuery qry, QString * result)
{
    int num = 0, homeWins = 0, awayWins = 0, totHome = 0, totAway = 0, maxScore = 0;
    while (qry.next()) {
         num++;
         QString comp = qry.value(4).toString();
         bool found = false;
         for (int i = 0; i < comps.size() && !found; i++)
             if (comp == comps.at(i))
                 found = true;
         if (!found)
             comps.push_back(comp);

         int home = qry.value(5).toInt();
         int away = qry.value(6).toInt();
         if (home > away)
             homeWins++;
         else if (away > home)
             awayWins++;
         totHome += home;
         totAway += away;

         if (home > maxScore)
             maxScore = home;
         if (away > maxScore)
             maxScore = away;
    }
    double homePerc = floor(((double)homeWins/num * 100) / scale + 0.5) * scale;
    homeWinPerc = homePerc;
    double awayPerc = floor(((double)awayWins/num * 100) / scale + 0.5) * scale;
    double avg = floor(((double)(totHome + totAway)/num) / scale + 0.5) * scale;
    double avgHome = floor(((double)totHome/num) / scale + 0.5) * scale;
    double avgAway = floor(((double)totAway/num) / scale + 0.5) * scale;

    //qDebug() << totHome << " " << totAway << " " << numTeams << endl;

    result->append("Match Analysis\n\nNumber of Matches:\t" + QString::number(num) + ".\nNumber of Competitions:\t" +
                QString::number(comps.size()) + ".\n\nNumber of home wins:\t" + QString::number(homeWins) + ", " +
                QString::number(homePerc) + "% of total matches.\nNumber of away wins:\t" + QString::number(awayWins)
                + ", " + QString::number(awayPerc) + "% of total matches.\nNumber of draws:\t\t" +
                QString::number(num - (homeWins + awayWins)) + ", " + QString::number(100.00 - (homePerc + awayPerc))
                + "% of total matches.\n\nAverage combined score per match:\t" + QString::number(avg) + " points.\nAverage home score:\t\t" +
                QString::number(avgHome) + " points.\nAverage away score:\t\t" + QString::number(avgAway) +
                " points.\nMaximum score:\t\t\t" + QString::number(maxScore) + " points.");
}

void DataAnalyser::analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches, QString * result)
{
    QString big, small;
    if (eve == 0){
        big = "Penalties";
        small = "penalties";
    }
    else{
        big = "Drop Goals";
        small = "drop goals";
    }
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeScores = 0, no = 0, oldNo, newNo;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr.at(b) + 1;
        int y = scoresArr.at(b) + 1;
        arr.replace(b, x);
        scoresArr.replace(b, y);
        if (b > 8){
            int x = arr.at(8) + 1;
            int y = scoresArr.at(8) + 1;
            arr.replace(8, x);
            scoresArr.replace(8, y);
        }

        matchIdNew = qry.value(11).toInt();
        if (matchIdNew == 0){
            matchIdOld = matchIdNew;
            noMatches++;
        }
        else if (matchIdNew != matchIdOld){
            matchIdOld = matchIdNew;
            noMatches++;
        }

        if (qry.value(13).toInt() == 0)
            homeScores++;

        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            no++;
            oldNo = newNo;
        }
    }
    result->append("Analysis of " + big + " Scored\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches)
            + ".\nTotal number of matches satisfying search criteria:\t" + QString::number(no) +
                ".\nTotal number of " + small + " scored:\t\t\t" + QString::number(num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = arr.at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result->append(big + " scored between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes:\t" + QString::number(n) + ", " + QString::number(y) + "% of total " + small + ".\n");
        }
        else{
            result->append(big + " scored after 80 minutes:\t\t\t" + QString::number(n) + ", " + QString::number(y)
                        + "% of total " + small + ".\n\n\n");
        }
    }
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeScores/(totMatches))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeScores)/(totMatches))/scale + 0.5) * scale;
    double homePerc = floor(((double)homeScores/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    result->append("Average number of " + small + " per match:\t" + QString::number(avg) + ".\nHome team " + small + ":\t\t\t" +
                QString::number(homeScores) + ", " + QString::number(homePerc) +
                "% of total " + small + ".\nAverage home " + small + ":\t\t\t" + QString::number(avgHome) +
                ".\nAway team " + small + ":\t\t\t" + QString::number(num - homeScores) + ", " +
                QString::number(100.00 - homePerc) + "% of total " + small + ".\nAverage away " + small + ":\t\t\t" +
                QString::number(avgAway) + ".");
}

void DataAnalyser::analyseBookings(QSqlQuery qry, int totMatches, QString * result)
{
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeBookings = 0, yellows = 0, homeYellows = 0, homeReds = 0,
                no = 0, oldNo, newNo;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr.at(b) + 1;
        arr.replace(b, x);
        if (b > 8){
            int x = arr.at(8) + 1;
            arr.replace(8, x);
        }

        matchIdNew = qry.value(11).toInt();
        if (matchIdNew == 0){
            matchIdOld = matchIdNew;
            noMatches++;
        }
        else if (matchIdNew != matchIdOld){
            matchIdOld = matchIdNew;
            noMatches++;
        }

        int homeAway = qry.value(13).toInt();
        if (homeAway == 0)
            homeBookings++;

        if (qry.value(14).toString() == "Yellow"){
            yellows++;
            if (homeAway == 0)
                homeYellows++;
        }
        else
            if (homeAway == 0)
                homeReds++;

        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            no++;
            oldNo = newNo;
        }
    }

    result->append("Analysis of Bookings\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches) +
                ".\nTotal number of matches satisfying search conditions:\t" + QString::number(no) +
                ".\nTotal number of bookings:\t\t\t\t" + QString::number(num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = arr.at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result->append("Bookings between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes:\t" + QString::number(n) + ", " + QString::number(y) + "% of total bookings.\n");
        }
        else{
            result->append("Bookings after 80 minutes:\t\t\t" + QString::number(n) + ", " + QString::number(y)
                        + "% of total penalties.\n\n");
        }
    }
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeBookings/(totMatches))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeBookings)/(totMatches))/scale + 0.5) * scale;
    double homePerc = floor(((double)homeBookings/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    double yelPer = floor(((double)yellows/num * 100) / scale + 0.5) * scale;
    double redPer = floor(((double)(num - yellows)/num * 100) / scale + 0.5) * scale;

    int reds = num - yellows;

    result->append("Average number of bookings per match:\t" + QString::number(avg) + ".\nHome team bookings:\t\t\t" +
                QString::number(homeBookings) + ", " + QString::number(homePerc) +
                "% of total bookings.\nAverage home bookings:\t\t\t" + QString::number(avgHome) +
                ".\nAway team bookings:\t\t\t" + QString::number(num - homeBookings) + ", " +
                QString::number(100.00 - homePerc) + "% of total bookings.\nAverage away bookings:\t\t\t" +
                QString::number(avgAway) + ".\n\nTotal yellow cards:\t\t" + QString::number(yellows) + ", " +
                QString::number(yelPer) + " % of total bookings.\nHome yellows:\t\t" + QString::number(homeYellows) +
                ".\nAway yellows:\t\t" + QString::number(yellows - homeYellows) + ".\n\nTotal red cards:\t\t" +
                QString::number(reds) + ", " + QString::number(redPer) + " % of total bookings.\nHome reds:\t\t" +
                QString::number(homeReds) + ".\nAway reds:\t\t" + QString::number(reds - homeReds) + ".");
}

void DataAnalyser::organiseMatches(QVector<Match> * matches)
{
    int l = 0, r;
    for (int i = 0; i < matches->size(); i++){
        Match m = matches->at(i);
        QVector <Score> home = m.home;
        if (home.size() > 1){
            r = home.size() - 1;
            home = sortMatches(home, l, r);
            m.home = home;
        }

        QVector <Score> away = m.away;
        if (away.size() > 1){
            r = away.size() - 1;
            away = sortMatches(away, l, r);
            m.away = away;
        }

        matches->replace(i, m);
    }
}

QVector <DataAnalyser::Score> DataAnalyser::sortMatches(QVector<DataAnalyser::Score> scores, int left, int right)
{
    int i = left, j = right;
    Score tmp;
    int pivot = scores.at((left + right) / 2).time;

    /* partition */
    while (i <= j) {
        while (scores.at(i).time < pivot)
            i++;
        while (scores.at(j).time > pivot)
            j--;
        if (i <= j) {
            tmp = scores.at(i);
            scores.replace(i, scores.at(j));
            scores.replace(j, tmp);
            i++;
            j--;
        }
    };

    /* recursion */
    if (left < j)
        scores = sortMatches(scores, left, j);
    if (i < right)
        scores = sortMatches(scores, i, right);

    return scores;
}

void DataAnalyser::generateStories(QVector<Match> * matches, QString * output, QVector <QVector <int> > * diffs)
{
    QVector <int> diff;
    int time, homeScr, awayScr, k, l, home5, away5;
    for (int i = 0; i < matches->size(); i++){
        homeScr = 0, awayScr = 0, k = 0, l = 0;
        Match m = matches->at(i);
        diff.insert(0, m.id);
        diff.insert(1, 0);
        //output->append("\n\nMatch Id: " + QString::number(m.id) + "   " + QString::number(0) + ": " +
          //                  QString::number(0));
        for (int j = 1; j < 81; j++){
            time = j;
            home5 = 0, away5 = 0;
            for (k; k < m.home.size();){
                if (j != 80 && m.home.at(k).time <= time){
                    home5 = m.home.at(k).value;
                    homeScr += home5;
                    k++;
                }
                else if (j == 81){
                    home5 = m.home.at(k).value;
                    homeScr += home5;
                    k++;
                }
                else
                    break;
            }
            for (l; l < m.away.size();){
                if (j != 81 && m.away.at(l).time <= time){
                    away5 = m.away.at(l).value;
                    awayScr += away5;
                    l++;
                }
                else if (j == 81){
                    away5 = m.away.at(l).value;
                    awayScr += away5;
                    l++;
                }
                else
                    break;
            }
           /* int val, scr = home5 - away5;
            if (scr == 0)
                val = 0;
            else if (scr < 0){
                if (scr >= -7)
                    val = -1;
                else if (scr >= -14)
                    val = -2;
                else
                    val = -3;
            }
            else{
                if (scr <= 7)
                    val = 1;
                else if (scr <= 14)
                    val = 2;
                else
                    val = 3;
            }
            //diff.insert(j, val);*/
            diff.insert(j+1, homeScr - awayScr);
            //output->append("   " + QString::number(time) + ": " + QString::number(homeScr - awayScr));
        }
        diffs->insert(i, diff);
    }
}

void DataAnalyser::groupStories(QString *output, QVector<QVector <int> > *diffs)
{
    //QVector <Story> stories;

    //output->append("\n\n\nLines of best fit:\n");
    QChart *chart = new QChart();
    /*int i, n = 17, oldno;
    for (int iq = 0; iq < diffs->size(); iq++){
        Story story;
        story.idx = iq;
        //QLineSeries *series = new QLineSeries();
        double x[17], y[17];
        QVector <int> d = diffs->at(iq);
        oldno = 0;
        for (int j = 0; j < 17; j++){
            int num = d.at(j+1);
            x[j] = j*5;
            y[j] = num;

            if (num > story.max)
                story.max = num;
            else if (num < story.min)
                story.min = num;

            if (j == 16)
                story.last = num;

            if (num > oldno)
                story.pos++;
            else if (num < oldno)
                story.neg++;

            //series->append(j*5, d.at(j+1));
        }
        //chart->addSeries(series);

        double xsum=0, x2sum=0, ysum=0, xysum=0;                //variables for sums/sigma of xi,yi,xi^2,xiyi etc
        for (i=0;i<17;i++)
        {
            xsum = xsum+x[i];                        //calculate sigma(xi)
            ysum = ysum+y[i];                        //calculate sigma(yi)
            x2sum = x2sum+pow(x[i],2);                //calculate sigma(x^2i)
            xysum = xysum+x[i]*y[i];                    //calculate sigma(xi*yi)
        }
        story.slope = (n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);            //calculate slope
        story.yInt = (x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);            //calculate intercept

        qDebug() << "Match Id:" << d.at(0) << ": " << story.slope << "x + " << story.yInt << endl;
        output->append("\nMatch Id:" + QString::number(d.at(0)) + ": " + QString::number(story.slope) + "x + " +
                            QString::number(story.yInt));

        stories.append(story);*/

        /*if (story.slope > 0 && story.slope <= 0.1 && story.yInt > 0){
            QLineSeries *series2 = new QLineSeries();
            output->append("\nMatch Found: Id: " + QString::number(diffs->at(iq).at(0)));
            for (int k = 0; k < 17; k++){
                output->append("\n " + QString::number(diffs->at(iq).at(k+1)));
                series2->append(k*5, diffs->at(iq).at(k+1));
            }
            chart->addSeries(series2);
        }*/
    //}

    //output->append("\n\n\nStory Comparison\n\n");
    /*for (int it0 = 0 ; it0 < stories.size() - 1; it0++){
        Story story0 = stories.at(it0), story1;
        output->append("\nMatch Id:" + QString::number(diffs->at(story0.idx).at(0))/* + " " +
                            QString::number(story0.max) + " " + QString::number(story0.min) + " " +
                            QString::number(story0.last) + " " + QString::number(story0.neg) + " " +
                            QString::number(story0.pos) + " " + QString::number(story0.slope) + " " +
                            QString::number(story0.yInt));
        if (story0.last >= 0){
            for (int it1 = it0 + 1; it1 < stories.size(); it1++){
                story1 = stories.at(it1);
                if (story1.last >= 0){
                    if (story0.slope > story1.slope - 0.05 && story0.slope < story1.slope + 0.05)
                        if (story0.yInt > story1.yInt - 5 && story0.yInt < story1.yInt + 5)
                            if ((story0.max - story0.min) > (story1.max - story1.min - 7) &&
                                    (story0.max - story0.min) < (story1.max - story1.min + 7))
                                output->append("\nSimilar Story Found, Match Id:" +
                                           QString::number(diffs->at(story1.idx).at(0)) + " " +
                                           QString::number(story1.slope) + " " + QString::number(story1.yInt));
                }
            }
        }
    }*/

    /*for (int i = 0 ; i < stories.size(); i++){
        Story story = stories.at(i);
        if (story.last >= 0){
            if (story.max <= 7 && story.min >= -7){ //tight game, one score game
                QLineSeries *series2 = new QLineSeries();
                output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                               QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                for (int k = 0; k < 17; k++){
                    //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                    series2->append(k*5, diffs->at(i).at(k+1));
                }
                //chart->addSeries(series2);
            }
            else if (story.yInt > 4.5){
                if (story.last <= 7){
                    if (story.max > 14){ //comebacks/bell curve shape/3 scores ahead
                        QLineSeries *series2 = new QLineSeries();
                        output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                       QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                        for (int k = 0; k < 17; k++){
                            //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                            series2->append(k*5, diffs->at(i).at(k+1));
                        }
                        //chart->addSeries(series2);
                    }
                }
                else if ((story.slope > 0.15 && story.slope <= 0.35) || (story.slope < 0.15 && story.slope > 0.1 &&
                                                                        story.yInt > 5)){ //maintain early lead
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else if (story.slope <= 0.15){
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else{
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
             }
             else if (story.slope > .4){ //comprehensive wins
                QLineSeries *series2 = new QLineSeries();
                output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                               QString::number(story.slope) + " " + QString::number(story.yInt));
                for (int k = 0; k < 17; k++){
                     //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                     series2->append(k*5, diffs->at(i).at(k+1));
                }
                //chart->addSeries(series2);
             }
             else if (story.min < -7){
                if (story.yInt < -5){ //comeback to win
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else{ //early comeback to win
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
            }
            else if (story.slope > 0.3){
                if (story.min < 0){ //small early comeback, comfortable win
                   QLineSeries *series2 = new QLineSeries();
                   output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                  QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                   for (int k = 0; k < 17; k++){
                       //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                       series2->append(k*5, diffs->at(i).at(k+1));
                   }
                   //chart->addSeries(series2);
                }
                else{
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
            }
            else if (story.slope <= 0.1){
                if (story.yInt < 2.5 && story.last > 7){ //tight game, pull away late
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else if (story.last > 7){ //tight up and down game - comfortable win
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else{ //tight game - narrow win
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
            }
            else if (story.slope > 0.2){
                if (story.last > 21){
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else if (story.yInt > 2){
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        //output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    //chart->addSeries(series2);
                }
                else{
                    QLineSeries *series2 = new QLineSeries();
                    output->append("\n\nMatch Found: Id: " + QString::number(diffs->at(i).at(0))  + " " +
                                   QString::number(story.slope) + " " + QString::number(story.yInt) + "  ");
                    for (int k = 0; k < 17; k++){
                        output->append(" " + QString::number(diffs->at(i).at(k+1)));
                        series2->append(k*5, diffs->at(i).at(k+1));
                    }
                    chart->addSeries(series2);
                }
            }
        }
    }*/

    /*VectorXf b(16);
    for (int a = 0; a < 16; a++)
        b(a) = 5;

    for (int i = 0; i < diffs->size(); i++){
        MatrixXf A(16,2);
        QVector <int> d = diffs->at(i);
        for (int j = 1; j < 17; j++){
            A(j-1,0) = j*5;
            A(j-1,1) = d.at(j);
        }

     //MatrixXf A = MatrixXf::Random(3, 2);
     //VectorXf b = VectorXf::Random(3);

        /*for (int i = 0; i < 16; i++){
            qDebug() << "DEBUG0  " << QString::number(b(i)) << endl;
            for (int j = 0; j < 2; j++)
                qDebug() << "DEBUG1  " << QString::number(A(i,j)) << endl;
        }

        std::cout << "\n\nThe least-squares solution is:\n"
                 << A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b) << endl;
        //qDebug() << "DEBUG" << A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b) << endl;
    }*/

    //int idee = 500;

    //output->append("\n\n\nSlope Comparison\n");
    /*bool equal = true, first = false;
    int scoreDiff = 0, timeDiff = 0;
    for (int parse = 0; parse < 3; parse++){
        int numSimilar = 0;
        if (parse == 1){
            scoreDiff = 3;
            timeDiff = 1;
        }
        else if (parse == 2)
            scoreDiff = 5;

        output->append("\n\n\nParse Number: " + QString::number(parse + 1) + "\nScore Difference Allowance: " +
                            QString::number(scoreDiff) + "\nTime Difference Allowance: " +
                            QString::number(timeDiff * 5) + "\n\n" );
        for (int i = 0; i < diffs->size() /*&& !first; i++){
            QVector <int> match0 =  diffs->at(i);
            for (int j = i + 1; j < diffs->size(); j++){
                equal = true, first = false;
                QVector <int> match1 =  diffs->at(j);
                for (int k = 2, m = 2; equal && k < 18 && m < 18; k++, m++){
                    int num0 = match0.at(k);
                    if (parse > 0 && !first){
                        for (int l = 0; l < timeDiff && !first; l++){
                            int num1 = match1.at(m + l);
                            //if ((num0 == num1) || (((num0 > 0 && num1 > 0) || (num0 > 0 && num1 > 0)) &&
                            if (num0 >= (num1 - scoreDiff) && num0 <= (num1 + scoreDiff)){
                                first = true;
                                m += l;
                            }
                        }
                        if (!first){
                            first = true;
                            equal = false;
                        }
                    }
                    else{
                        int num1 = match1.at(m);
                        if (!(num0 >= (num1 - scoreDiff) && num0 <= (num1 + scoreDiff)))
                            equal = false, first = true;
                    }
                }

                if (equal){
                    numSimilar++;
                    output->append("\n\nSimilar Matches Found: Match 1 Id: " + QString::number(match0.at(0))  +
                                   "  Match 2 Id: " + QString::number(match1.at(0)) + "\n");
                    //idee = match0.at(0);
                    QLineSeries *series0 = new QLineSeries();
                    QLineSeries *series1 = new QLineSeries();

                    for (int k = 0; k < 17; k++){
                        output->append(" " + QString::number(match0.at(k+1)));
                        if (parse < 2 /*match0.at(0) == 41 ){
                            series0->append(k*5, match0.at(k+1));
                        }
                    }
                    output->append("\n");
                    for (int k = 0; k < 17; k++){
                        output->append(" " + QString::number(match1.at(k+1)));
                        if (parse < 2 /*match0.at(0) == 41 ){
                            series1->append(k*5, match1.at(k+1));
                        }
                    }
                    chart->addSeries(series0);
                    chart->addSeries(series1);
                    //first = true;
                }
            }
        }
        output->append("\n\nNumber of pairs of similar matches found for parse:  " + QString::number(numSimilar));
    }*/

    output->append("Game Story Comparison using Sum of Distances formula\n\n");
    int numFound = 0;
    for (int i = 0; i < diffs->size(); i++){
        if (diffs->at(i).at(81) >= 0){
            QVector <int> match0 =  diffs->at(i);
            for (int j = i + 1; j < diffs->size(); j++){
                int distance = 0;
                if (diffs->at(j).at(81) >= 0){
                    QVector <int> match1 =  diffs->at(j);
                    for (int k = 1; k < 81; k++){
                        int num1 = match0.at(k);
                        int num2 = match1.at(k);
                        if (num1 > num2)
                            distance += (num1 - num2);
                        else if (num2 > num1)
                            distance += (num2 - num1);
                    }
                    //qDebug() << distance << endl;
                    if (distance <= 125){
                        numFound++;
                        QString m1 = getMatchInfo(match0.at(0));
                        QString m2 = getMatchInfo(match1.at(0));
                        //if (numFound == 10){
                            output->append("\n\nSimilar Matches Found: \nMatch 1: " + m1 +
                                           "\nMatch 2: " + m2 + "\nDistance: " +
                                           QString::number(distance) + "\nMatch 1 Story: ");
                            QLineSeries *series0 = new QLineSeries();
                            QLineSeries *series1 = new QLineSeries();

                            for (int k = 0; k < 81; k++){
                                output->append(" " + QString::number(match0.at(k+1)));
                                if (match0.at(0) == 141 && match1.at(0) == 253)
                                    series0->append(k, match0.at(k+1));
                            }
                            output->append("\nMatch 2 Story: ");
                            for (int k = 0; k < 81; k++){
                                output->append(" " + QString::number(match1.at(k+1)));
                                if (match0.at(0) == 141 && match1.at(0) == 253)
                                    series1->append(k, match1.at(k+1));
                            }
                            if (match0.at(0) == 141 && match1.at(0) == 253){
                                series0->setName(m1);
                                series1->setName(m2);
                                chart->addSeries(series0);
                                chart->addSeries(series1);
                            }
                    }
                }
            }
        }
    }
    output->append("\n\n\nNumber of similar stories found using sum of distances method (within 125 points): " +
                        QString::number(numFound));

            //bool equal = true;
            //for (int k = 18; k < 19 && equal; k++){
                /*if (diffs->at(i).at(17) >= diffs->at(j).at(17) - 0.5 && diffs->at(i).at(17) < diffs->at(j).at(17) + 0.5){
                    //if (diffs->at(i).at(18) >= diffs->at(j).at(18) - 1 && diffs->at(i).at(18) < diffs->at(j).at(18) + 1){
                        output->append("\nMatch Found: Id: " + QString::number(diffs->at(i).at(0)) + " and Id: " +
                                QString::number(diffs->at(j).at(0)));
                        QLineSeries *series = new QLineSeries();
                        QLineSeries *series2 = new QLineSeries();
                        for (int k = 0; k < 17; k++){
                            output->append("\n " + QString::number(diffs->at(i).at(k+1)) + " " +
                                                            QString::number(diffs->at(j).at(k+1)));
                            series->append(k*5, diffs->at(j).at(k+1));
                            series2->append(k*5, diffs->at(i).at(k+1));
                        }
                        chart->addSeries(series);
                        chart->addSeries(series2);
                        //found = true;
                    //}
                }*/
            //}
            /*if (equal){
                output->append("\nMatch Found: Id: " + QString::number(diffs->at(i).at(0)) + " and Id: " +
                        QString::number(diffs->at(j).at(0)));
                /*for (int k = 0; k < 17 && equal; k++)
                    output->append("\n " + QString::number(diffs->at(i).at(k)) + " " +
                                    QString::number(diffs->at(j).at(k)));*/
           // }
        //}


    //}

    chart->createDefaultAxes();
    chart->setTitle("Similar Game Stories Example");

    chart2 = new QChartView(chart);
}

void DataAnalyser::tryConditions(QSqlQuery qry, QVector<Booking> *bookings)
{
    Booking b;
    while (qry.next()){
        int id = qry.value(0).toInt();
        bool found = false;
        for (int i = 0; i < bookings->size() && !found; i++){
            if (id == bookings->at(i).id){
                b = bookings->at(i);
                bookings->removeAt(i);
                found = true;
            }
        }
        if (!found){
            b.id = qry.value(0).toInt();
            b.time = qry.value(2).toInt();
            b.team = qry.value(3).toInt();
            b.colour = qry.value(4).toString();
            b.scored = 0;
            b.conceded = 0;
        }
        int tryTime = qry.value(7).toInt();
        if ((b.colour == "Yellow" && tryTime >= b.time && tryTime < b.time + 10) || (b.colour == "Red" &&
                tryTime >= b.time)){
            int score;
            if (qry.value(9).toInt() == 0)
                score = 5;
            else
                score = 7;
            if (qry.value(8).toInt() == b.team)
                b.scored += score;
            else
                b.conceded += score;
        }
        bookings->push_front(b);
    }
    /*for (int i = 0; i < bookings->size(); i++){
        b = bookings->at(i);
        output += "\nBooking Id: " + QString::number(b.id) + ", time: " + QString::number(b.time) + ", team: " +
                    QString::number(b.team) + ", scored: " + QString::number(b.scored) + ", conceded: " +
                    QString::number(b.conceded);
    }*/
}

void DataAnalyser::penDropConditions(QSqlQuery qry, int eve, QVector <Booking> * bookings)
{
    QString event;
    if (eve == 0)
        event = "Penalties";
    else
        event = "Drop Goals";
    Booking b;
    while (qry.next()){

            int id = qry.value(0).toInt();
            bool found = false;
            for (int i = 0; i < bookings->size() && !found; i++){
                if (id == bookings->at(i).id){
                    b = bookings->at(i);
                    bookings->removeAt(i);
                    found = true;
                }
            }
            if (!found){
                b.id = qry.value(0).toInt();
                b.time = qry.value(2).toInt();
                b.team = qry.value(3).toInt();
                b.colour = qry.value(4).toString();
                b.scored = 0;
                b.conceded = 0;
            }
            int scoreTime = qry.value(7).toInt();
            if ((b.colour == "Yellow" && scoreTime >= b.time && scoreTime < b.time + 10) || (b.colour == "Red" &&
                    scoreTime >= b.time)){
                if (qry.value(8).toInt() == b.team)
                    b.scored += 3;
                else
                    b.conceded += 3;
            }
            bookings->push_front(b);

    }
}

void DataAnalyser::setPieChart(double perc)
{
    QPieSeries *series2 = new QPieSeries();
    series2->append("Home - " + QString::number(perc) + "%", perc);
    series2->append("Away - " + QString::number(100 - perc) + "%", 100 - perc);

    QChart *chart = new QChart();
    chart->addSeries(series2);
    chart->setTitle("Home/Away Team Comparison");

    chart2 = new QChartView(chart);
}

QString DataAnalyser::getMatchInfo(int matId)
{
    QString matchInfo = "";
    QueryDB qdb;
    QueryBuilder qb;
    QString query = qb.getMatchQuery(matId);
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    qry.next();
    matchInfo += "ID: " + QString::number(qry.value(0).toInt()) + ", " + qry.value(2).toString() + " vs " +
                    qry.value(3).toString() + ", " + qry.value(4).toString() + ", " +
                    (qry.value(1).toString()).remove(10, 12);
    return matchInfo;
}

void DataAnalyser::setChart1(QString str)
{
    QBarSet *set0 = new QBarSet(str);
    QStringList categories;
    for (int i = 0; i < arr.size() - 1; i++){
        *set0 << arr.at(i);
        QString str1;
        if (i == 8)
            str1.append(QString::number(i) + "0+");
        else
            str1.append(QString::number(i) + "0-" + QString::number(i+1) + "0");
        categories << str1;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Match Time Analysis");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chart1 = new QChartView(chart);
}

void DataAnalyser::setScoresChart()
{
    QBarSet *set0 = new QBarSet("All Scores");
    QStringList categories;
    for (int i = 0; i < scoresArr.size() - 1; i++){
        *set0 << scoresArr.at(i);
        QString str1;
        if (i == 8)
            str1.append(QString::number(i) + "0+");
        else
            str1.append(QString::number(i) + "0-" + QString::number(i+1) + "0");
        categories << str1;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Match Time Analysis");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chart2 = new QChartView(chart);
}

void DataAnalyser::setPieChartMatches()
{
    QPieSeries *series2 = new QPieSeries();
    series2->append("Home - " + QString::number(homeWinPerc) + "%", homeWinPerc);
    series2->append("Away - " + QString::number(100 - homeWinPerc) + "%", 100 - homeWinPerc);

    QChart *chart = new QChart();
    chart->addSeries(series2);
    chart->setTitle("Home/Away Wins");

    chart2 = new QChartView(chart);
}

void DataAnalyser::analyseStories(int totMatches, QVector<QString> queries, QString *result)
{
    int oldNo, newNo;
    //result->append("Game Stories Generation\n\nTotal Matches in Query: " + QString::number(totMatches) + "\n\n\n");
    QVector <Score> home, away;
    bool first;
    QVector <Match> matchesVec;
    QVector <QVector <int> > diffs;

    for (int i = 0; i < queries.size(); i++){
        QString query = queries.at(i);
        QueryDB qdb;
        QSqlQuery qry;
        qdb.setQuery(query);
        qry = qdb.executeQuery();
        first = true;

        while (qry.next()) {
                newNo = qry.value(0).toInt();
                if (newNo != oldNo){
                    if (!first){
                        if (matchesVec.size() == 0){
                            Match m;
                            m.away = away;
                            m.home = home;
                            m.id = oldNo;
                            matchesVec.push_back(m);
                        }
                        else{
                            bool found = false;
                            for (int j = 0; j < matchesVec.size() && !found; j++){

                                if (oldNo == matchesVec.at(j).id){
                                    found = true;
                                    for (int x = 0; x < matchesVec.at(j).away.size(); x++)
                                        away.push_back(matchesVec.at(j).away[x]);
                                    for (int x = 0; x < matchesVec.at(j).home.size(); x++)
                                        home.push_back(matchesVec.at(j).home[x]);
                                    Match mat = matchesVec.at(j);
                                    matchesVec.removeAt(j);
                                    mat.away = away;
                                    mat.home = home;
                                    matchesVec.push_back(mat);
                                }
                            }
                            if (!found){
                                Match m;
                                m.away = away;
                                m.home = home;
                                m.id = oldNo;
                                matchesVec.push_back(m);
                            }
                        }
                    }
                    oldNo = newNo;
                    home.clear();
                    away.clear();
                }
                Score s;
                s.time = qry.value(12).toInt();
                int team = qry.value(13).toInt();

                if (i == 0){
                    int conv = qry.value(14).toInt();

                    if (conv == 0)
                        s.value = 5;
                    else
                        s.value = 7;
                }
                else
                    s.value = 3;

                if (team == 0)//home
                    home.push_back(s);
                else
                    away.push_back(s);

                first = false;
        }
        bool found = false;
        for (int j = 0; j < matchesVec.size() && !found; j++){

            if (newNo == matchesVec.at(j).id){
                found = true;
                for (int x = 0; x < matchesVec.at(j).away.size(); x++)
                    away.push_back(matchesVec.at(j).away[x]);
                for (int x = 0; x < matchesVec.at(j).home.size(); x++)
                    home.push_back(matchesVec.at(j).home[x]);
                Match mat = matchesVec.at(j);
                matchesVec.removeAt(j);
                mat.away = away;
                mat.home = home;
                matchesVec.push_back(mat);
            }
        }
        if (!found){
            Match m;
            m.away = away;
            m.home = home;
            m.id = newNo;
            matchesVec.push_back(m);
        }
        qry.clear();
    }

    organiseMatches(&matchesVec);
    generateStories(&matchesVec, result, &diffs);

    qDebug() << result << endl;
    groupStories(result, &diffs);
}

void DataAnalyser::analyseConditions(int num, QVector<QString> queries, QString * result)
{
    QVector <Booking> bookings;
    for (int i = 0; i < queries.size(); i++){
        QString query = queries.at(i);
        QueryDB qdb;
        QSqlQuery qry;
        qdb.setQuery(query);
        qry = qdb.executeQuery();

        switch (i) {
        case 0: analyseBookings(qry, num, result);
            break;
        case 1: tryConditions(qry, &bookings);
            break;
        case 2: penDropConditions(qry, 0, &bookings);
            break;
        case 3: penDropConditions(qry, 1, &bookings);
            break;
        default:
            break;
        }
        qry.clear();
    }

    double yAvgCon = 0.0, yAvgScor = 0.0, yAvgDiff = 0.0, rAvgCon = 0.0, rAvgScor = 0.0, rAvgDiff = 0.0;
    int ySize = 0, rSize = 0;
    for (int i = 0; i < bookings.size(); i++){
        Booking b;
        b = bookings.at(i);
        int scored = b.scored;
        int conceded = b.conceded;
        if (b.colour == "Yellow"){
            ySize++;
            yAvgCon += conceded;
            yAvgScor += scored;
            yAvgDiff += scored - conceded;
        }
        else{
            rSize++;
            rAvgCon += conceded;
            rAvgScor += scored;
            rAvgDiff += scored - conceded;
        }
       /* qDebug() << scored << " " << avgScor << " " << conceded << " " << avgCon << " " << size << endl;
        result->append("\nBooking Id: " + QString::number(b.id) + ", time: " + QString::number(b.time) + ", team: " +
                    QString::number(b.team) + ", scored: " + QString::number(b.scored) + ", conceded: " +
                    QString::number(b.conceded));*/
    }
    yAvgCon = floor(((double)yAvgCon / ySize)/scale + 0.5) * scale;
    yAvgScor = floor(((double)yAvgScor / ySize)/scale + 0.5) * scale;
    yAvgDiff = floor(((double)yAvgDiff / ySize)/scale + 0.5) * scale;
    rAvgCon = floor(((double)rAvgCon / rSize)/scale + 0.5) * scale;
    rAvgScor = floor(((double)rAvgScor / rSize)/scale + 0.5) * scale;
    rAvgDiff = floor(((double)rAvgDiff / rSize)/scale + 0.5) * scale;
    result->append("\n\n\nYellow Card Score Analysis:\n\nAverage score conceded by team with sin binned player:\t" +
                QString::number(yAvgCon) + "\n\nAverage score scored by team with sin binned player:\t" +
                QString::number(yAvgScor) + "\n\nAverage change in score during a sin bin:\t\t" +
                QString::number(yAvgDiff)+
                "\n\n\nRed Card Score Analysis:\n\nAverage score conceded by team with player sent off:\t" +
                QString::number(rAvgCon) +
                "\n\nAverage score scored by team with player sent off:\t" + QString::number(rAvgScor) +
                "\n\nAverage change in score when player sent off:\t\t" + QString::number(rAvgDiff));
}

QChartView *DataAnalyser::getChart()
{
    return chart2;
}

QChartView *DataAnalyser::getChart1()
{
    return chart1;
}

void DataAnalyser::resetArray(int i)
{
    for (int i = 0; i < arr.length(); i++){
        arr.replace(i, 0);
        if (i == 1)
            scoresArr.replace(i, 0);
    }
}

/*QString DataAnalyser::compare(QString str1, QString str2)
{
    QString result = "Comparison of Data Sets\n\n" + str1 + "\n\n\n" + str2;
    return result;
}*/

