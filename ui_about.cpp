#include "ui_about.h"
#include "ui_ui_about.h"

ui_about::ui_about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_about)
{
    ui->setupUi(this);
}

ui_about::~ui_about()
{
    delete ui;
}
