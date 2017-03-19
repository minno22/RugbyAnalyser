#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include "keyeventoptions.h"
#include "viewdb.h"

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = 0);
    ~MenuWindow();

private slots:
    void on_btnKeyEventAnalysis_clicked();

    void on_btnViewDb_clicked();

    void on_btnUpdateDb_clicked();

    void on_btnStories_clicked();

private:
    Ui::MenuWindow *ui;
    KeyEventOptions keo;
    ViewDb vdb;
};

#endif // MENUWINDOW_H
