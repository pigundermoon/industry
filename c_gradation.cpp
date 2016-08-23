#include "c_gradation.h"
#include "ui_c_gradation.h"
#include "mainwindow.h"
#include "hist.h"
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QImage"
#include "QPoint"
#include "QtConcurrent/QtConcurrent"



using namespace cv;





extern void getLevelMap(unsigned short map[], unsigned short inputDark, unsigned short inputGray, unsigned short inputLight, unsigned short outDark, unsigned short outLight);

extern void curve(Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);

extern void divcurve(int istart,int iend,int jstart,int jend, Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);

extern void levelAdjustment(Mat_<unsigned short>  input, Mat_<unsigned short> & output, unsigned short inputDark, unsigned short inputGray,unsigned short inputLight,  unsigned short outDark, unsigned short outLight);


c_gradation::c_gradation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::c_gradation)
{
    ui->setupUi(this);
}

c_gradation::~c_gradation()
{
    delete ui;
}

cv::Mat_<unsigned char> pre_sendshort( cv::Mat_<unsigned short> img)
{
    cv::Mat_<unsigned char> tosendimg=cv::Mat(img.rows,img.cols,CV_8UC1);
    for (int i=0;i<img.rows;i++)
    {
        for (int j=0;j<img.cols;j++)
        {
            tosendimg(i,j)=unsigned char(double(img(i,j))/65535*255);
        }
    }

    return tosendimg;

}


void c_gradation::on_input_black_valueChanged(int arg1)
{
    if(flag==1)
    {
        flag=0;
        return;
    }
    if (arg1>ui->input_gray->value())
    {
        return;
    }
//    int temp=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    int gnum=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    flag=1;
    ui->input_gray->setValue(gnum);
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
    emit s_imageshort(output);
    int newpos1=int(double(gnum)/65535*320)+10;
    int newpos=min(int(double(ui->input_black->value())/65535*320)+10,max(newpos1-1,0));
    ui->ing->setlstart(newpos+1);
    ui->inb->setlend(newpos1-1);
    ui->ing->setGeometry(QRect(newpos1,ui->ing->y(),ui->ing->width(),ui->ing->height()));
    ui->inb->setGeometry(QRect(newpos,ui->inb->y(),ui->inb->width(),ui->inb->height()));
}
void c_gradation::on_inb_poschanged(int num)
{
    int arg1=int(double(num-10)/320*65535);
    if (arg1>ui->input_gray->value())
    {
        return;
    }
    flag=1;
    ui->input_black->setValue(arg1);
////    int temp=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    int gnum=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    flag=1;
    ui->input_gray->setValue(gnum);
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
    emit s_imageshort(output);
    int newpos1=max(int(double(gnum)/65535*320)+10,num+1);
    ui->ing->setlstart(num+1);
    ui->ing->setpos(newpos1);
    ui->inb->setlend(newpos1-1);
    ui->ing->setGeometry(QRect(newpos1,ui->ing->y(),ui->ing->width(),ui->ing->height()));
}
void c_gradation::on_input_gray_valueChanged(int arg1)
{
    if(flag==1)
    {
        flag=0;
        return;
    }
    if (arg1<=ui->input_black->value())
    {
        return;
        ui->input_gray->setValue(ui->input_black->value()+1);
    }
    if (ui->input_gray->value()>=ui->input_white->value())
    {
        return;
        ui->input_gray->setValue(ui->input_white->value()-1);
    }
    rate=log(0.5) / log((double)(ui->input_gray->value()-ui->input_black->value()) /
                        (ui->input_white->value()-ui->input_black->value()));
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);
   int newpos=int(double(arg1)/65535*320)+10;
   ui->inb->setlend(newpos-1);
   ui->inw->setlstart(newpos+1);
   ui->ing->setGeometry(QRect(newpos,ui->ing->y(),ui->ing->width(),ui->ing->height()));
}
void c_gradation::on_ing_poschanged(int num)
{
    int arg1=int(double(num-10)/320*65535);
    if (arg1<=ui->input_black->value())
    {
        return;
        ui->input_gray->setValue(ui->input_black->value()+1);
    }
    if (ui->input_gray->value()>=ui->input_white->value())
    {
        return;
        ui->input_gray->setValue(ui->input_white->value()-1);
    }
    flag=1;
    ui->input_gray->setValue(arg1);
    rate=log(0.5) / log((double)(ui->input_gray->value()-ui->input_black->value()) /
                        (ui->input_white->value()-ui->input_black->value()));
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort(output);
   int newpos=num;
   ui->inb->setlend(newpos-1);
   ui->inw->setlstart(newpos+1);
//   ui->label_3->setText(QString::number(ui->ing->getlstart())+'\n'+QString::number(ui->ing->getlend())+'\n'+QString::number(ui->ing->getpos()));


}

