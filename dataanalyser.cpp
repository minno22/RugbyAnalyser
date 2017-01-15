#include "dataanalyser.h"

DataAnalyser::DataAnalyser()
{

}

QString DataAnalyser::analyse(QSqlQuery qry, int idx, int num)
{
    QString result;
    switch (idx) {
    case 0: result = analyseMatches(qry);
        break;
    case 1: result = analyseTries(qry, num);
        break;
    case 2: result = analysePenaltiesDropGoals(qry, 0, num);
        break;
    case 3: result = analysePenaltiesDropGoals(qry, 1, num);
        break;
    case 4: result = analyseBookings(qry, num);
        break;
    default:
        break;
    }
    return result;
}

QString DataAnalyser::analyseTries(QSqlQuery qry, int totMatches)
{
    QString result;
    int arr[9] = {0};
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeTries = 0, converted = 0, penalty = 0;
    while (qry.next()) {
        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        arr[b]++;
        if (b > 8){
            arr[8]++;
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

        if (qry.value(13).toInt() == 0)
            homeTries++;
        if (qry.value(14).toInt() == 1)
            converted++;
        if (qry.value(15).toInt() == 1)
            penalty++;
    }
    result += "Analysis of Tries Scored\n\nTotal number of matches in query: " + QString::number(totMatches) +
                "\nTotal number of tries scored: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr[i-1];
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

QString DataAnalyser::analysePenaltiesDropGoals(QSqlQuery qry, int eve, int totMatches)
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
    int arr[9] = {0};
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeScores = 0;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        arr[b]++;
        if (b > 8){
            arr[8]++;
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
    }
    result += "Analysis of " + big + " Scored\n\nTotal number of matches in query: " + QString::number(totMatches) +
                "\nTotal number of " + small + " scored: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr[i-1];
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

    result += "Average number of " + small + " per match: " + QString::number(avg) + "\nHome team " + small + ": " +
                QString::number(homeScores) + ", " + QString::number(homePerc) +
                "% of total " + small + ".\nAverage home " + small + ": " + QString::number(avgHome) +
                ".\nAway team " + small + ": " + QString::number(num - homeScores) + ", " +
                QString::number(100.00 - homePerc) + "% of total " + small + ".\nAverage away " + small + ": " +
                QString::number(avgAway) + ".";
    return result;
}

QString DataAnalyser::analyseBookings(QSqlQuery qry, int totMatches)
{
    QString result;
    int arr[9] = {0};
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeBookings = 0, yellows = 0, homeYellows = 0, homeReds = 0;
    while (qry.next()) {
        num++;

        int a = qry.value(12).toInt();
        int b = (int)(a/10);
        arr[b]++;
        if (b > 8){
            arr[8]++;
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
    }

    result += "Analysis of Bookings\n\nTotal number of matches in query: " + QString::number(totMatches) +
                "\nTotal number of bookings: " + QString::number(num) + "\n\n";
    for (int i = 1; i < 10; i++){
        int n = arr[i-1];
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

