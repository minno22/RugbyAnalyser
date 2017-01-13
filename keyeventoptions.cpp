#include "keyeventoptions.h"
#include "ui_keyeventoptions.h"

KeyEventOptions::KeyEventOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyEventOptions)
{
    ui->setupUi(this);
}

KeyEventOptions::~KeyEventOptions()
{
    delete ui;
}
