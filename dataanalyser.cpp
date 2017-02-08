#include "dataanalyser.h"
#include "querydb.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>
#include <Eigen/Dense>
#include <iostream>
#include <iomanip>

using namespace Eigen;

DataAnalyser::DataAnalyser()
{

}

QString DataAnalyser::analyse(QSqlQuery qry, int idx, int num, QVector <int> * arr)
{
    QString result;
    switch (idx) {
    case 0: result += analyseMatches(qry);
        break;
    case 1: result = analyseTries(qry, num, arr);
        break;
    case 2: result = analysePenaltiesDropGoals(qry, 0, num, arr);
        break;
    case 3: result = analysePenaltiesDropGoals(qry, 1, num, arr);
        break;
    case 4: result = analyseBookings(qry, num, arr);
        break;
    default:
        break;
    }
    return result;
}

QString DataAnalyser::analyseTries(QSqlQuery qry, int totMatches, QVector <int> * arr)
{
    QString result;

    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeTries = 0, converted = 0, penalty = 0, no = 0, oldNo, newNo;
    while (qry.next()) {
        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr->at(b) + 1;
        arr->removeAt(b);
        arr->insert(b, x);
        if (b > 8){
            int x = arr->at(8) + 1;
            arr->removeAt(8);
            arr->insert(8, x);
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
    result += "Analysis of Tries Scored\n\nTotal number of matches in competition: " + QString::number(totMatches) +
                "\nTotal number of matches in query: " + QString::number(no) +
                "\nTotal number of tries scored: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr->at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result += "Tries scored between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes: " + QString::number(n) + ", " + QString::number(y) + "% of total tries.\n";
        }
        else{
            result += "Tries scored after 80 minutes: " + QString::number(n) + ", " + QString::number(y)
                        + "% of total tries.\n\n";
        }
    }
    qDebug() << noMatches << endl;
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeTries/(totMatches/2))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeTries)/(totMatches/2))/scale + 0.5) * scale;
    double conPercent = floor(((double)converted/num * 100) / scale + 0.5) * scale;
    double penPerc = floor(((double)penalty/num * 100) / scale + 0.5) * scale;
    double homePerc = floor(((double)homeTries/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    result += "Average number of tries per match: " + QString::number(avg) + "\nConverted Tries: " +
                QString::number(converted) + ", " + QString::number(conPercent) + "% of total tries.\nPenalty Tries: " +
                QString::number(penalty) + ", " + QString::number(penPerc) + "% of total tries.\nHome team Tries: " +
                QString::number(homeTries) + ", " + QString::number(homePerc) + "% of total tries.\nAverage home tries: "
                + QString::number(avgHome) + ".\nAway team Tries: " + QString::number(num - homeTries) + ", " +
                QString::number(100.00 - homePerc) + "% of total tries.\nAverage away tries: " + QString::number(avgAway)
                + ".";
    return result;
}

QString DataAnalyser::analyseMatches(QSqlQuery qry)
{
    QString result;
    int num = 0, homeWins = 0, awayWins = 0, totHome = 0, totAway = 0, maxScore = 0;
    QVector <QString>  comps;
    while (qry.next()) {
         num++;
         QString comp = qry.value(4).toString();
         bool found = false;
         for (int i = 0; i < comps.size() && !found; i++)
             if (comp == comps[i])
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
    double awayPerc = floor(((double)awayWins/num * 100) / scale + 0.5) * scale;
    int numTeams = num / 2;
    double avg = floor(((double)(totHome + totAway)/num) / scale + 0.5) * scale;
    double avgHome = floor(((double)totHome/numTeams) / scale + 0.5) * scale;
    double avgAway = floor(((double)totAway/numTeams) / scale + 0.5) * scale;

    //qDebug() << totHome << " " << totAway << " " << numTeams << endl;

    result += "Match Analysis\nNumber of matches: " + QString::number(num) + ".\nNumber of Competitions: " +
                QString::number(comps.size()) + ".\n\nNumber of home wins: " + QString::number(homeWins) + ", " +
                QString::number(homePerc) + "% of total matches.\nNumber of away wins: " + QString::number(awayWins)
                + ", " + QString::number(awayPerc) + "% of total matches.\nNumber of draws: " +
                QString::number(num - (homeWins + awayWins)) + ", " + QString::number(100.00 - (homePerc + awayPerc))
                + "% of total matches.\n\nAverage score: " + QString::number(avg) + ".\nAverage home score: " +
                QString::number(avgHome) + ".\nAverage away score: " + QString::number(avgAway) + ".\nMaximum score: "
                + QString::number(maxScore) + ".";
    return result;
}

QString DataAnalyser::analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches, QVector <int> * arr)
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
    QString result;
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeScores = 0, no = 0, oldNo, newNo;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr->at(b) + 1;
        arr->removeAt(b);
        arr->insert(b, x);
        if (b > 8){
            int x = arr->at(8) + 1;
            arr->removeAt(8);
            arr->insert(8, x);
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
    result += "Analysis of " + big + " Scored\n\nTotal number of matches in competition: " + QString::number(totMatches)
            + "\nTotal number of matches in query: " + QString::number(no) +
                "\nTotal number of " + small + " scored: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr->at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result += big + " scored between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes: " + QString::number(n) + ", " + QString::number(y) + "% of total " + small + ".\n";
        }
        else{
            result += big + " scored after 80 minutes: " + QString::number(n) + ", " + QString::number(y)
                        + "% of total " + small + ".\n\n";
        }
    }
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeScores/(totMatches/2))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeScores)/(totMatches/2))/scale + 0.5) * scale;
    double homePerc = floor(((double)homeScores/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    result += "Average number of " + small + " per match: " + QString::number(avg) + "\nHome team " + small + ": " +
                QString::number(homeScores) + ", " + QString::number(homePerc) +
                "% of total " + small + ".\nAverage home " + small + ": " + QString::number(avgHome) +
                ".\nAway team " + small + ": " + QString::number(num - homeScores) + ", " +
                QString::number(100.00 - homePerc) + "% of total " + small + ".\nAverage away " + small + ": " +
                QString::number(avgAway) + ".";
    return result;
}

QString DataAnalyser::analyseBookings(QSqlQuery qry, int totMatches, QVector<int> *arr)
{
    QString result;
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeBookings = 0, yellows = 0, homeYellows = 0, homeReds = 0,
                no = 0, oldNo, newNo;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        int x = arr->at(b) + 1;
        arr->removeAt(b);
        arr->insert(b, x);
        if (b > 8){
            int x = arr->at(8) + 1;
            arr->removeAt(8);
            arr->insert(8, x);
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

    result += "Analysis of Bookings\n\nTotal number of matches in competition: " + QString::number(totMatches) +
            "\nTotal number of matches in query: " + QString::number(no) +
                "\nTotal number of bookings: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr->at(i-1);
        double y = floor(((double)n/num * 100) / scale + 0.5) * scale;
        if (i != 9){
            result += "Bookings between " + QString::number(i-1) + "0 minutes and " + QString::number(i) +
                    "0 minutes: " + QString::number(n) + ", " + QString::number(y) + "% of total bookings.\n";
        }
        else{
            result += "Bookings after 80 minutes: " + QString::number(n) + ", " + QString::number(y)
                        + "% of total penalties.\n\n";
        }
    }
    double avg = floor(((double)num/totMatches)/scale + 0.5) * scale;
    double avgHome = floor(((double)homeBookings/(totMatches/2))/scale + 0.5) * scale;
    double avgAway = floor(((double)(num - homeBookings)/(totMatches/2))/scale + 0.5) * scale;
    double homePerc = floor(((double)homeBookings/num * 100) / scale + 0.5) * scale;

    setPieChart(homePerc);

    double yelPer = floor(((double)yellows/num * 100) / scale + 0.5) * scale;
    double redPer = floor(((double)(num - yellows)/num * 100) / scale + 0.5) * scale;

    int reds = num - yellows;

    result += "Average number of bookings per match: " + QString::number(avg) + "\nHome team bookings: " +
                QString::number(homeBookings) + ", " + QString::number(homePerc) +
                "% of total bookings.\nAverage home bookings: " + QString::number(avgHome) +
                ".\nAway team bookings: " + QString::number(num - homeBookings) + ", " +
                QString::number(100.00 - homePerc) + "% of total bookings.\nAverage away bookings: " +
                QString::number(avgAway) + ".\n\nTotal yellow cards: " + QString::number(yellows) + ", " +
                QString::number(yelPer) + ".\nHome yellows: " + QString::number(homeYellows) + ".\nAway yellows: "
                + QString::number(yellows - homeYellows) + ".\n\nTotal red cards: " + QString::number(reds) + ", "
                + QString::number(redPer) + ".\nHome reds: " + QString::number(homeReds) + ".\nAway reds: "
                + QString::number(reds - homeReds) + ".";

    return result;
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
        output->append("\n\nMatch Id: " + QString::number(m.id) + "   " + QString::number(0) + ": " +
                            QString::number(0));
        for (int j = 1; j < 17; j++){
            time = j * 5;
            home5 = 0, away5 = 0;
            for (k; k < m.home.size();){
                if (j != 17 && m.home.at(k).time <= time){
                    home5 = m.home.at(k).value;
                    homeScr += home5;
                    k++;
                }
                else if (j == 17){
                    home5 = m.home.at(k).value;
                    homeScr += home5;
                    k++;
                }
                else
                    break;
            }
            for (l; l < m.away.size();){
                if (j != 17 && m.away.at(l).time <= time){
                    away5 = m.away.at(l).value;
                    awayScr += away5;
                    l++;
                }
                else if (j == 17){
                    away5 = m.away.at(l).value;
                    awayScr += away5;
                    l++;
                }
                else
                    break;
            }
            int val, scr = home5 - away5;
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
            //diff.insert(j, val);
            diff.insert(j+1, homeScr - awayScr);
            output->append("   " + QString::number(time) + ": " + QString::number(homeScr - awayScr));
        }
        diffs->insert(i, diff);
    }
}

