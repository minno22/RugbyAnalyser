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

    void analyseStories();

private slots:
    void on_btnAnalyse_clicked();

    void on_cbEvent_currentIndexChanged(int index);

private:
    Ui::KeyEventOptions *ui;

    struct dataSet{
        int event;
        QString comp;
        QString compare;
        int within;
        int time;
        bool set = false;
    } ds;

    void populateDDL();
    QueryBuilder qb;
    QueryDB qdb;
    DataAnalyser da;
    ResultsWindow *rw;
    QStringList events, comps, comps2, withins, times;
    QString result;

    int getNumMatches(QString comp);
    QStringList getComps();
    void analyse();
    void displayResults(int num);
};

#endif // KEYEVENTOPTIONS_H
