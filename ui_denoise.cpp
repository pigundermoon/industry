#include "ui_denoise.h"
#include "ui_ui_denoise.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
static Mat srcimg;
static float t1;
static float t2;
static int imgw;
static int imgh;
void boxFilter(Mat & input, Mat & output, int win_sz)
{
    Mat src;
    //边界处理，扩展win_sz
    copyMakeBorder(input, src, win_sz, win_sz, win_sz, win_sz, 4);
    //box_cols和box_rows为结果图像的大小
    int box_cols = input.cols, box_rows = input.rows;
    //cols和rows为扩展边界后的图像大小
    int cols = src.cols, rows = src.rows;
    //box_sz窗口大小
    int box_sz = 2 * win_sz + 1;
    CV_Assert(box_sz <= box_cols && box_sz <= box_rows);

    output = Mat(box_rows, box_cols, CV_32FC1);
    //盒式滤波需要长为cols的数组储存中间值
    float * buff = new float[cols];
    memset(buff, 0, cols * sizeof(float));

    //第一行单独处理，buff[i]记录图像前box_sz行的第i列的元素之和
    for (int i = 0; i < box_sz; i++)
    for (int j = 0; j < cols; j++)
        buff[j] += src.at<float>(i, j);

    for (int i = 0; i < box_rows; i++)
    {
        float temp_sum = 0;

        //第i行，第0列的窗口的元素之和
        for (int j = 0; j < box_sz; j++)
            temp_sum += buff[j];

        //第i行，第k列的窗口元素之和S[k]为S[k]=S[k-1]+buff[k]-buff[k-box_sz]
        output.at<float>(i, 0) = temp_sum;
        for (int j = 0; j < box_cols - 1; j++){
            temp_sum += buff[j + box_sz] - buff[j];
            output.at<float>(i, j + 1) = temp_sum;
        }

        //更新buff
        if (i == box_rows - 1) break;
        //窗口底部有第i-1行移动到第i行后，第j列的buff[j]=buff[j]+I[i][j]-I[i-box_sz][j]
        for (int j = 0; j < cols; j++)
            buff[j] += src.at<float>(i + box_sz, j) - src.at<float>(i, j);
    }

    output /= (box_sz * box_sz);	//取平均值
    delete[] buff;
}
//导向滤波
//灰度图去噪 原图像和导向图像相同，都为input
void guidedFilterGray(Mat& input, Mat& output, int radius, float epsilon)
{
    radius*=MIN(imgw,imgh);
    epsilon=(epsilon-1)*65535;
    float t1,t2;
    Mat guided_32f;
    input.copyTo(guided_32f);
//    guided_32f=guided_32f*double(255)/65535;
    t1=guided_32f.at<float>(1,1);
    //计算I*p和I*I
    Mat mat_I2;
    multiply(guided_32f, guided_32f, mat_I2);
    t2=mat_I2.at<float>(1,1);
    //计算各种均值
    Mat mean_I, mean_I2;

    boxFilter(guided_32f, mean_I, radius);
    boxFilter(mat_I2, mean_I2, radius);
    t1=mean_I.at<float>(1,1);
    t2=mean_I2.at<float>(1,1);
    //计算协方差和I的方差
    Mat cov_Ip = mean_I2 - mean_I.mul(mean_I);
    Mat var_I = cov_Ip + epsilon;
    t1=cov_Ip.at<float>(1,1);
    t2=var_I.at<float>(1,1);

    //求a和b
    Mat a, b;
    divide(cov_Ip, var_I, a);
    b = mean_I - a.mul(mean_I);
    t1=a.at<float>(1,1);
    t2=b.at<float>(1,1);

    //对包含像素i的所有a、b做平均
    Mat mean_a, mean_b;
    boxFilter(a, mean_a, radius);
    boxFilter(b, mean_b, radius);
    t1=mean_a.at<float>(1,1);
    t2=mean_b.at<float>(1,1);

    //计算输出
    output = mean_a.mul(guided_32f) + mean_b;
    double d1,d2;
    cv::Mat div=srcimg-output;
    cv::minMaxIdx(div,&d1,&d2);
    output.convertTo(output,CV_16UC1);
}



ui_denoise::ui_denoise(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_denoise)
{
    ui->setupUi(this);
}

ui_denoise::~ui_denoise()
{
    delete ui;
}
void ui_denoise::r_imageshort(cv::Mat_<unsigned short> a)
{
    imgw=a.rows/100;
    imgh=a.cols/100;
    a.convertTo(srcimg,CV_32FC1);
    guidedFilterGray(srcimg,outputimg,ui->windowsize->value(),ui->smooth->value());
    emit s_imageshort(outputimg);
}

void ui_denoise::on_windowsize_valueChanged(int value)
{
    ui->windowsizeedit->setText(QString::number(value));
        guidedFilterGray(srcimg,outputimg,ui->windowsize->value(),ui->smooth->value());

    emit s_imageshort(outputimg);
}

void ui_denoise::on_windowsizeedit_editingFinished()
{
    int pos=ui->windowsizeedit->text().toInt();
    pos=pos<ui->windowsize->maximum()?pos:ui->windowsize->maximum();
    pos=pos>ui->windowsize->minimum()?pos:ui->windowsize->minimum();
    ui->windowsize->setValue(pos);

}

void ui_denoise::on_smooth_valueChanged(int value)
{
    ui->smoothedit->setText(QString::number(value));
        guidedFilterGray(srcimg,outputimg,ui->windowsize->value(),ui->smooth->value());

    emit s_imageshort(outputimg);


}

void ui_denoise::on_smoothedit_editingFinished()
{
    int pos=ui->smoothedit->text().toInt();
    pos=pos<ui->smooth->maximum()?pos:ui->smooth->maximum();
    pos=pos>ui->smooth->minimum()?pos:ui->smooth->minimum();
    ui->smooth->setValue(pos);
}

void ui_denoise::on_ok_button_clicked()
{
    emit s_ok(outputimg);
    this->close();
}

void ui_denoise::on_cancel_button_clicked()
{
    emit s_cancel();
    this->close();
}

