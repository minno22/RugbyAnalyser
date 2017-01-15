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

void ViewDb::displayTable(int index)
{
    QString query = qb.getQuery(index, "", 0, 0);
    qdb.setQuery(query);
    QSqlQuery qry = qdb.executeQuery();
    QString result;
    while (qry.next()) {
        QString row;
        for (int i = 0; i < 7; i++){
            row += qry.value(i).toString() + "  ";
        }
        result += row + "\n";
    }
    ui->textBrowser->setText(result);
}

void ViewDb::populateDDL()
{
    QStringList tables;
    tables << "Match" << "Try" << "Penalty" << "Drop Goal" << "Booking";
    ui->cbTable->insertItems(0, tables);
}
