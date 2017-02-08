#include "resultswindow.h"
#include "ui_resultswindow.h"
#include <QDebug>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

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

void ResultsWindow::setText(QString text)
{
    ui->textBrowser->setText(text);
}

void ResultsWindow::setGraph(QVector<int> *arr, QChartView *chartView1)
{
    ui->verticalLayout->removeWidget(chartView);
    ui->verticalLayout->removeWidget(chartView2);

   /* QBarSet *set0 = new QBarSet("Event");
    QStringList categories;
    for (int i = 0; i < arr->size(); i++){
        *set0 << arr->at(i);
        QString str;
        str.append(QString::number(i) + "0-" + QString::number(i+1) + "0");
        categories << str;
    }
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Match Time Analysis");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout->addWidget(chartView);*/

    chartView2 = chartView1;
    chartView2->setRenderHint(QPainter::Antialiasing);

    ui->verticalLayout->addWidget(chartView2);
}