void c_gradation::on_input_white_valueChanged(int arg1)
{
    if(flag==1)
    {
        flag=0;
        return;
    }
    if (ui->input_gray->value()+1>arg1)
    {
        return;
        ui->input_white->setValue(ui->input_black->value()+2);
    }
    int gnum=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    flag=1;
    ui->input_gray->setValue(gnum);
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);
   int newpos1=int(double(gnum)/65535*320)+10;
   int newpos=max(int(double(ui->input_white->value())/65535*320)+10,newpos1+1);
   ui->ing->setlend(newpos-1);
   ui->inw->setlstart(newpos1+1);
   ui->ing->setGeometry(QRect(newpos1,ui->ing->y(),ui->ing->width(),ui->ing->height()));
   ui->inw->setGeometry(QRect(newpos,ui->inw->y(),ui->inw->width(),ui->inw->height()));
}

void c_gradation::on_inw_poschanged(int num)
{
    int arg1=int(double(num-10)/320*65535);
    if (ui->input_gray->value()+1>arg1)
    {
        return;
        ui->input_white->setValue(ui->input_black->value()+2);
    }
    flag=1;
    ui->input_white->setValue(arg1);
    int gnum=ui->input_black->value()+(int)((double)(ui->input_white->value()-ui->input_black->value())*pow(double(2.718),log(0.5)/rate));
    flag=1;
    ui->input_gray->setValue(gnum);
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);
   int newpos1=min(int(double(gnum)/65535*320)+10,arg1-1);
   int newpos=num;
   ui->ing->setlend(newpos-1);
   ui->ing->setpos(newpos1);
   ui->inw->setlstart(newpos1+1);
   ui->ing->setGeometry(QRect(newpos1,ui->ing->y(),ui->ing->width(),ui->ing->height()));


}

void c_gradation::on_output_black_valueChanged(int arg1)
{
    if(flag==1)
    {
        flag=0;
        return;
    }
    if (arg1>=ui->outpu_white->value())
    {
        return;
        ui->output_black->setValue(ui->outpu_white->value()-1);
    }
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);
   int newpos=int(double(ui->output_black->value())/65535*320)+10;
   ui->outb->setpos(newpos);
   ui->outb->setGeometry(QRect(newpos,ui->outb->y(),ui->outb->width(),ui->outb->height()));
}


void c_gradation::on_outpu_white_valueChanged(int arg1)
{
    if(flag==1)
    {
        flag=0;
        return;
    }
    if (ui->output_black->value()>=arg1)
    {
        return;
        ui->outpu_white->setValue(ui->output_black->value()+1);
    }
    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort(output);
    int newpos=int(double(ui->outpu_white->value())/65535*320)+10;
    ui->outw->setpos(newpos);
    ui->outw->setGeometry(QRect(newpos,ui->outw->y(),ui->outw->width(),ui->outw->height()));
}

//收到发来16位图
void c_gradation::r_imageshort(cv::Mat_<unsigned short> a)
{
    srcimg=a;
    output=Mat_<unsigned short>(a.rows, a.cols, CV_16UC1);
    flag=0;
    rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
}


void c_gradation::closeEvent(QCloseEvent *)
{
    emit s_cancel();
}

void c_gradation::on_b_cancel_clicked()
{
    emit s_cancel();
    this->close();
}

