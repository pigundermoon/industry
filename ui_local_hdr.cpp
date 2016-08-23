#include "ui_local_hdr.h"
#include "ui_ui_local_hdr.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <QString>
#include "QMutex"
#include "QtConcurrent/QtConcurrent"
#include "QDateTime"
#include "iostream"

using namespace cv;
using namespace std;
extern int local_hdr_div_w;
extern int local_hdr_div_h;

void loadExposureSeq(String, vector<Mat>&, vector<float>&);

static Mat src;
static Mat map_img;
static Mat tmap_img;
static cv::Mat dst;
static cv::Mat res;
static Mat log_img;
static Mat img;
static double minv, maxv;
static QMutex sendchar;
static QMutex cntlock;
static Mat tmap;
static Mat ttmap;
static float contrast;
static float gamma;
static int cnt;


inline void log_(const Mat & src, Mat & dst)
{
    max(src, 1e-4, dst);
    log(dst, dst);
}

void linearTonemapping(float _gamma, Mat src, Mat & dst)
{
    dst = Mat(src.size(), CV_32F);

    double min, max;
    minMaxLoc(src, &min, &max);
    if (max - min > DBL_EPSILON) {
        dst = (src - min) / (max - min);
    }
    else {
        src.copyTo(dst);
    }

    pow(dst, 1.0f / _gamma, dst);
}

void mapLuminance(Mat src, Mat & dst, Mat lum, Mat new_lum, float saturation)
{
    dst = src.mul(1.0f / lum);
    pow(dst, saturation, dst);
    dst = dst.mul(new_lum);
}

void prebilateralTonemapping1(Mat src, Mat & dst, float _gamma = 1.0f, float _contrast = 4.0f, float _saturation = 1.0f, float _sigma_color = 2.0f, float _sigma_space = 2.0f)
{

    float sigma_space = _sigma_color;
    float sigma_color = _sigma_space;


    dst = Mat(src.size(), CV_32F);
    img=Mat(src.size(), CV_32F);
    linearTonemapping(1.0f, src, img); //变为0-1范围内的float形


    log_(img, log_img);

    bilateralFilter(log_img, map_img, -1, sigma_color, sigma_space);


    minMaxLoc(map_img, &minv, &maxv);
    tmap=log_img-map_img*1.0f;
    ttmap=map_img/ static_cast<float>(maxv - minv);

}
void divhdr(int pos0,int pos1,int pos2,int pos3)
{
    for (int i=pos0;i<=pos1;i++)
    {
        for (int j=pos2;j<=pos3;j++)
        {
            float tt=exp(ttmap.at<float>(i,j)*contrast+tmap.at<float>(i,j));
            res.at<uchar>(i,j)=(uchar)round(pow(tt, 1.0f / gamma)*255);
        }
    }
    cntlock.lock();
    cnt++;
    cntlock.unlock();
}

void hdrsolve(float _gamma = 1.0f, float _contrast = 4.0f, float _saturation = 1.0f)
{
    gamma = _gamma;
    float saturation = _saturation;
    contrast = _contrast;
//    mapLuminance(img, img, img, tmap_img, saturation);
    cnt=0;
    int position[4];
    int di=img.rows/local_hdr_div_h;
    int dj=img.cols/local_hdr_div_w;
    for (int iw=0;iw<local_hdr_div_w;iw++)
    {
        for (int ih=0;ih<local_hdr_div_h;ih++)
        {
            position[0]=(ih==0?0:(ih*di+1));
            position[1]=(ih==(local_hdr_div_h-1)?(img.rows-1):(ih+1)*di);
            position[2]=(iw==0?0:(iw*dj+1));
            position[3]=(iw==(local_hdr_div_w-1)?(img.cols-1):(iw+1)*dj);
            QtConcurrent::run(divhdr,position[0],position[1],position[2],position[3]);
        }
    }
    while(true)
    {
        cntlock.lock();
        if (cnt==local_hdr_div_w*local_hdr_div_h)
        {
            cnt=0;
            cntlock.unlock();
            break;
        }
        cntlock.unlock();
    }
}

ui_local_hdr::ui_local_hdr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_local_hdr)
{
    ui->setupUi(this);
}

ui_local_hdr::~ui_local_hdr()
{
    delete ui;
}
//初始
void ui_local_hdr::r_imageshort(cv::Mat_<unsigned short> a)
{
    srcimg=a;
    value1=ui->v1text->text().toFloat()/10;
    value2=ui->v2text->text().toFloat()/10;

    src=Mat(srcimg.size(), CV_32F);
    dst=Mat(srcimg.size(), CV_32F);
    res=Mat(srcimg.size(), CV_8UC1);
//    tmap_img=Mat(srcimg.size(), CV_32F);
    srcimg.convertTo(src,CV_32F);
    prebilateralTonemapping1(src, dst,value1,value2);
    hdrsolve(value1,value2);
    emit s_imagechar(res);
}
//变动
void ui_local_hdr::on_v1_valueChanged(int value)
{

    ui->v1text->setText(QString::number(value));
    value1=float(value)/10;
    hdrsolve(value1,value2);

    emit s_imagechar(res);


}
void ui_local_hdr::on_v1text_editingFinished()
{
    int pos=ui->v1text->text().toInt();
    pos=pos>0?pos:1;
    pos=pos<101?pos:100;
    ui->v1->setValue(pos);

}


void ui_local_hdr::on_minus_clicked()
{
    int pos=ui->v1text->text().toInt()-1;
    pos=pos>0?pos:1;
    pos=pos<101?pos:100;
    ui->v1->setValue(pos);
}

void ui_local_hdr::on_plus_clicked()
{
    int pos=ui->v1text->text().toInt()+1;
    pos=pos>0?pos:1;
    pos=pos<101?pos:100;
    ui->v1->setValue(pos);
}


void ui_local_hdr::on_v1_sliderReleased()
{
    value1=float(ui->v1->value())/10;
    hdrsolve(value1,value2);
    emit s_imagechar(res);
}
void ui_local_hdr::on_v2_valueChanged(int value)
{

    ui->v2text->setText(QString::number(value));
    value2=float(value)/10;

    hdrsolve(value1,value2);
    emit s_imagechar(res);
}

void ui_local_hdr::on_minus_2_clicked()
{
    int pos=ui->v2text->text().toInt()-1;
    pos=pos>1?pos:1;
    pos=pos<101?pos:100;
    ui->v2->setValue(pos);
}

void ui_local_hdr::on_plus_2_clicked()
{
    int pos=ui->v2text->text().toInt()+1;
    pos=pos>1?pos:1;
    pos=pos<101?pos:100;
    ui->v2->setValue(pos);
}


void ui_local_hdr::on_v2text_editingFinished()
{
    int pos=ui->v2text->text().toInt();
    pos=pos>1?pos:1;
    pos=pos<101?pos:100;
    ui->v2->setValue(pos);
}
void ui_local_hdr::on_v2_sliderReleased()
{
    value1=float(ui->v2->value())/10;
    hdrsolve(value1,value2);
    emit s_imagechar(res);
}

void ui_local_hdr::on_ok_button_clicked()
{

}
void ui_local_hdr::closeEvent(QCloseEvent *)
{
    emit s_cancel();
}

void ui_local_hdr::on_cancel_button_clicked()
{
    emit s_cancel();
    this->close();
}















