#include "keyeventoptions.h"
#include "ui_keyeventoptions.h"


KeyEventOptions::KeyEventOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyEventOptions)
{
    ui->setupUi(this);
    populateDDL();
}

KeyEventOptions::~KeyEventOptions()
{
    delete ui;
}

void KeyEventOptions::analyseStories()
{
    QApplication::setOverrideCursor(Qt::WaitCursor); //changes mouse icon to wait
    result = "";
    int num = getNumMatches("All Competitions"); //get number of matches in database
    QVector <QString> queries;
    for (int i = 1; i < 4; i++){ //loop to get required sql queries
        QString query = qb.getQuery(i, "All", 0, 0, 0); //gets sql query
        queries.push_back(query); //add query to vector of queries
    }
    da.analyseStories(num, queries, &result); //calls data analyser to analyse game stories and generate an output string
    QApplication::restoreOverrideCursor(); //resets mouse icon
    displayResults(1);
}

void KeyEventOptions::on_btnAnalyse_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor); //changes mouse icon to wait
    result = "";
    ds.event = ui->cbEvent->currentIndex();
    ds.comp = ui->cbComp->currentText();
    ds.compare = ui->cbCompare->currentText();
    ds.within = ui->cbWithin->currentIndex();
    ds.time = ui->cbTime->currentIndex();

    da.resetArray(1); //Set all indexes of array to zero
    analyse(); // pass selected analysis options to analyse to select appropriate analysis function/s
    QApplication::restoreOverrideCursor(); //resets mouse icon
    displayResults(0);
}

void KeyEventOptions::displayResults(int num) //configure and display results window object
{
    qDebug() << result << endl;
    rw.setText(result); //add the result text to the window
    if (ds.event == 0)
        da.setPieChartMatches();
    else if (ds.event == 5)
        da.setScoresChart();
    QChartView *chartView = da.getChart(); //get chart from data analyser
    if (num == 0 && ds.event != 0 && ds.event != 5){ //if more than one chart to be added to window
        da.setChart1(events.at(ds.event)); //configure chart in data analyser
        QChartView *chartView0 = da.getChart1(); //get chart from data analyser
        rw.setGraph0(chartView0, chartView); //add graph to window
    }
    else
        rw.setGraph1(chartView); //add graph to window
    rw.show(); //display window
}

void KeyEventOptions::populateDDL() //configure the drop down lists in the key event options window
{
    //add strings/options to QStringLists
    events << "Matches" << "Tries" << "Penalties" << "Drop Goals" << "Bookings" << "All Scores";
    comps << "All Competitions" << getComps();
    comps2 << "None" << getComps();
    withins << "All Time" << "Last Week" << "Last 30 Days" << "Last 6 Months" << "Last Year";
    times << "Full Match" << "First Half" << "Second Half" << "0-20 Minutes" << "20-40 Minutes"
            << "40-60 Minutes" << "60-80 Minutes";
    //add QStringLists to drop down lists
    ui->cbEvent->insertItems(0, events);
    ui->cbComp->insertItems(0, comps);
    ui->cbCompare->insertItems(0, comps2);
    ui->cbWithin->insertItems(0, withins);
    ui->cbTime->insertItems(0, times);
}

int KeyEventOptions::getNumMatches(QString comp) //get the number of matches in a particular competition
{
    int no = 0, oldNo, newNo;
    QString query = qb.getQuery(0, comp, 0, 0, 0);  //get the sql query from query builder object
    qdb.setQuery(query); //set the sql query in the query DB object
    QSqlQuery qry = qdb.executeQuery(); //execute the query in the query DB object to obtain QSqlQuery object
    while (qry.next()) {
        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            no++;
            oldNo = newNo;
        }
    }
    return no;
}

QStringList KeyEventOptions::getComps() //get list of competitions from database
{
    QStringList comps;
    QString query = qb.getUnique(0); //get the sql query string
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    while (qry.next()) {
       QString comp = qry.value(0).toString();
       bool found = false;
       for (int i = 0; i < comps.size() && !found; i++) //ensure no duplicate strings
           if(comps.at(i) == comp)
               found = true;
       if (!found){
           comps << comp;
       }
    }
    return comps;
}

void KeyEventOptions::analyse()
{
    QString comp;
    int iterations = 1;
    if (ds.compare != "None"){
        iterations = 2;
    }

    for (int i = 0; i < iterations; i++){
        if (i == 0)
            comp = ds.comp;
        else
            comp = ds.compare;
        result += comp + ": ";
        int num = getNumMatches(comp);
        QString query;
        QSqlQuery qry;

        switch(ds.event){
        case 0: { //analyse matches
            for (int i = 0; i < 5; i++){
                query = qb.getQuery(i, comp, ds.within, ds.time, 0);
                qdb.setQuery(query);
                qry = qdb.executeQuery();
                da.resetArray(0);
                da.analyse(qry, i, num, &result);
                result += "\n\n\n";
                qry.clear();
            }
        }break; //analyse bookings
        case 4: {
            QVector <QString> queries;
            query = qb.getQuery(4, comp, ds.within, ds.time, 0);
            queries.push_back(query);
            for (int i = 1; i < 4; i++){
                query = qb.getQuery(i, comp, ds.within, ds.time, 1);
                queries.push_back(query);
            }
            result += "\n\n";
            da.analyseConditions(num, queries, &result);
        }break;
        case 5: { //analyse all scores
            for (int i = 1; i < 4; i++){
                query = qb.getQuery(i, comp, ds.within, ds.time, 0);
                qdb.setQuery(query);
                qry = qdb.executeQuery();
                da.resetArray(0);
                da.analyse(qry, i, num, &result);
                result += "\n\n";
                qry.clear();
            }
        }break;
        default: { //penalties, tries, drop goals
            query = qb.getQuery(ds.event, comp, ds.within, ds.time, 0);
            qdb.setQuery(query);
            qry = qdb.executeQuery();
            da.analyse(qry, ds.event, num, &result);
            result += "\n\n";
        }break;
        }
    }
}

void KeyEventOptions::on_cbEvent_currentIndexChanged(int index)
{
    if (index != 0)
        ui->cbTime->setEnabled(true);
    else{
        ui->cbTime->setCurrentIndex(0);
        ui->cbTime->setEnabled(false);
    }
}