void c_gradation::on_b_ok_clicked()
{

    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(),(unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
    emit s_ok(output);
    this->close();
}

cv::Mat_<unsigned char> myCal_Hist(cv::Mat_<unsigned char> Gray_img)
{
    int hist[256]={0};
    int bins = 256;


    for (int i=0;i<Gray_img.rows;i++)
    {
        for (int j=0;j<Gray_img.cols;j++)
        {

            int ts=int(Gray_img(i,j));
            int tpixel=ts;
            hist[tpixel]++;
        }
     }

    //绘制直方图图像
    int hist_height=256;
    //int bins = 256;
    int max_val=0;  //直方图的最大值
    int scale = 2;   //直方图的宽度
    for(int i=3;i<254;i++)
    {
        if (max_val<hist[i])
            max_val=hist[i];
    }
    max_val=double(max_val);
    cv::Mat_<unsigned char> hist_img = Mat::zeros(hist_height,bins*scale, CV_8UC1); //创建一个直方图图像并初始化为0
    for (int i=0;i<hist_img.rows;i++)
        for (int j=0;j<hist_img.cols;j++)
            hist_img(i,j)=255;

    //在直方图图像中写入直方图数据
    for(int i=0;i<bins;i++)
    {
        float bin_val =float(hist[i]); //	第i灰度级上的数
        int intensity = cvRound(bin_val*hist_height/max_val);  //要绘制的高度
        //填充第i灰度级的数据
        rectangle(hist_img,Point(i*scale,hist_height-1),
            Point((i+1)*scale - 1, hist_height - intensity),
            CV_RGB(0,0,0));
    }

    return hist_img;
}

void c_gradation::r_hist(cv::Mat_<unsigned char> a)
{
    cv::Mat_<unsigned char> histimg=myCal_Hist(a);
    QImage img=mat2qimage(histimg);
    img=img.scaled(331,150,Qt::IgnoreAspectRatio);
    ui->l_hist->setPixmap(QPixmap::fromImage(img));
    ui->l_hist->setAlignment(Qt::AlignCenter);


    cv::Mat_<unsigned char> bar_img = Mat::zeros(12,512,CV_8UC1);
    for(int i=0;i<256;i++)
    {
        float bin_val =1;
        double max_val=1;
        int intensity = cvRound(bin_val*12/max_val);
        rectangle(bar_img,Point(i*2,12-1),
            Point((i+1)*2-1, 12-intensity),
            CV_RGB(i,i,i));
    }
//    cv::imshow("",bar_img);
    QImage img1=mat2qimage(bar_img);
    img1=img1.scaled(331,20,Qt::KeepAspectRatio);
    ui->l_bar->setPixmap(QPixmap::fromImage(img1));
    ui->l_bar->setAlignment(Qt::AlignCenter);


}
//void c_gradation::mouseMoveEvent(QMouseEvent *ev)
//{
//    QPoint ts=ui->l_hist->pos();
//    QPoint tt=ev->pos();
//    ui->label_3->setText(QString::number(ts.x())+','+QString::number(ts.y())+'\n'+QString::number(tt.x())+','+QString::number(tt.y()));
//}
void c_gradation::on_outb_poschanged(int num)
{
    int st=ui->outb->getlstart();
    int len=320;
    int newnum=int(double(num-st)/double(len)*65535);
    flag=1;
    ui->output_black->setValue(newnum);
    if (newnum>=ui->outpu_white->value())
    {
        newnum=ui->outpu_white->value()-1;
        ui->output_black->setValue(newnum);
    }

    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);

}


void c_gradation::on_outw_poschanged(int num)
{
    int st=ui->outw->getlstart();
    int len=320;
    int newnum=int(double(num-st)/double(len)*65535);
    flag=1;
    ui->outpu_white->setValue(newnum);
    if (ui->output_black->value()>=newnum)
    {
        newnum=ui->output_black->value()+1;
        ui->outpu_white->setValue(newnum);
    }

    levelAdjustment(srcimg, output, (unsigned short)ui->input_black->value(),
                    (unsigned short)ui->input_gray->value(), (unsigned short)ui->input_white->value(), (unsigned short)ui->output_black->value(),
                    (unsigned short)ui->outpu_white->value());
   emit s_imageshort( output);


}













