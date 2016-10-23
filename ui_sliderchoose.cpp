#include "ui_sliderchoose.h"
#include "ui_ui_sliderchoose.h"

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

void ui_sliderchoose::on_ok_button_clicked()
{
    float value = ui->degree->text().toFloat();
    if (value<=-90||value>=90) value = 0 ;
    emit s_ok_value(value);
    this->close();
}

void ui_sliderchoose::on_cancel_button_clicked()
{
    emit s_value(0);
    this->close();
}

void ui_sliderchoose::on_degree_textEdited(const QString &arg1)
{
    float value = ui->degree->text().toFloat();
    if (value<=-90||value>=90) value = 0 ;
    emit s_value(value);
}
