#include "querybuilder.h"
#include <QDebug>
#include <ctime>

QueryBuilder::QueryBuilder()
{

}

QString QueryBuilder::getQuery(int event, QString comp, int within, int time)
{
    //SELECT * FROM Try JOIN Match ON Try.MatchID = Match.MatchID WHERE Competition LIKE 'Pro 12'

    QString select, table, join = "", where = "", yymmdd = "", timeStr = "";

    select = "SELECT * FROM Match";

    if (event > 0){
        switch (event) {
        case 1: table = "Try";
            break;
        case 2: table = "Penalty";
            break;
        case 3: table = "DropGoal";
            break;
        case 4: table = "Booking";
            break;
        default:
            break;
        }
        join = " JOIN " + table + " ON Match.MatchID = " + table + ".MatchID";
    }

    if (comp != "All")
        where = " WHERE Competition LIKE '" + comp + "'";

    if (within > 0)
        yymmdd = setDate(within);

    if (time > 0)
        timeStr = setTime(time);

    query = select + join + where + yymmdd + timeStr;
    qDebug() << query;
    return query;
}

QString QueryBuilder::setDate(int idx)
{
    QString yymmdd = " AND MatchDate >= '";

    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);

    struct tm date = { 0, 0, 12 } ;

    // Set up the date structure
    date.tm_year = ltm->tm_year;
    date.tm_mon = ltm->tm_mon;  // note: zero indexed
    date.tm_mday = ltm->tm_mday;       // note: not zero indexed

    int year, month, day;

    switch (idx) {
    case 1: DatePlusDays( &date, -7 ) ;
        break;
    case 2: DatePlusDays( &date, -30 ) ;
        break;
    case 3: DatePlusDays( &date, -182 ) ;
        break;
    case 4: DatePlusDays( &date, -365 ) ;
        break;
    default:
        break;
    }
    year = date.tm_year + 1900;
    month = date.tm_mon + 1;
    day = date.tm_mday;
    yymmdd += QString::number(year) + "-" + QString::number(month) + "-" + QString::number(day) + "'";

    return yymmdd;
}

QString QueryBuilder::setTime(int idx)
{
    QString time = " AND TimeInMatch ";
    switch (idx) {
    case 1: time += "<= 40" ;
        break;
    case 2: time += "> 40" ;
        break;
    case 3: time += "< 20" ;
        break;
    case 4: time += ">= 20 AND TimeInMatch < 40" ;
        break;
    case 5: time += ">= 40 AND TimeInMatch < 60" ;
        break;
    case 6: time += "> 60" ;
        break;
    default:
        break;
    }
    return time;
}

QString QueryBuilder::maxId()
{
    return "SELECT MAX(MatchID) FROM Match";
}

QString QueryBuilder::getUnique(int idx)
{
    return "SELECT Competition FROM Match";
}

//taken from http://stackoverflow.com/questions/2344330/algorithm-to-add-or-subtract-days-from-a-date
// Adjust date by a number of days +/-
void QueryBuilder::DatePlusDays( struct tm* date, int days )
{
    const time_t ONE_DAY = 24 * 60 * 60 ;

    // Seconds since start of epoch
    time_t date_seconds = mktime( date ) + (days * ONE_DAY) ;

    // Update caller's date
    // Use localtime because mktime converts to UTC so may change date
    *date = *localtime( &date_seconds ) ; ;
}

