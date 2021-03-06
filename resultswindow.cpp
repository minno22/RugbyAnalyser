#include "resultswindow.h"
#include "ui_resultswindow.h"
#include <QDebug>

ResultsWindow::ResultsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsWindow)
{
    ui->setupUi(this);
}

ResultsWindow::~ResultsWindow()
{
    delete ui;
    delete this;
}

void ResultsWindow::setText(QString text) //set the output string as the text
{
    ui->textBrowser->setText(text);
}

void ResultsWindow::setGraph0(QChartView *chartView0, QChartView *chartView1) //set two charts
{
    ui->verticalLayout->removeWidget(chartView);
    ui->verticalLayout->removeWidget(chartView2);

    chartView = chartView0;
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(chartView);
    chartView2 = chartView1;
    chartView2->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout->addWidget(chartView2);
}

void ResultsWindow::setGraph1(QChartView *chartView0) //set one chart
{
    ui->verticalLayout->removeWidget(chartView);
    ui->verticalLayout->removeWidget(chartView2);

    chartView = chartView0;
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(chartView);
}
