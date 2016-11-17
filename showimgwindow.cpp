#include "showimgwindow.h"
#include "ui_showimgwindow.h"

showimgwindow::showimgwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showimgwindow)
{
    ui->setupUi(this);
    ui->showcontainer->resize(this->size());
}

showimgwindow::~showimgwindow()
{
    delete ui;
}

void showimgwindow::resizeEvent(QResizeEvent * event)
{
    ui->showcontainer->resize(this->size());
    showimg();
}

void showimgwindow::closeEvent(QCloseEvent *event)
{
    emit s_bigshow_close();
}

void showimgwindow::r_img(QImage img)
{
    qshowimg = img;
    showimg();
}
void showimgwindow::showimg()
{
    QGraphicsDropShadowEffect *e1=new QGraphicsDropShadowEffect;
    e1->setColor(QColor(0,0,0));
    e1->setBlurRadius(10);
    e1->setOffset(1,1);
    ui->showimg->setGraphicsEffect(e1);
    QImage tshowimg  = qshowimg.scaled(ui->showcontainer->width(),ui->showcontainer->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->showimg->resize(ui->showcontainer->size());
    ui->showimg->setPixmap(QPixmap::fromImage(tshowimg));
}
