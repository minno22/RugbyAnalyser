#ifndef VIEWDB_H
#define VIEWDB_H

#include <QDialog>
#include "querybuilder.h"
#include "querydb.h"
#include "resultswindow.h"

namespace Ui {
class ViewDb;
}

class ViewDb : public QDialog
{
    Q_OBJECT

public:
    explicit ViewDb(QWidget *parent = 0);
    ~ViewDb();
    void setup(int idx);

private slots:
    void displayTable(int index);

private:
    Ui::ViewDb *ui;
    QueryBuilder qb;
    QueryDB qdb;
    void populateDDL();
};

#endif // VIEWDB_H
