#include "ui_sliderchoose.h"
#include "ui_sliderchoose.h"

ui_sliderchoose::ui_sliderchoose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_sliderchoose)
{
    ui->setupUi(this);
}

ui_sliderchoose::~ui_sliderchoose()
{
    delete ui;
}
