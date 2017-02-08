#include "keyeventoptions.h"
#include "ui_keyeventoptions.h"


KeyEventOptions::KeyEventOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyEventOptions)
{
    ui->setupUi(this);
    populateDDL();
    for (int i = 0; i < 10; i++)
        arr.append(0);
}

KeyEventOptions::~KeyEventOptions()
{
    delete ui;
}

void KeyEventOptions::on_btnAnalyse_clicked()
{
    DataAnalyser::dataSet ds;
    ds.event = ui->cbEvent->currentIndex();
    ds.comp = ui->cbComp->currentText();
    ds.within = ui->cbWithin->currentIndex();
    ds.time = ui->cbTime->currentIndex();

    QString result = analyse(ds);

    qDebug() << result << endl;
    rw.setText(result);
    QChartView *chartView = da.getChart();
    rw.setGraph(&arr, chartView);
    rw.show();
}

void KeyEventOptions::populateDDL()
{
    events << "Matches" << "Tries" << "Penalties" << "Drop Goals" << "Bookings" << "All Scores" << "Game Stories";
    comps = getComps();
    withins << "All Time" << "Last Week" << "Last 30 Days" << "Last 6 Months" << "Last Year";
    times << "Full Match" << "First Half" << "Second Half" << "0-20 Minutes" << "20-40 Minutes"
            << "40-60 Minutes" << "60-80 Minutes";
    ui->cbEvent->insertItems(0, events);
    ui->cbComp->insertItems(0, comps);
    ui->cbWithin->insertItems(0, withins);
    ui->cbTime->insertItems(0, times);
}

int KeyEventOptions::getNumMatches(QString comp)
{
    int no = 0, oldNo, newNo;
    QString query = qb.getQuery(0, comp, 0, 0, 0);
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    while (qry.next()) {
        newNo = qry.value(0).toInt();
        if (newNo != oldNo){
            no++;
            oldNo = newNo;
        }
    }
    return no;
}

QStringList KeyEventOptions::getComps()
{
    QStringList comps;
    comps << "All";
    QString query = qb.getUnique(0);
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    while (qry.next()) {
       QString comp = qry.value(0).toString();
       bool found = false;
       for (int i = 0; i < comps.size() && !found; i++)
           if(comps.at(i) == comp)
               found = true;
       if (!found){
           comps << comp;
       }
    }
    return comps;
}

void KeyEventOptions::setDataSet(int num)
{
    int event = ui->cbEvent->currentIndex();
    QString comp = ui->cbComp->currentText();
    int within = ui->cbWithin->currentIndex();
    int time = ui->cbTime->currentIndex();
    QString output = events.at(event) + "\n\n" + comp + "\n\n" + withins.at(within) + "\n\n" + times.at(time)
                        + "\n\n";

    if (num == 1){
        Set1.event = event;
        Set1.comp = comp;
        Set1.within = within;
        Set1.time = time;
        Set1.set = true;

        ui->tbData1->setText(output);
    }
    else{
        Set2.event = event;
        Set2.comp = comp;
        Set2.within = within;
        Set2.time = time;
        Set2.set = true;

        ui->tbData2->setText(output);
    }
}

QString KeyEventOptions::analyse(DataAnalyser::dataSet data)
{
    int num = getNumMatches(data.comp);
    QString query, result;
    QSqlQuery qry;

    switch(data.event){
    case 0: {
        for (int i = 0; i < 5; i++){
            query = qb.getQuery(i, data.comp, data.within, data.time, 0);
            qdb.setQuery(query);
            qry = qdb.executeQuery();
            result += da.analyse(qry, i, num, &arr) + "\n\n";
            qry.clear();
        }
    }break;
    case 4: {
        QVector <QString> queries;
        query = qb.getQuery(4, data.comp, data.within, data.time, 0);
        queries.push_back(query);
        for (int i = 1; i < 4; i++){
            query = qb.getQuery(i, data.comp, data.within, data.time, 1);
            queries.push_back(query);
        }
        result = da.analyseConditions(num, queries, &arr) + "\n\n";
    }break;
    case 5: {
        for (int i = 1; i < 4; i++){
            query = qb.getQuery(i, data.comp, data.within, data.time, 0);
            qdb.setQuery(query);
            qry = qdb.executeQuery();
            result += da.analyse(qry, i, num, &arr) + "\n\n";
            qry.clear();
        }
    }break;
    case 6: {
        QVector <QString> queries;
        for (int i = 1; i < 4; i++){
            query = qb.getQuery(i, data.comp, data.within, data.time, 0);
            queries.push_back(query);
        }
        result = da.analyseStories(num, queries);
    }break;
    default: {
        query = qb.getQuery(data.event, data.comp, data.within, data.time, 0);
        qdb.setQuery(query);
        qry = qdb.executeQuery();
        result = da.analyse(qry, data.event, num, &arr);
    }break;
    }

    return result;
}

void KeyEventOptions::on_btnData1_clicked()
{   
    setDataSet(1);
}

void KeyEventOptions::on_btnData2_clicked()
{
    setDataSet(2);
}

void KeyEventOptions::on_btnCompare_clicked()
{
    if (Set1.set && Set2.set){
        if (Set1.event != Set2.event)
            qDebug() << "Cannot compare " + events.at(Set1.event) + " with " + events.at(Set2.event) << endl;
        else{
            qDebug() << "ok to compare" << endl;
            QString first = analyse(Set1);
            QString second = analyse(Set2);
            QString result = da.compare(first, second);

            rw.setText(result);
            rw.show();
        }
    }
    else
        qDebug() << "Both data sets must be set for comparison" << endl;
}
