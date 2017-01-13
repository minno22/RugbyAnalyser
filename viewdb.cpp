#include "viewdb.h"
#include "ui_viewdb.h"

ViewDb::ViewDb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewDb)
{
    ui->setupUi(this);
}

ViewDb::~ViewDb()
{
    delete ui;
}
