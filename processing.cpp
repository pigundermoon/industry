#include "processing.h"
#include "ui_processing.h"
#include <iostream>
using namespace std;

processing::processing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::processing)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);

    QMovie *movie = new QMovie(":/img/img/loading");

    ui->showlabel->setMovie(movie);

    movie->start();

}

processing::~processing()
{
    delete ui;
}

void processing::r_denoise_close()
{
    emit s_denoise_close_ok();
    this->close();
}

