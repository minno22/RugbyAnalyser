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
#include <QtCharts/QPercentBarSeries>

DataAnalyser::DataAnalyser()
{
    for (int i = 0; i < 10; i++){
        homeScoresArr.append(0);
        awayScoresArr.append(0);
    }
    firstComp = true;
}

void DataAnalyser::analyse(QSqlQuery qry, int idx, int num, QString * result, QString comp) //selects which analysis should be done
{
    if(firstComp)
        firstCompName = comp;
    else
        secondCompName = comp;
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

void DataAnalyser::compare(int type, QString * output)
{
    QString result;
    switch (type){

    case 0:{
        compareMatches(&result);
        result.append("\n");
        compareTries(&result);
        result.append("\n");
        comparePen(&result);
        result.append("\n");
        compareDrop(&result);
        result.append("\n");
        compareBookings(&result);
    } break;
    case 1:{compareTries(&result);} break;
    case 2:{comparePen(&result);} break;
    case 3:{compareDrop(&result);} break;
    case 4:{compareBookings(&result);} break;
    case 5:{
        compareTries(&result);
        result.append("\n");
        comparePen(&result);
        result.append("\n");
        compareDrop(&result);
    } break;
    }
    output->prepend(result);
}

void DataAnalyser::analyseTries(QSqlQuery qry, int totMatches, QString * result)
{
    //qDebug() << "DEBUG!!!!!!" << endl;
    tryDataCruncher(qry, totMatches);
    TryStats ts;
    if (firstComp){
        ts = firstCompTry;
        setPieChart(ts.homePerc);
        setChart1("Tries", ts.arr);
    }
    else{
        ts = secondCompTry;
        setChart2(firstCompTry.arr, secondCompTry.arr);
        setPieChart2(firstCompTry.avg, secondCompTry.avg);
    }

    result->append("Analysis of Tries Scored\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches) +
                ".\nTotal number of matches satisfying search criteria:\t" + QString::number(ts.no) +
                ".\nTotal number of tries scored:\t\t\t\t" + QString::number(ts.num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = ts.arr.at(i-1);
        double y = floor(((double)n/ts.num * 100) / scale + 0.5) * scale;
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

    result->append("Average number of tries per match:\t" + QString::number(ts.avg) + ".\nConverted tries:\t\t\t" +
                QString::number(ts.converted) + ", " + QString::number(ts.conPercent) + "% of total tries.\nPenalty tries:\t\t\t" +
                QString::number(ts.penalty) + ", " + QString::number(ts.penPerc) + "% of total tries.\nHome team tries:\t\t\t" +
                QString::number(ts.homeTries) + ", " + QString::number(ts.homePerc) + "% of total tries.\nAverage home tries:\t\t"
                + QString::number(ts.avgHome) + ".\nAway team tries:\t\t\t" + QString::number(ts.num - ts.homeTries) + ", " +
                QString::number(100.00 - ts.homePerc) + "% of total tries.\nAverage away tries:\t\t\t" + QString::number(ts.avgAway)
                   + ".");
}

void DataAnalyser::tryDataCruncher(QSqlQuery qry, int totMatches)
{
    TryStats ts;
    for (int i = 0; i < 10; i++){
        ts.arr.append(0);
    }
    ts.num = 0, ts.homeTries = 0, ts.converted = 0, ts.penalty = 0, ts.no = 0;
    int noMatches = 0, matchIdOld, matchIdNew, oldNo, newNo;
    while (qry.next()) {
        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = ts.arr.at(b) + 1;
        ts.arr.replace(b, x);
        if (firstComp){
            int y = homeScoresArr.at(b) + 1;
            homeScoresArr.replace(b, y);
        }
        else{
            int y = awayScoresArr.at(b) + 1;
            awayScoresArr.replace(b, y);
        }
        if (b > 8){
            int x = ts.arr.at(8) + 1;
            ts.arr.replace(8, x);
            if (firstComp){
                int y = homeScoresArr.at(8) + 1;
                homeScoresArr.replace(8, y);
            }
            else{
                int y = awayScoresArr.at(8) + 1;
                awayScoresArr.replace(8, y);
            }
        }
        ts.num++;

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
            ts.no++;
            oldNo = newNo;
        }

        if (qry.value(13).toInt() == 0)
            ts.homeTries++;
        if (qry.value(14).toInt() == 1)
            ts.converted++;
        if (qry.value(15).toInt() == 1)
            ts.penalty++;
    }

    ts.avg = floor(((double)ts.num/totMatches)/scale + 0.5) * scale;
    ts.avgHome = floor(((double)ts.homeTries/(totMatches))/scale + 0.5) * scale;
    ts.avgAway = floor(((double)(ts.num - ts.homeTries)/(totMatches))/scale + 0.5) * scale;
    ts.conPercent = floor(((double)ts.converted/ts.num * 100) / scale + 0.5) * scale;
    ts.penPerc = floor(((double)ts.penalty/ts.num * 100) / scale + 0.5) * scale;
    ts.homePerc = floor(((double)ts.homeTries/ts.num * 100) / scale + 0.5) * scale;

    if (firstComp)
        firstCompTry = ts;
    else
        secondCompTry = ts;
}

void DataAnalyser::analyseMatches(QSqlQuery qry, QString * result)
{
    matchDataCruncher(qry);
    MatchStats ms;
    if (firstComp)
        ms = firstCompMatch;
    else
        ms = secondCompMatch;

    //qDebug() << totHome << " " << totAway << " " << numTeams << endl;

    result->append("Match Analysis\n\nNumber of Matches:\t" + QString::number(ms.num) + ".\nNumber of Competitions:\t" +
                QString::number(ms.comps.size()) + ".\n\nNumber of home wins:\t" + QString::number(ms.homeWins) + ", " +
                QString::number(ms.homePerc) + "% of total matches.\nNumber of away wins:\t" + QString::number(ms.awayWins)
                + ", " + QString::number(ms.awayPerc) + "% of total matches.\nNumber of draws:\t\t" +
                QString::number(ms.num - (ms.homeWins + ms.awayWins)) + ", " + QString::number(100.00 - (ms.homePerc + ms.awayPerc))
                + "% of total matches.\n\nAverage combined score per match:\t" + QString::number(ms.avg) + " points.\nAverage home score:\t\t" +
                QString::number(ms.avgHome) + " points.\nAverage away score:\t\t" + QString::number(ms.avgAway) +
                   " points.\nMaximum score:\t\t\t" + QString::number(ms.maxScore) + " points.");
}

void DataAnalyser::matchDataCruncher(QSqlQuery qry)
{
    MatchStats ms;
    ms.num = 0, ms.homeWins = 0, ms.awayWins = 0, ms.maxScore = 0;
    int totHome = 0, totAway = 0;
    while (qry.next()) {
         ms.num++;
         QString comp = qry.value(4).toString();
         bool found = false;
         for (int i = 0; i < ms.comps.size() && !found; i++)
             if (comp == ms.comps.at(i))
                 found = true;
         if (!found)
             ms.comps.push_back(comp);

         int home = qry.value(5).toInt();
         int away = qry.value(6).toInt();
         if (home > away)
             ms.homeWins++;
         else if (away > home)
             ms.awayWins++;
         totHome += home;
         totAway += away;

         if (home > ms.maxScore)
             ms.maxScore = home;
         if (away > ms.maxScore)
             ms.maxScore = away;
    }
    ms.homePerc = floor(((double)ms.homeWins/ms.num * 100) / scale + 0.5) * scale;
    ms.awayPerc = floor(((double)ms.awayWins/ms.num * 100) / scale + 0.5) * scale;
    ms.avg = floor(((double)(totHome + totAway)/ms.num) / scale + 0.5) * scale;
    ms.avgHome = floor(((double)totHome/ms.num) / scale + 0.5) * scale;
    ms.avgAway = floor(((double)totAway/ms.num) / scale + 0.5) * scale;

    if (firstComp)
        firstCompMatch = ms;
    else
        secondCompMatch = ms;
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

    PenDropDataCruncher(qry, totMatches, eve);

    PenDropStats pds;
    if (firstComp){
        if (eve == 0)
            pds = firstCompPen;
        else
            pds = firstCompDrop;
        setPieChart(pds.homePerc);
        setChart1(big, pds.arr);
    }
    else{
        if (eve == 0)
            pds = secondCompPen;
        else
            pds = secondCompDrop;
        setPieChart2(firstCompPen.avg, secondCompPen.avg);
        setChart2(firstCompPen.arr, secondCompPen.arr);
    }

    result->append("Analysis of " + big + " Scored\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches)
            + ".\nTotal number of matches satisfying search criteria:\t" + QString::number(pds.no) +
                ".\nTotal number of " + small + " scored:\t\t\t" + QString::number(pds.num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = pds.arr.at(i-1);
        double y = floor(((double)n/pds.num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result->append(big + " scored between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes:\t" + QString::number(n) + ", " + QString::number(y) + "% of total " + small + ".\n");
        }
        else{
            result->append(big + " scored after 80 minutes:\t\t\t" + QString::number(n) + ", " + QString::number(y)
                        + "% of total " + small + ".\n\n\n");
        }
    }

    result->append("Average number of " + small + " per match:\t" + QString::number(pds.avg) + ".\nHome team " + small + ":\t\t\t" +
                QString::number(pds.homeScores) + ", " + QString::number(pds.homePerc) +
                "% of total " + small + ".\nAverage home " + small + ":\t\t\t" + QString::number(pds.avgHome) +
                ".\nAway team " + small + ":\t\t\t" + QString::number(pds.num - pds.homeScores) + ", " +
                QString::number(100.00 - pds.homePerc) + "% of total " + small + ".\nAverage away " + small + ":\t\t\t" +
                   QString::number(pds.avgAway) + ".");
}

void DataAnalyser::PenDropDataCruncher(QSqlQuery qry, int totMatches, int eve)
{
    PenDropStats pds;
    for (int i = 0; i < 10; i++){
        pds.arr.append(0);
    }

    pds.num = 0, pds.homeScores = 0, pds.no = 0;
    int noMatches = 0, matchIdOld, matchIdNew, oldNo, newNo;
    while (qry.next()) {
        pds.num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = pds.arr.at(b) + 1;
        pds.arr.replace(b, x);
        if (firstComp){
            int y = homeScoresArr.at(b) + 1;
            homeScoresArr.replace(b, y);
        }
        else{
            int y = awayScoresArr.at(b) + 1;
            awayScoresArr.replace(b, y);
        }
        if (b > 8){
            int x = pds.arr.at(8) + 1;
            pds.arr.replace(8, x);
            if (firstComp){
                int y = homeScoresArr.at(8) + 1;
                homeScoresArr.replace(8, y);
            }
            else{
                int y = awayScoresArr.at(8) + 1;
                awayScoresArr.replace(8, y);
            }
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
            pds.homeScores++;

        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            pds.no++;
            oldNo = newNo;
        }
    }

    pds.avg = floor(((double)pds.num/totMatches)/scale + 0.5) * scale;
    pds.avgHome = floor(((double)pds.homeScores/(totMatches))/scale + 0.5) * scale;
    pds.avgAway = floor(((double)(pds.num - pds.homeScores)/(totMatches))/scale + 0.5) * scale;
    pds.homePerc = floor(((double)pds.homeScores/pds.num * 100) / scale + 0.5) * scale;

    if (firstComp){
        if (eve == 0)
            firstCompPen = pds;
        else
            firstCompDrop = pds;
    }
    else{
        if (eve == 0)
            secondCompPen = pds;
        else
            secondCompDrop = pds;
    }
}

void DataAnalyser::analyseBookings(QSqlQuery qry, int totMatches, QString * result)
{
    bookingsDataCruncher(qry, totMatches);

    BookingStats bs;
    if (firstComp){
        bs = firstCompBookings;
        setPieChart(bs.homePerc);
        setChart1("Bookings", bs.arr);
    }
    else{
        bs = secondCompBookings;
        setPieChart2(firstCompBookings.avg, secondCompBookings.avg);
        setChart2(firstCompBookings.arr, secondCompBookings.arr);
    }

    result->append("Analysis of Bookings\n\nTotal number of matches in competition/s:\t\t" + QString::number(totMatches) +
                ".\nTotal number of matches satisfying search conditions:\t" + QString::number(bs.no) +
                ".\nTotal number of bookings:\t\t\t\t" + QString::number(bs.num) + ".\n\n");
    for (int i = 1; i < 10; i++){
        int n = bs.arr.at(i-1);
        double y = floor(((double)n/bs.num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result->append("Bookings between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes:\t" + QString::number(n) + ", " + QString::number(y) + "% of total bookings.\n");
        }
        else{
            result->append("Bookings after 80 minutes:\t\t\t" + QString::number(n) + ", " + QString::number(y)
                        + "% of total penalties.\n\n");
        }
    }

    result->append("Average number of bookings per match:\t" + QString::number(bs.avg) + ".\nHome team bookings:\t\t\t" +
                QString::number(bs.homeBookings) + ", " + QString::number(bs.homePerc) +
                "% of total bookings.\nAverage home bookings:\t\t\t" + QString::number(bs.avgHome) +
                ".\nAway team bookings:\t\t\t" + QString::number(bs.num - bs.homeBookings) + ", " +
                QString::number(100.00 - bs.homePerc) + "% of total bookings.\nAverage away bookings:\t\t\t" +
                QString::number(bs.avgAway) + ".\n\nTotal yellow cards:\t\t" + QString::number(bs.yellows) + ", " +
                QString::number(bs.yelPer) + " % of total bookings.\nHome yellows:\t\t" + QString::number(bs.homeYellows) +
                ".\nAway yellows:\t\t" + QString::number(bs.yellows - bs.homeYellows) + ".\n\nTotal red cards:\t\t" +
                QString::number(bs.reds) + ", " + QString::number(bs.redPer) + " % of total bookings.\nHome reds:\t\t" +
                   QString::number(bs.homeReds) + ".\nAway reds:\t\t" + QString::number(bs.reds - bs.homeReds) + ".");
}

void DataAnalyser::bookingsDataCruncher(QSqlQuery qry, int totMatches)
{
    BookingStats bs;
    for (int i = 0; i < 10; i++){
        bs.arr.append(0);
    }

    bs.num = 0, bs.homeBookings = 0, bs.yellows = 0, bs.homeYellows = 0, bs.homeReds = 0, bs.no = 0;
    int noMatches = 0, matchIdOld, matchIdNew, oldNo, newNo;
    while (qry.next()) {
        bs.num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = bs.arr.at(b) + 1;
        bs.arr.replace(b, x);
        if (b > 8){
            int x = bs.arr.at(8) + 1;
            bs.arr.replace(8, x);
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
            bs.homeBookings++;

        if (qry.value(14).toString() == "Yellow"){
            bs.yellows++;
            if (homeAway == 0)
                bs.homeYellows++;
        }
        else
            if (homeAway == 0)
                bs.homeReds++;

        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            bs.no++;
            oldNo = newNo;
        }
    }

    bs.avg = floor(((double)bs.num/totMatches)/scale + 0.5) * scale;
    bs.avgHome = floor(((double)bs.homeBookings/(totMatches))/scale + 0.5) * scale;
    bs.avgAway = floor(((double)(bs.num - bs.homeBookings)/(totMatches))/scale + 0.5) * scale;
    bs.homePerc = floor(((double)bs.homeBookings/bs.num * 100) / scale + 0.5) * scale;

    bs.yelPer = floor(((double)bs.yellows/bs.num * 100) / scale + 0.5) * scale;
    bs.redPer = floor(((double)(bs.num - bs.yellows)/bs.num * 100) / scale + 0.5) * scale;

    bs.reds = bs.num - bs.yellows;

    if (firstComp)
        firstCompBookings = bs;
    else
        secondCompBookings = bs;
}

void DataAnalyser::compareMatches(QString *result)
{
    result->append("General Match Comparison\n\n");
    if (firstCompMatch.homePerc > secondCompMatch.homePerc)
        result->append(firstCompName + " home win percentage is " +
                QString::number(firstCompMatch.homePerc - secondCompMatch.homePerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompMatch.homePerc < secondCompMatch.homePerc)
        result->append(firstCompName + " home win percentage is " +
                QString::number(secondCompMatch.homePerc - firstCompMatch.homePerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical home win percentages of " +
                QString::number(firstCompMatch.homePerc) + "%.\n");

    if (firstCompMatch.awayPerc > secondCompMatch.awayPerc)
        result->append(firstCompName + " away win percentage is " +
                QString::number(firstCompMatch.awayPerc - secondCompMatch.awayPerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompMatch.awayPerc < secondCompMatch.awayPerc)
        result->append(firstCompName + " away win percentage is " +
                QString::number(secondCompMatch.awayPerc - firstCompMatch.awayPerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical away win percentages of " +
                QString::number(firstCompMatch.awayPerc) + "%.\n");

    if (firstCompMatch.avg > secondCompMatch.avg)
        result->append(firstCompName + " average match score is " +
                QString::number(firstCompMatch.avg - secondCompMatch.avg) + " points more than " + secondCompName + ".\n");
    else if (firstCompMatch.avg < secondCompMatch.avg)
        result->append(firstCompName + " average match score is " +
                QString::number(secondCompMatch.avg - firstCompMatch.avg) + " points less than " + secondCompName + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average match scores of " +
                QString::number(firstCompMatch.avg) + "points.\n");

    if (firstCompMatch.avgHome > secondCompMatch.avgHome)
        result->append(firstCompName + " average home score is " +
                QString::number(firstCompMatch.avgHome - secondCompMatch.avgHome) + " points more than " + secondCompName + ".\n");
    else if (firstCompMatch.avgHome < secondCompMatch.avgHome)
        result->append(firstCompName + " average home score is " +
                QString::number(secondCompMatch.avgHome - firstCompMatch.avgHome) + " points less than " + secondCompName + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home scores of " +
                QString::number(firstCompMatch.avgHome) + "points.\n");

    if (firstCompMatch.avgAway > secondCompMatch.avgAway)
        result->append(firstCompName + " average away score is " +
                QString::number(firstCompMatch.avgAway - secondCompMatch.avgAway) + " points more than " + secondCompName + ".\n\n");
    else if (firstCompMatch.avgAway < secondCompMatch.avgAway)
        result->append(firstCompName + " average away score is " +
                QString::number(secondCompMatch.avgAway - firstCompMatch.avgAway) + " points less than " + secondCompName + ".\n\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away scores of " +
                       QString::number(firstCompMatch.avgAway) + "points.\n\n");
}

void DataAnalyser::compareTries(QString *result)
{
    result->append("Try Comparison\n\n");

    if (firstCompTry.avg > secondCompTry.avg)
        result->append(firstCompName + " average tries per match is " +
                QString::number(firstCompTry.avg - secondCompTry.avg) + " tries more than " + secondCompName
                + ".\n");
    else if (firstCompTry.avg < secondCompTry.avg)
        result->append(firstCompName + " average tries per match is " +
                QString::number(secondCompTry.avg - firstCompTry.avg) + " tries less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average tries per match of " +
                QString::number(firstCompTry.avg) + "tries.\n");

    if (firstCompTry.avgHome > secondCompTry.avgHome)
        result->append(firstCompName + " average home tries per match is " +
                QString::number(firstCompTry.avgHome - secondCompTry.avgHome) + " tries more than " + secondCompName
                + ".\n");
    else if (firstCompTry.avgHome < secondCompTry.avgHome)
        result->append(firstCompName + " average home tries per match is " +
                QString::number(secondCompTry.avgHome - firstCompTry.avgHome) + " tries less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home tries per match of " +
                QString::number(firstCompTry.avgHome) + "tries.\n");

    if (firstCompTry.avgAway > secondCompTry.avgAway)
        result->append(firstCompName + " average away tries per match is " +
                QString::number(firstCompTry.avgAway - secondCompTry.avgAway) + " tries more than " + secondCompName
                + ".\n");
    else if (firstCompTry.avgAway < secondCompTry.avgAway)
        result->append(firstCompName + " average away tries per match is " +
                QString::number(secondCompTry.avgAway - firstCompTry.avgAway) + " tries less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away tries per match of " +
                       QString::number(firstCompTry.avgHome) + "tries.\n");

    if (firstCompTry.conPercent > secondCompTry.conPercent)
        result->append(firstCompName + " try conversion percentage is " +
                QString::number(firstCompTry.conPercent - secondCompTry.conPercent) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompTry.avgAway < secondCompTry.avgAway)
        result->append(firstCompName + " try conversion percentage is " +
                QString::number(secondCompTry.conPercent - firstCompTry.conPercent) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical try conversion percentages of " +
                       QString::number(firstCompTry.conPercent) + "%.\n");

    if (firstCompTry.penPerc > secondCompTry.penPerc)
        result->append(firstCompName + " penalty try percentage is " +
                QString::number(firstCompTry.penPerc - secondCompTry.penPerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompTry.penPerc < secondCompTry.penPerc)
        result->append(firstCompName + " penalty try percentage is " +
                QString::number(secondCompTry.penPerc - firstCompTry.penPerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical penalty try percentages of " +
                       QString::number(firstCompTry.conPercent) + "%.\n\n\n");
}

void DataAnalyser::comparePen(QString *result)
{
    result->append("Penalty Comparison\n\n");

    if (firstCompPen.avg > secondCompPen.avg)
        result->append(firstCompName + " average penalties per match is " +
                QString::number(firstCompPen.avg - secondCompPen.avg) + " penalties more than " + secondCompName
                + ".\n");
    else if (firstCompPen.avg < secondCompPen.avg)
        result->append(firstCompName + " average penalties per match is " +
                QString::number(secondCompPen.avg - firstCompPen.avg) + " penalties less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average penalties per match of " +
                QString::number(firstCompPen.avg) + " penalties.\n");

    if (firstCompPen.avgHome > secondCompPen.avgHome)
        result->append(firstCompName + " average home penalties per match is " +
                QString::number(firstCompPen.avgHome - secondCompPen.avgHome) + " penalties more than " + secondCompName
                + ".\n");
    else if (firstCompPen.avgHome < secondCompPen.avgHome)
        result->append(firstCompName + " average home penalties per match is " +
                QString::number(secondCompPen.avgHome - firstCompPen.avgHome) + " penalties less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home penalties per match of " +
                QString::number(firstCompPen.avgHome) + " penalties.\n");

    if (firstCompPen.avgAway > secondCompPen.avgAway)
        result->append(firstCompName + " average away penalties per match is " +
                QString::number(firstCompPen.avgAway - secondCompPen.avgAway) + " penalties more than " + secondCompName
                + ".\n");
    else if (firstCompPen.avgAway < secondCompPen.avgAway)
        result->append(firstCompName + " average away penalties per match is " +
                QString::number(secondCompPen.avgAway - firstCompPen.avgAway) + " penalties less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away penalties per match of " +
                QString::number(firstCompPen.avgAway) + " penalties.\n");

    if (firstCompPen.homePerc > secondCompPen.homePerc)
        result->append(firstCompName + " average home penalties percentage per match is " +
                QString::number(firstCompPen.homePerc - secondCompPen.homePerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompPen.homePerc < secondCompPen.homePerc)
        result->append(firstCompName + " average home penalties percentage per match is " +
                QString::number(secondCompPen.homePerc - firstCompPen.homePerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home penalties percentages per match of " +
                QString::number(firstCompPen.homePerc) + "%.\n");

    double firstAwayPerc = 100 - firstCompPen.homePerc;
    double secondAwayPerc = 100 - secondCompPen.homePerc;
    if (firstAwayPerc > secondAwayPerc)
        result->append(firstCompName + " average away penalties percentage per match is " +
                QString::number(firstAwayPerc - secondAwayPerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstAwayPerc < secondAwayPerc)
        result->append(firstCompName + " average away penalties percentage per match is " +
                QString::number(secondAwayPerc - firstAwayPerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away penalties percentages per match of " +
                       QString::number(firstAwayPerc) + "%.\n\n\n");
}

void DataAnalyser::compareDrop(QString *result)
{
    result->append("Drop Goal Comparison\n\n");

    if (firstCompDrop.avg > secondCompDrop.avg)
        result->append(firstCompName + " average drop goals per match is " +
                QString::number(firstCompDrop.avg - secondCompDrop.avg) + " drop goals more than " + secondCompName
                + ".\n");
    else if (firstCompDrop.avg < secondCompDrop.avg)
        result->append(firstCompName + " average drop goals per match is " +
                QString::number(secondCompDrop.avg - firstCompDrop.avg) + " drop goals less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average drop goals per match of " +
                QString::number(firstCompDrop.avg) + " drop goals.\n");

    if (firstCompDrop.avgHome > secondCompDrop.avgHome)
        result->append(firstCompName + " average home drop goals per match is " +
                QString::number(firstCompDrop.avgHome - secondCompDrop.avgHome) + " drop goals more than " + secondCompName
                + ".\n");
    else if (firstCompDrop.avgHome < secondCompDrop.avgHome)
        result->append(firstCompName + " average home drop goals per match is " +
                QString::number(secondCompDrop.avgHome - firstCompDrop.avgHome) + " drop goals less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home drop goals per match of " +
                QString::number(firstCompDrop.avgHome) + " drop goals.\n");

    if (firstCompDrop.avgAway > secondCompDrop.avgAway)
        result->append(firstCompName + " average away drop goals per match is " +
                QString::number(firstCompDrop.avgAway - secondCompDrop.avgAway) + " drop goals more than " + secondCompName
                + ".\n");
    else if (firstCompDrop.avgAway < secondCompDrop.avgAway)
        result->append(firstCompName + " average away drop goals per match is " +
                QString::number(secondCompDrop.avgAway - firstCompDrop.avgAway) + " drop goals less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away drop goals per match of " +
                QString::number(firstCompDrop.avgAway) + " drop goals.\n");

    if (firstCompDrop.homePerc > secondCompDrop.homePerc)
        result->append(firstCompName + " average home drop goals percentage per match is " +
                QString::number(firstCompDrop.homePerc - secondCompDrop.homePerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompDrop.homePerc < secondCompDrop.homePerc)
        result->append(firstCompName + " average home drop goals percentage per match is " +
                QString::number(secondCompDrop.homePerc - firstCompDrop.homePerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home drop goals percentages per match of " +
                QString::number(firstCompDrop.homePerc) + "%.\n");

    double firstAwayPerc = 100 - firstCompDrop.homePerc;
    double secondAwayPerc = 100 - secondCompDrop.homePerc;
    if (firstAwayPerc > secondAwayPerc)
        result->append(firstCompName + " average away drop goals percentage per match is " +
                QString::number(firstAwayPerc - secondAwayPerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstAwayPerc < secondAwayPerc)
        result->append(firstCompName + " average away drop goals percentage per match is " +
                QString::number(secondAwayPerc - firstAwayPerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away drop goals percentages per match of " +
                       QString::number(firstAwayPerc) + "%.\n\n\n");
}

void DataAnalyser::compareBookings(QString *result)
{
    result->append("Booking Comparison\n\n");

    if (firstCompBookings.avg > secondCompBookings.avg)
        result->append(firstCompName + " average bookings per match is " +
                QString::number(firstCompBookings.avg - secondCompBookings.avg) + " booking/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.avg < secondCompBookings.avg)
        result->append(firstCompName + " average bookings per match is " +
                QString::number(secondCompBookings.avg - firstCompBookings.avg) + " booking/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average bookings per match of " +
                QString::number(firstCompBookings.avg) + " booking/s.\n");

    if (firstCompBookings.avgHome > secondCompBookings.avgHome)
        result->append(firstCompName + " average home bookings per match is " +
                QString::number(firstCompBookings.avgHome - secondCompBookings.avgHome) + " booking/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.avgHome < secondCompBookings.avgHome)
        result->append(firstCompName + " average home bookings per match is " +
                QString::number(secondCompBookings.avgHome - firstCompBookings.avgHome) + " booking/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average home bookings per match of " +
                QString::number(firstCompBookings.avgHome) + " booking/s.\n");

    if (firstCompBookings.avgAway > secondCompBookings.avgAway)
        result->append(firstCompName + " average away bookings per match is " +
                QString::number(firstCompBookings.avgAway - secondCompBookings.avgAway) + " booking/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.avgAway < secondCompBookings.avgAway)
        result->append(firstCompName + " average away bookings per match is " +
                QString::number(secondCompBookings.avgAway - firstCompBookings.avgAway) + " booking/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average away bookings per match of " +
                QString::number(firstCompBookings.avgAway) + " booking/s.\n");

    if (firstCompBookings.homePerc > secondCompBookings.homePerc)
        result->append(firstCompName + " percentage of home bookings is " +
                QString::number(firstCompBookings.homePerc - secondCompBookings.homePerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.homePerc < secondCompBookings.homePerc)
        result->append(firstCompName + " percentage of home bookings is " +
                QString::number(secondCompBookings.homePerc - firstCompBookings.homePerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical percentages of home bookings of " +
                QString::number(firstCompBookings.homePerc) + " booking/s.\n");

    double firstAwayPerc = 100 - firstCompBookings.homePerc;
    double secondAwayPerc = 100 - secondCompBookings.homePerc;
    if (firstAwayPerc > secondAwayPerc)
        result->append(firstCompName + " percentage of away bookings is " +
                QString::number(firstAwayPerc - secondAwayPerc) + "% more than " + secondCompName
                + ".\n");
    else if (firstAwayPerc < secondAwayPerc)
        result->append(firstCompName + " percentage of away bookings is " +
                QString::number(secondAwayPerc - firstAwayPerc) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical percentages of away bookings of " +
                QString::number(firstAwayPerc) + " booking/s.\n");

    if (firstCompBookings.yelPer > secondCompBookings.yelPer)
        result->append(firstCompName + " percentage of yellow cards is " +
                QString::number(firstCompBookings.yelPer - secondCompBookings.yelPer) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.yelPer < secondCompBookings.yelPer)
        result->append(firstCompName + " percentage of yellow cards is " +
                QString::number(secondCompBookings.yelPer - firstCompBookings.yelPer) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical percentages of yellow cards of " +
                QString::number(firstCompBookings.yelPer) + " card/s.\n");

    if (firstCompBookings.redPer > secondCompBookings.redPer)
        result->append(firstCompName + " percentage of red cards is " +
                QString::number(firstCompBookings.redPer - secondCompBookings.redPer) + "% more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.redPer < secondCompBookings.redPer)
        result->append(firstCompName + " percentage of red cards is " +
                QString::number(secondCompBookings.redPer - firstCompBookings.redPer) + "% less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical percentages of red cards of " +
                QString::number(firstCompBookings.redPer) + " card/s.\n");

    if (firstCompBookings.yAvgCon > secondCompBookings.yAvgCon)
        result->append(firstCompName + " average conceded with player sin binned is " +
                QString::number(firstCompBookings.yAvgCon - secondCompBookings.yAvgCon) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.yAvgCon < secondCompBookings.yAvgCon)
        result->append(firstCompName + " average conceded with player sin binned is " +
                QString::number(secondCompBookings.yAvgCon - firstCompBookings.yAvgCon) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical averages conceded with player sin binned of " +
                QString::number(firstCompBookings.yAvgCon) + " points/s.\n");

    if (firstCompBookings.yAvgScor > secondCompBookings.yAvgScor)
        result->append(firstCompName + " average scored with player sin binned is " +
                QString::number(firstCompBookings.yAvgScor - secondCompBookings.yAvgScor) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.yAvgScor < secondCompBookings.yAvgScor)
        result->append(firstCompName + " average scored with player sin binned is " +
                QString::number(secondCompBookings.yAvgScor - firstCompBookings.yAvgScor) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical averages scored with player sin binned of " +
                QString::number(firstCompBookings.yAvgScor) + " points/s.\n");

    if (firstCompBookings.yAvgDiff > secondCompBookings.yAvgDiff)
        result->append(firstCompName + " average differential with player sin binned is " +
                QString::number(firstCompBookings.yAvgDiff - secondCompBookings.yAvgDiff) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.yAvgDiff < secondCompBookings.yAvgDiff)
        result->append(firstCompName + " average differential with player sin binned is " +
                QString::number(secondCompBookings.yAvgDiff - firstCompBookings.yAvgDiff) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average differentials with player sin binned of " +
                QString::number(firstCompBookings.yAvgDiff) + " points/s.\n");

    if (firstCompBookings.rAvgCon > secondCompBookings.rAvgCon)
        result->append(firstCompName + " average conceded with player sent off is " +
                QString::number(firstCompBookings.rAvgCon - secondCompBookings.rAvgCon) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.rAvgCon < secondCompBookings.rAvgCon)
        result->append(firstCompName + " average conceded with player sent off is " +
                QString::number(secondCompBookings.rAvgCon - firstCompBookings.rAvgCon) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical averages conceded with player sent off of " +
                QString::number(firstCompBookings.rAvgCon) + "points/s.\n");

    if (firstCompBookings.rAvgScor > secondCompBookings.rAvgScor)
        result->append(firstCompName + " average scored with player sent off is " +
                QString::number(firstCompBookings.rAvgScor - secondCompBookings.rAvgScor) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.rAvgScor < secondCompBookings.rAvgScor)
        result->append(firstCompName + " average scored with player sent off is " +
                QString::number(secondCompBookings.rAvgScor - firstCompBookings.rAvgScor) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical averages scored with player sent off of " +
                QString::number(firstCompBookings.rAvgScor) + " points/s.\n");

    if (firstCompBookings.rAvgDiff > secondCompBookings.rAvgDiff)
        result->append(firstCompName + " average differential with player sent off is " +
                QString::number(firstCompBookings.rAvgDiff - secondCompBookings.rAvgDiff) + " point/s more than " + secondCompName
                + ".\n");
    else if (firstCompBookings.rAvgDiff < secondCompBookings.rAvgDiff)
        result->append(firstCompName + " average differential with player sent off is " +
                QString::number(secondCompBookings.rAvgDiff - firstCompBookings.rAvgDiff) + " point/s less than " + secondCompName
                + ".\n");
    else
        result->append(firstCompName + " and " + secondCompName + " have identical average differentials with player sent off of " +
                QString::number(firstCompBookings.rAvgDiff) + " points/s.\n\n\n");

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

void DataAnalyser::setPieChart2(double a, double b)
{
    QPieSeries *series2 = new QPieSeries();
    series2->append(firstCompName + " - " + QString::number(a), a);
    series2->append(secondCompName + " - " + QString::number(b), b);

    QChart *chart = new QChart();
    chart->addSeries(series2);
    chart->setTitle("Average Per Match Comparison");

    chart2 = new QChartView(chart);
}

void DataAnalyser::setChart1(QString str, QVector <int> arr)
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

void DataAnalyser::setChart2(QVector<int> arrA, QVector<int> arrB)
{
    QBarSet *set0 = new QBarSet(firstCompName);
    QBarSet *set1 = new QBarSet(secondCompName);
    QStringList categories;
    for (int i = 0; i < arrA.size() - 1; i++){
        *set0 << arrA.at(i);
        *set1 << arrB.at(i);
        QString str1;
        if (i == 8)
            str1.append(QString::number(i) + "0+");
        else
            str1.append(QString::number(i) + "0-" + QString::number(i+1) + "0");
        categories << str1;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
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
    QBarSet *set0 = new QBarSet(firstCompName);
    QStringList categories;
    for (int i = 0; i < homeScoresArr.size() - 1; i++){
        *set0 << homeScoresArr.at(i);
        QString str1;
        if (i == 8)
            str1.append(QString::number(i) + "0+");
        else
            str1.append(QString::number(i) + "0-" + QString::number(i+1) + "0");
        categories << str1;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    if (!firstComp){
        QBarSet *set1 = new QBarSet(secondCompName);
        for (int i = 0; i < awayScoresArr.size() - 1; i++)
            *set1 << awayScoresArr.at(i);
        series->append(set1);
    }
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("All Scores Match Time Analysis");
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
     QChart *chart = new QChart();
    if (firstComp){
        QPieSeries *series2 = new QPieSeries();
        series2->append("Home - " + QString::number(firstCompMatch.homePerc) + "%", firstCompMatch.homePerc);
        series2->append("Away - " + QString::number(firstCompMatch.awayPerc) + "%", firstCompMatch.awayPerc);
        series2->append("Draw - " + QString::number(100 - (firstCompMatch.homePerc + firstCompMatch.awayPerc)) + "%", 100 - (firstCompMatch.homePerc + firstCompMatch.awayPerc));

        chart->addSeries(series2);
        chart->setTitle("Home/Away Wins");
    }
    else{
        QBarSet *set0 = new QBarSet("Home Win %");
        QBarSet *set1 = new QBarSet("Away Win %");
        QBarSet *set2 = new QBarSet("Draw %");
        *set0 << firstCompMatch.homePerc << secondCompMatch.homePerc;
        *set1 << firstCompMatch.awayPerc << secondCompMatch.awayPerc;
        *set2 << 100 - (firstCompMatch.homePerc + firstCompMatch.awayPerc) << 100 - (secondCompMatch.homePerc + secondCompMatch.awayPerc);

        QPercentBarSeries *series = new QPercentBarSeries();
        series->append(set0);
        series->append(set1);
        series->append(set2);

        chart->addSeries(series);
        chart->setTitle("Win/Draw Breakdown Comparison");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QStringList categories;
        categories << firstCompName << secondCompName;
        QBarCategoryAxis *axis = new QBarCategoryAxis();
        axis->append(categories);
        chart->createDefaultAxes();
        chart->setAxisX(axis, series);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
    }
    chart2 = new QChartView(chart);
}

void DataAnalyser::analyseStories(int totMatches, QVector<QString> queries, QString *result)
{
    int oldNo, newNo;
    result->append("Total Matches in Database: " + QString::number(totMatches) + "\n\n");
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
    generateStories(&matchesVec, &diffs);

    //qDebug() << result << endl;
    groupStories(result, &diffs);
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

void DataAnalyser::generateStories(QVector<Match> * matches, QVector <QVector <int> > * diffs)
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

            diff.insert(j+1, homeScr - awayScr);
            //output->append("   " + QString::number(time) + ": " + QString::number(homeScr - awayScr));
        }
        diffs->insert(i, diff);
    }
}

void DataAnalyser::groupStories(QString *output, QVector<QVector <int> > *diffs)
{
    QChart *chart = new QChart();
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
                                if (match0.at(0) == 141)
                                    series0->append(k, match0.at(k+1));
                            }
                            output->append("\nMatch 2 Story: ");
                            for (int k = 0; k < 81; k++){
                                output->append(" " + QString::number(match1.at(k+1)));
                                if (match0.at(0) == 141)
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

    chart->createDefaultAxes();
    chart->setTitle("Similar Game Stories Example");

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

void DataAnalyser::analyseConditions(int num, QVector<QString> queries, QString * result, QString comp)
{
    if (firstComp)
        firstCompName = comp;
    else
        secondCompName = comp;

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

    if (firstComp){
        firstCompBookings.yAvgCon = yAvgCon;
        firstCompBookings.yAvgScor = yAvgScor;
        firstCompBookings.yAvgDiff = yAvgDiff;
        firstCompBookings.rAvgCon = rAvgCon;
        firstCompBookings.rAvgScor = rAvgScor;
        firstCompBookings.rAvgDiff = rAvgDiff;
    }
    else{
        secondCompBookings.yAvgCon = yAvgCon;
        secondCompBookings.yAvgScor = yAvgScor;
        secondCompBookings.yAvgDiff = yAvgDiff;
        secondCompBookings.rAvgCon = rAvgCon;
        secondCompBookings.rAvgScor = rAvgScor;
        secondCompBookings.rAvgDiff = rAvgDiff;
    }
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
    if (i == 1){
        for (int j = 0; j < homeScoresArr.size(); j++){
            homeScoresArr.replace(j, 0);
            awayScoresArr.replace(j, 0);
        }

    }
}

void DataAnalyser::setFirstComp()
{
    if (firstComp == true)
        firstComp = false;
    else
        firstComp = true;
}


