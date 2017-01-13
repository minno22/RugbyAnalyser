#ifndef VIEWDB_H
#define VIEWDB_H

#include <QDialog>

namespace Ui {
class ViewDb;
}

class ViewDb : public QDialog
{
    Q_OBJECT

public:
    explicit ViewDb(QWidget *parent = 0);
    ~ViewDb();

private:
    Ui::ViewDb *ui;
};

#endif // VIEWDB_H
