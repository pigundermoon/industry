#include "ui_emboss.h"
#include "ui_ui_emboss.h"

ui_emboss::ui_emboss(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_emboss)
{
    ui->setupUi(this);
}

ui_emboss::~ui_emboss()
{
    delete ui;
}

void ui_emboss::on_minus_clicked()
{
    int position=ui->dis->value()-1;
    position=position<ui->dis->maximum()?position:ui->dis->maximum();
    position=position>ui->dis->minimum()?position:ui->dis->minimum();
    ui->dis->setValue(position);
}

void ui_emboss::on_plus_clicked()
{
    int position=ui->dis->value()+1;
    position=position<ui->dis->maximum()?position:ui->dis->maximum();
    position=position>ui->dis->minimum()?position:ui->dis->minimum();
    ui->dis->setValue(position);
}

void ui_emboss::on_cminus_clicked()
{
    int position=ui->contrast->value()-1;
    position=position<ui->contrast->maximum()?position:ui->contrast->maximum();
    position=position>ui->contrast->minimum()?position:ui->contrast->minimum();
    ui->contrast->setValue(position);
}

void ui_emboss::on_cplus_clicked()
{
    int position=ui->contrast->value()+1;
    position=position<ui->contrast->maximum()?position:ui->contrast->maximum();
    position=position>ui->contrast->minimum()?position:ui->contrast->minimum();
    ui->contrast->setValue(position);
}

void ui_emboss::on_lineEdit_editingFinished()
{
    int position=ui->lineEdit->text().toInt();
    position=position<ui->dis->maximum()?position:ui->dis->maximum();
    position=position>ui->dis->minimum()?position:ui->dis->minimum();
    ui->dis->setValue(position);
}

void ui_emboss::on_clineEdit_editingFinished()
{
    int position=ui->clineEdit->text().toInt();
    position=position<ui->contrast->maximum()?position:ui->contrast->maximum();
    position=position>ui->contrast->minimum()?position:ui->contrast->minimum();
    ui->contrast->setValue(position);
}

void ui_emboss::on_dis_valueChanged(int value)
{
    ui->lineEdit->setText(QString::number(value));
    emit s_value(ui->dis->value(),ui->contrast->value());
}

void ui_emboss::on_contrast_valueChanged(int value)
{
    ui->clineEdit->setText(QString::number(value));
    emit s_value(ui->dis->value(),ui->contrast->value());
}

void ui_emboss::on_ok_button_clicked()
{
    emit s_ok_value(ui->dis->value(),ui->contrast->value());
    this->close();
}


void ui_emboss::on_cancel_button_clicked()
{
    emit s_cancel();
    this->close();
}
