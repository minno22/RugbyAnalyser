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

    void on_btnData1_clicked();

    void on_btnData2_clicked();

    void on_btnCompare_clicked();

private:
    Ui::KeyEventOptions *ui;

    void populateDDL();
    QueryBuilder qb;
    QueryDB qdb;
    DataAnalyser da;
    ResultsWindow rw;
    QStringList events, comps, withins, times;
    DataAnalyser::dataSet Set1, Set2;
    QVector <int> arr;

    int getNumMatches(QString comp);
    QStringList getComps();
    void setDataSet(int num);
    QString analyse(DataAnalyser::dataSet data);
};

#endif // KEYEVENTOPTIONS_H
