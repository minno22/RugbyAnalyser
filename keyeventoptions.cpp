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


void KeyEventOptions::on_btnAnalyse_clicked()
{
    int event = ui->cbEvent->currentIndex();
    QString comp = ui->cbComp->currentText();
    int within = ui->cbWithin->currentIndex();
    int time = ui->cbTime->currentIndex();
    qDebug() << within << endl;
    QString query = qb.getQuery(event, comp, within, time);
    qdb.setQuery(query);
    int num = getNumMatches();
    QSqlQuery qry = qdb.executeQuery();
    QString result = da.analyse(qry, event, num);
    qDebug() << result << endl;
    rw.setText(result);
    rw.show();
}

void KeyEventOptions::populateDDL()
{
    QStringList events, comps, within, time;
    events << "Matches" << "Tries" << "Penalties" << "Drop Goals" << "Bookings";
    comps = getComps();
    within << "All Time" << "Last Week" << "Last 30 Days" << "Last 6 Months" << "Last Year";
    time << "Full Match" << "First Half" << "Second Half" << "0-20 Minutes" << "20-40 Minutes"
            << "40-60 Minutes" << "60-80 Minutes";
    ui->cbEvent->insertItems(0, events);
    ui->cbComp->insertItems(0, comps);
    ui->cbWithin->insertItems(0, within);
    ui->cbTime->insertItems(0, time);
}

int KeyEventOptions::getNumMatches()
{
    int no = 0, oldNo, newNo;
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
