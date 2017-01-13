#include "resultswindow.h"
#include "ui_resultswindow.h"

ResultsWindow::ResultsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsWindow)
{
    ui->setupUi(this);
}

ResultsWindow::~ResultsWindow()
{
    delete ui;
}
