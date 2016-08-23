#include "processing.h"
#include "ui_processing.h"

processing::processing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::processing)
{
    ui->setupUi(this);

}

processing::~processing()
{
    delete ui;
}


void processing::r_number(int num)
{
    this->ui->process->setValue(num);
    if (num==100)
        this->close();
}
