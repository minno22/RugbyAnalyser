#ifndef KEYEVENTOPTIONS_H
#define KEYEVENTOPTIONS_H

#include <QDialog>

namespace Ui {
class KeyEventOptions;
}

class KeyEventOptions : public QDialog
{
    Q_OBJECT

public:
    explicit KeyEventOptions(QWidget *parent = 0);
    ~KeyEventOptions();

private:
    Ui::KeyEventOptions *ui;
};

#endif // KEYEVENTOPTIONS_H
