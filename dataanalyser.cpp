#include "dataanalyser.h"
#include "querydb.h"

DataAnalyser::DataAnalyser()
{

}

QString DataAnalyser::analyse(QSqlQuery qry, int idx, int num)
{
    QString result;
    switch (idx) {
    case 0: result += analyseMatches(qry);
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
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeTries = 0, converted = 0, penalty = 0, no = 0, oldNo, newNo;
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
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeScores = 0, no = 0, oldNo, newNo;
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
    int num = 0, noMatches = 0, matchIdOld, matchIdNew, homeBookings = 0, yellows = 0, homeYellows = 0, homeReds = 0,
                no = 0, oldNo, newNo;
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
        output->append("\n\nMatch Id: " + QString::number(m.id));
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
            diff.insert(j, val);
            output->append("   " + QString::number(time) + ": " + QString::number(homeScr - awayScr));
        }
        diffs->insert(i, diff);
    }
}

void DataAnalyser::groupStories(QString *output, QVector<QVector <int> > *diffs)
{
    output->append("\n\n\nStory Comparison\n");
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
                                    QString::number(diffs->at(j).at(k)));*/
            }
        }
    }
}

QString DataAnalyser::tryConditions(QSqlQuery qry, int totMatches, QVector<Booking> *bookings)
{
    QString output = "Analysis of Tries Scored during sin bin.\n\n\n";
    QString cardColour = "Yellow";
    Booking b;
    while (qry.next()){
        if (qry.value(4).toString() == "Yellow"){
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
                b.scored = 0;
                b.conceded = 0;
            }
            int tryTime = qry.value(7).toInt();
            if (tryTime >= b.time && tryTime < b.time + 10){
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
    }
    /*for (int i = 0; i < bookings->size(); i++){
        b = bookings->at(i);
        output += "\nBooking Id: " + QString::number(b.id) + ", time: " + QString::number(b.time) + ", team: " +
                    QString::number(b.team) + ", scored: " + QString::number(b.scored) + ", conceded: " +
                    QString::number(b.conceded);
    }*/
    return output;
}

QString DataAnalyser::penDropConditions(QSqlQuery qry, int eve, int totMatches, QVector <Booking> * bookings)
{
    QString event;
    if (eve == 0)
        event = "Penalties";
    else
        event = "Drop Goals";
    QString output = "Analysis of " + event + " Scored during sin bin.\n\n\n";
    QString cardColour = "Yellow";
    Booking b;
    while (qry.next()){
        if (qry.value(4).toString() == "Yellow"){
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
                b.scored = 0;
                b.conceded = 0;
            }
            int scoreTime = qry.value(7).toInt();
            if (scoreTime >= b.time && scoreTime < b.time + 10){
                int score = 3;

                if (qry.value(8).toInt() == b.team)
                    b.scored += score;
                else
                    b.conceded += score;
            }
            bookings->push_front(b);
        }
    }

    return output;
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

QString DataAnalyser::analyseConditions(int num, QVector<QString> queries)
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
        case 0: result += analyseBookings(qry, num);
            break;
        case 1: result += tryConditions(qry, num, &bookings);
            break;
        case 2: result += penDropConditions(qry, 0, num, &bookings);
            break;
        case 3: result += penDropConditions(qry, 1, num, &bookings);
            break;
        default:
            break;
        }
        qry.clear();
    }

    double avgCon = 0.0, avgScor = 0.0, avgDiff = 0.0;
    int size = bookings.size();
    for (int i = 0; i < size; i++){
        Booking b;
        b = bookings.at(i);
        int scored = b.scored;
        int conceded = b.conceded;
        avgCon += conceded;
        avgScor += scored;
        avgDiff += scored - conceded;
       // qDebug() << scored << " " << avgScor << " " << conceded << " " << avgCon << " " << size << endl;
        /*result += "\nBooking Id: " + QString::number(b.id) + ", time: " + QString::number(b.time) + ", team: " +
                    QString::number(b.team) + ", scored: " + QString::number(b.scored) + ", conceded: " +
                    QString::number(b.conceded);*/
    }
    avgCon = floor(((double)avgCon / size)/scale + 0.5) * scale;
    avgScor = floor(((double)avgScor / size)/scale + 0.5) * scale;
    avgDiff = floor(((double)avgDiff / size)/scale + 0.5) * scale;
    result += "Number of Matches in Query: " + QString::number(num) + "\nNumber of Bookings in Query: " +
                QString::number(size) +
                "\n\nAverage score conceded by team with sin binned player: " + QString::number(avgCon) +
                "\n\nAverage score scored by team with sin binned player: " + QString::number(avgScor) +
                "\n\nAverage change in score during a sin bin: " + QString::number(avgDiff);
    return result;
}

QString DataAnalyser::compare(QString str1, QString str2)
{
    QString result = "Comparison of Data Sets\n\n" + str1 + "\n\n\n" + str2;
    return result;
}

