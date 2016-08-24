#include "ui_imageinfo.h"
#include "ui_ui_imageinfo.h"
#include "opencv2/opencv.hpp"
#include "QString"

ui_imageinfo::ui_imageinfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_imageinfo)
{
    ui->setupUi(this);
}

ui_imageinfo::~ui_imageinfo()
{
    delete ui;
}

void ui_imageinfo::r_imageinfo(recdcmtkfile *file)
{
    ui->width->setText(QString::number(file->width));
    ui->height->setText(QString::number(file->height));
    if (file->flag)
    {
        ui->name->setText(file->name);
        ui->id->setText(file->id);
        ui->date->setText(file->date);
        ui->time->setText(file->time);
    }
}
