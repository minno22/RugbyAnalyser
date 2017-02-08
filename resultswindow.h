#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ResultsWindow;
}

class ResultsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsWindow(QWidget *parent = 0);
    ~ResultsWindow();
    void setText(QString text);
    void setGraph(QVector <int> * arr, QChartView * chartView1);

private:
    Ui::ResultsWindow *ui;
    QChartView *chartView, *chartView2;
};

#endif // RESULTSWINDOW_H
