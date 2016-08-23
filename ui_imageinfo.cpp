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

void ui_imageinfo::r_imageshort(cv::Mat_<unsigned short> a)
{
    ui->width->setText(QString::number(a.cols));
    ui->height->setText(QString::number(a.rows));
}