void DataAnalyser::groupStories(QString *output, QVector<QVector <int> > *diffs)
{
    output->append("\n\n\nLines of best fit\n");
    QChart *chart = new QChart();
    int i, n = 16;
    for (int iq = 0; iq < diffs->size(); iq++){
        QLineSeries *series = new QLineSeries();
        double x[17], y[17], a, b;
        QVector <int> d = diffs->at(iq);
        for (int j = 0; j < 17; j++){
            x[j] = j*5;
            y[j] = d.at(j+1);
            series->append(j*5, d.at(j+1));
        }
        chart->addSeries(series);

        double xsum=0, x2sum=0, ysum=0, xysum=0;                //variables for sums/sigma of xi,yi,xi^2,xiyi etc
        for (i=0;i<17;i++)
        {
            xsum = xsum+x[i];                        //calculate sigma(xi)
            ysum = ysum+y[i];                        //calculate sigma(yi)
            x2sum = x2sum+pow(x[i],2);                //calculate sigma(x^2i)
            xysum = xysum+x[i]*y[i];                    //calculate sigma(xi*yi)
        }
        a=(n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);            //calculate slope
        b=(x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);            //calculate intercept
        double y_fit[17];                        //an array to store the new fitted values of y
        for (i=0;i<17;i++)
            y_fit[i]=a*x[i]+b;                    //to calculate y(fitted) at given x points
        qDebug() << "Match Id:" << d.at(0) << ": " << a << "x + " << b << endl;
        output->append("\nMatch Id:" + QString::number(d.at(0)) + ": " + QString::number(a) + "x + " +
                            QString::number(b));
    }

    chart->createDefaultAxes();
    chart->setTitle("Game Stories");
    chart->legend()->hide();

    chart2 = new QChartView(chart);

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
    }



    /*output->append("\n\n\nStory Comparison\n");
    for (int i = 0; i < diffs->size(); i++){
        for (int j = i + 1; j < diffs->size(); j++){
            bool equal = true;
            for (int k = 1; k < 17 && equal; k++){
                if (diffs->at(i).at(k) != diffs->at(j).at(k))
                    equal = false;
            }
            if (equal){
                output->append("\nMatch Found: Id: " + QString::number(diffs->at(i).at(0)) + " and Id: " +
                        QString::number(diffs->at(j).at(0)));
                /*for (int k = 0; k < 17 && equal; k++)
                    output->append("\n " + QString::number(diffs->at(i).at(k)) + " " +
                                    QString::number(diffs->at(j).at(k)));
            }
        }
    }*/
}

