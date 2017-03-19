#include "menuwindow.h"
#include "ui_menuwindow.h"

#include <QtSql>
#include <QDebug>

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::on_btnKeyEventAnalysis_clicked()
{
    keo.show();
}

void MenuWindow::on_btnViewDb_clicked()
{
    vdb.setup(0);
    vdb.show();
}

void MenuWindow::on_btnUpdateDb_clicked()
{
    system("C:\\Users\\Niall\\Documents\\LM051\\YR4\\FYP\\Project\\RugbyAnalyser\\Scripts\\UpdateDB.py");
}

void MenuWindow::on_btnStories_clicked()
{
    keo.analyseStories();
}
