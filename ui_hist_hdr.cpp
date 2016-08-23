#include "ui_hist_hdr.h"
#include "ui_ui_hist_hdr.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include "QMutex"

using namespace cv;
using namespace std;

extern int hist_hdr_div_w;
extern int hist_hdr_div_h;

#define PRECISION (256)
static QMutex mm;
static float cdf[PRECISION], cdf2[PRECISION], mincdf, mincdf2, mincdf3;
static int cnt=0;
static float tpdf[PRECISION] = { 0 };
static cv::Mat img;


void pregetCDF(Mat input, float cdf[PRECISION], float &mincdf)
{
    for (int k = 0; k < PRECISION; k++)
    {
        tpdf[k]=0;
    }
    input.convertTo(input, CV_16UC1, (PRECISION - 1));
    for (int ii = 0; ii < input.rows; ii++)
        for (int jj = 0; jj < input.cols; jj++)
        {
            unsigned short b = input.at<unsigned short>(ii, jj);
            tpdf[b]++;
        }
}

void getCDF(Mat input, float cdf[PRECISION], float &mincdf, float _anti_constrast)
{

    float pdf[PRECISION] = { 0 };
    float clip_level = _anti_constrast;
    float excess = 0.0;
    float excess_avg = 0.0;

    int non_zero_hist = PRECISION;
    for (int k = 0; k < PRECISION; k++)
    {
        pdf[k]=tpdf[k];
        pdf[k] /= (input.rows * input.cols);
        if (pdf[k] > clip_level) excess += (pdf[k] - clip_level);
    }


    excess_avg = excess / non_zero_hist;
    for (int k = 0; k < PRECISION; k++)
    {
        if (pdf[k] > clip_level) pdf[k] = clip_level;
        else if (pdf[k] > clip_level - excess_avg) {
            excess -= clip_level - pdf[k];
            pdf[k] = clip_level;
        }
        else{
            excess -= excess_avg;
            pdf[k] += excess_avg;
        }
    }
    excess_avg = excess / non_zero_hist;

    float min = 1.0;
    for (int k = 0; k < PRECISION; k++)
    {
        pdf[k] += excess_avg;
        if (min > pdf[k]) min = pdf[k];
    }
    mincdf = min;

    cdf[0] = pdf[0];
    for (int k = 1; k < PRECISION; k++)
    {
        cdf[k] = cdf[k - 1] + pdf[k];
    }
}


float clahe2D(float brightness, float cdf[PRECISION])
{
    float b1 = cdf[(int)round(brightness * (PRECISION - 1))];
    return (b1 - mincdf) * mincdf2;
}

void divcalc(Mat_<unsigned char> &dst,int pos0,int pos1,int pos2,int pos3)
{

    for (int i=pos0;i<=pos1;i++)
    {
        for (int j=pos2;j<=pos3;j++)
        {
            dst.at<uchar>(i, j) = (uchar)round(cdf2[img.at<int>(i, j)] - mincdf3);
        }
    }
    mm.lock();
    cnt++;
    mm.unlock();
}

void ui_hist_hdr::histogramTonemapping(Mat img, Mat_<unsigned char> &dst, float _contrast, float _anti_constrast)
//void ui_hist_hdr::histogramTonemapping(Mat img, Mat_<unsigned char> & dst, float _contrast = 1.0f, float _anti_constrast = 0.01f)
{
    getCDF(img, cdf, mincdf, _anti_constrast);
    mincdf2 = 255 / (1.0 - mincdf);
    for (int i = 0; i < PRECISION ; i++)
    {
        cdf2[i] = cdf[i] * mincdf2;
    }
    mincdf3 = mincdf2 * mincdf;
    cnt=0;
    int position[4];
    int di=img.rows/hist_hdr_div_h;
    int dj=img.cols/hist_hdr_div_w;
    for (int iw=0;iw<hist_hdr_div_w;iw++)
    {
        for (int ih=0;ih<hist_hdr_div_h;ih++)
        {
            position[0]=(ih==0?0:(ih*di+1));
            position[1]=(ih==(hist_hdr_div_h-1)?(img.rows-1):(ih+1)*di);
            position[2]=(iw==0?0:(iw*dj+1));
            position[3]=(iw==(hist_hdr_div_w-1)?(img.cols-1):(iw+1)*dj);
            QtConcurrent::run(divcalc,dst,position[0],position[1],position[2],position[3]);
        }
    }
    while(true)
    {
        mm.lock();
        if (cnt==hist_hdr_div_w*hist_hdr_div_h)
        {
            cnt=0;
            mm.unlock();
            break;
        }
        mm.unlock();
    }
    emit s_imagechar(dst);

}


ui_hist_hdr::ui_hist_hdr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_hist_hdr)
{
    ui->setupUi(this);
}

ui_hist_hdr::~ui_hist_hdr()
{
    delete ui;
}
//初始
void ui_hist_hdr::r_imageshort(cv::Mat_<unsigned short> a)
{
    srcimg=a;
    value=float(1)/400;
    outputimg = Mat(srcimg.size(), CV_8UC1);
    srcimg.convertTo(img, CV_32F);
    double min, max;
    minMaxLoc(img, &min, &max);
    if (max - min > DBL_EPSILON) {
        img = (img - min) / (max - min);
    }
    else
    {
       return;
    }
    pregetCDF(img, cdf, mincdf);
    img = img * (PRECISION - 1);
    img.convertTo(img, CV_32SC1);
    ui_hist_hdr::histogramTonemapping(img, outputimg, 1, value);

}
//变化处理
void ui_hist_hdr::on_horizontalSlider_valueChanged(int position)
{
    ui->shownum->setText(QString::number(position));
    value=float(position)/ 400;
    ui_hist_hdr::histogramTonemapping(img, outputimg, 1, value);
}

void ui_hist_hdr::on_minus_clicked()
{
    int position=ui->shownum->text().toInt()-1;
    position=position>0?position:1;
    position=position<101?position:100;
    ui->horizontalSlider->setValue(position);
}


void ui_hist_hdr::on_plus_clicked()
{
    int position=ui->shownum->text().toInt()+1;
    position=position>0?position:1;
    position=position<101?position:100;
    ui->horizontalSlider->setValue(position);
}

void ui_hist_hdr::on_shownum_editingFinished()
{
    int position=ui->shownum->text().toInt();
    position=position>0?position:1;
    position=position<101?position:100;
    ui->horizontalSlider->setValue(position);
//    value=float(pow(2.0,position))/100;
//    cv::Mat_<unsigned char> outputimg;
//    histogramTonemapping(srcimg, outputimg, 1, value);

}

void ui_hist_hdr::on_ok_button_clicked()
{

}
void ui_hist_hdr::closeEvent(QCloseEvent *)
{
    emit s_cancel();
}

void ui_hist_hdr::on_cancel_button_clicked()
{
    emit s_cancel();
    this->close();
}