QString DataAnalyser::tryConditions(QSqlQuery qry, QVector<Booking> *bookings)
{
    QString output;// = "\n\n\nAnalysis of Tries Scored during sin bin.\n\n\n";
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
    return output;
}

QString DataAnalyser::penDropConditions(QSqlQuery qry, int eve, QVector <Booking> * bookings)
{
    QString event;
    if (eve == 0)
        event = "Penalties";
    else
        event = "Drop Goals";
    QString output;// = "Analysis of " + event + " Scored during sin bin.\n\n\n";
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

    return output;
}

void DataAnalyser::setPieChart(double perc)
{
    QPieSeries *series2 = new QPieSeries();
    series2->append("Home - " + QString::number(perc) + "%", perc);
    series2->append("Away - " + QString::number(100 - perc) + "%", 100 - perc);

    QChart *chart = new QChart();
    chart->addSeries(series2);
    chart->setTitle("Home/Away Team Comparison");
    //chart->legend()->hide();

    chart2 = new QChartView(chart);
}

QString DataAnalyser::analyseStories(int totMatches, QVector<QString> queries)
{
    int oldNo, newNo;
    QString result = "Game Stories Generation\n\nTotal Matches in Query: " + QString::number(totMatches) + "\n\n\n";
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
    generateStories(&matchesVec, &result, &diffs);

    qDebug() << result << endl;
    groupStories(&result, &diffs);

    return result;
}

