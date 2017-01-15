#ifndef KEYEVENTOPTIONS_H
#define KEYEVENTOPTIONS_H

#include <QDialog>
#include "querybuilder.h"
#include "querydb.h"
#include "resultswindow.h"
#include "dataanalyser.h"

namespace Ui {
class KeyEventOptions;
}

class KeyEventOptions : public QDialog
{
    Q_OBJECT

public:
    explicit KeyEventOptions(QWidget *parent = 0);
    ~KeyEventOptions();


private slots:
    void on_btnAnalyse_clicked();

private:
    Ui::KeyEventOptions *ui;
    void populateDDL();
    QueryBuilder qb;
    QueryDB qdb;
    DataAnalyser da;
    ResultsWindow rw;
    int getNumMatches();
    QStringList getComps();
};

#endif // KEYEVENTOPTIONS_H
