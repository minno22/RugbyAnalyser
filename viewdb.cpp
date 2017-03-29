#include "viewdb.h"
#include "ui_viewdb.h"

ViewDb::ViewDb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewDb)
{
    ui->setupUi(this);

    populateDDL();

    connect(ui->cbTable, SIGNAL(currentIndexChanged(int)), SLOT(displayTable(int)));
}

ViewDb::~ViewDb()
{
    delete ui;
}

void ViewDb::setup(int idx)
{
    displayTable(idx);
}

void ViewDb::displayTable(int index) //display database table as string
{
    QApplication::setOverrideCursor(Qt::WaitCursor); //changes mouse icon to wait
    QString query = qb.getQuery(index, "All", 0, 0, 0);
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    QString result;
    switch (index){
    case 0: result += "Match ID\tMatch Date\t\tHome Team\t\t\tAway Team\t\t\tCompetition\t\t\tHome Score\tAway Score\n\n\n";
        break;
    case 1: result += "Try ID\tMatch ID\tTime In Match\tTeam\tConverted\tPenalty Try\n\n\n"; break;
    case 2: result += "Penalty ID\tMatch ID\tTime In Match\tTeam\n\n\n"; break;
    case 3: result += "Drop Goal ID\tMatch ID\tTime In Match\tTeam\n\n\n"; break;
    case 4: result += "Booking ID\tMatch ID\tTime In Match\tTeam\tColour\n\n\n"; break;
    }

    while (qry.next()) {
        QString row;
        switch (index){
        case 0: {
            for (int i = 0; i < 7; i++){
                if (i == 1)
                    row += (qry.value(i).toString()).remove(10, 12) + "\t\t";
                else if (i > 1 && i < 5){
                    QString str = qry.value(i).toString();
                    str = str.leftJustified(40, '.');
                    //str.resize(50, ' ');
                    //str.append("X");
                    row += str + "\t";
                }
                else
                    row += qry.value(i).toString() + "\t";
            }
            result += row + "\n"; }
            break;
        case 1: {
            for (int i = 10; i < 16; i++){
                row += qry.value(i).toString() + "\t";
                if (i == 12)
                    row += "\t";
            }
            result += row + "\n"; }
            break;
        case 2: case 3: {
            for (int i = 10; i < 14; i++){
                row += qry.value(i).toString() + "\t";
                if (i == 12)
                    row += "\t";
            }
            result += row + "\n"; }
            break;
        case 4: {
            for (int i = 10; i < 15; i++){
                row += qry.value(i).toString() + " \t";
                if (i == 12)
                    row += "\t";
            }
            result += row + "\n"; }
            break;
        }
    }
    QApplication::restoreOverrideCursor(); //resets mouse icon
    ui->textBrowser->setText(result);
}

void ViewDb::populateDDL() //configure the drop down list
{
    QStringList tables;
    tables << "Match" << "Try" << "Penalty" << "Drop Goal" << "Booking";
    ui->cbTable->insertItems(0, tables);
}