QString DataAnalyser::analyseConditions(int num, QVector<QString> queries, QVector <int> * arr)
{
    QVector <Booking> bookings;
    QString result;
    for (int i = 0; i < queries.size(); i++){
        QString query = queries.at(i);
        QueryDB qdb;
        QSqlQuery qry;
        qdb.setQuery(query);
        qry = qdb.executeQuery();

        switch (i) {
        case 0: result += analyseBookings(qry, num, arr);
            break;
        case 1: result += tryConditions(qry, &bookings);
            break;
        case 2: result += penDropConditions(qry, 0, &bookings);
            break;
        case 3: result += penDropConditions(qry, 1, &bookings);
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
       // qDebug() << scored << " " << avgScor << " " << conceded << " " << avgCon << " " << size << endl;
        /*result += "\nBooking Id: " + QString::number(b.id) + ", time: " + QString::number(b.time) + ", team: " +
                    QString::number(b.team) + ", scored: " + QString::number(b.scored) + ", conceded: " +
                    QString::number(b.conceded);*/
    }
    yAvgCon = floor(((double)yAvgCon / ySize)/scale + 0.5) * scale;
    yAvgScor = floor(((double)yAvgScor / ySize)/scale + 0.5) * scale;
    yAvgDiff = floor(((double)yAvgDiff / ySize)/scale + 0.5) * scale;
    rAvgCon = floor(((double)rAvgCon / rSize)/scale + 0.5) * scale;
    rAvgScor = floor(((double)rAvgScor / rSize)/scale + 0.5) * scale;
    rAvgDiff = floor(((double)rAvgDiff / rSize)/scale + 0.5) * scale;
    result += "\n\n\nYellow Card Score Analysis:\n\nAverage score conceded by team with sin binned player: " +
                QString::number(yAvgCon) + "\n\nAverage score scored by team with sin binned player: " +
                QString::number(yAvgScor) + "\n\nAverage change in score during a sin bin: " +
                QString::number(yAvgDiff)+
                "\n\nRed Card Score Analysis:\n\nAverage score conceded by team with player sent off: " +
                QString::number(rAvgCon) +
                "\n\nAverage score scored by team with player sent off: " + QString::number(rAvgScor) +
                "\n\nAverage change in score when player sent off: " + QString::number(rAvgDiff);
    return result;
}

QChartView *DataAnalyser::getChart()
{
    return chart2;
}

QString DataAnalyser::compare(QString str1, QString str2)
{
    QString result = "Comparison of Data Sets\n\n" + str1 + "\n\n\n" + str2;
    return result;
}

