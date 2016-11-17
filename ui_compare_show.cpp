#include "ui_compare_show.h"
#include "ui_ui_compare_show.h"

ui_compare_show::ui_compare_show(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_compare_show)
{
    ui->setupUi(this);

    curScale = 100;
    minScale = 25;
    maxScale = 400;

    h_center = 0.5;
    w_center = 0.5;

    ui->show1->installEventFilter(this);
    ui->show2->installEventFilter(this);
}

ui_compare_show::~ui_compare_show()
{
    delete ui;
}

void ui_compare_show::resizeEvent(QResizeEvent *event)
{
    ui->widget1->resize(this->width(),this->height()/2-3);
    ui->widget2->resize(this->width(),this->height()/2-3);
    show_image();
}

bool ui_compare_show::eventFilter(QObject *target, QEvent *e)
{

    if (e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* ev = static_cast<QMouseEvent*>(e);
        if (ev->button() == Qt::LeftButton)
        {
            scrollpos = ev->globalPos();
            scrollclicked = true;
        }
        return true;
    }
    else if (e->type() == QEvent::MouseMove)
    {
        QMouseEvent* ev = static_cast<QMouseEvent*>(e);
        if (scrollclicked == true)
        {
            QPoint temp = ev->globalPos();
            int xvalue = temp.x()-scrollpos.x();
            int yvalue = temp.y()-scrollpos.y();
            w_center -= ((double)xvalue)/((double)curScale*10)/4;
            h_center -= ((double)yvalue)/((double)curScale*10)/4;
            scrollpos=temp;
            show_image(0);
        }
            return true;
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* ev = static_cast<QMouseEvent*>(e);
        if (ev->button() == Qt::LeftButton)
        {
            scrollclicked = false;
        }
        return true;
    }
    else if (e->type() == QEvent::Wheel)
    {
        QWheelEvent* ev = static_cast<QWheelEvent*>(e);
        int num = ev->delta()/20;
        curScale = curScale + num;
        if (curScale > maxScale) curScale = maxScale;
        if (curScale < minScale) curScale = minScale;
        show_image();
        return true;
    }

    return QDialog::eventFilter(target,e);

}

void ui_compare_show::show_image(int type)
{
    ui->show1->clear();
    ui->show1->resize(ui->widget1->size());

    ui->show2->clear();
    ui->show2->resize(ui->widget2->size());

    int height = ((double)curScale)/100*imchar1.rows;
    int width = ((double)curScale)/100*imchar1.cols;
    double scalerate = ((double)curScale)/100;

    if (type==1) cv::resize(imchar1,cvtsrcimgchar1,cv::Size(width,height),scalerate,scalerate,CV_INTER_AREA);

    cv::Mat_<unsigned char> matshowimg = cvCreateMat(ui->show1->height(),ui->show1->width(),CV_8UC1);
    for (int i=0;i<matshowimg.rows;i++)
    {
        for (int j=0;j<matshowimg.cols;j++)
        {
            int ppi = h_center*cvtsrcimgchar1.rows+i-matshowimg.rows/2;
            int ppj = w_center*cvtsrcimgchar1.cols+j-matshowimg.cols/2;
            if (ppi<0 || ppj<0 || ppi>=cvtsrcimgchar1.rows || ppj>=cvtsrcimgchar1.cols)
            {
                matshowimg(i,j)=46;
            }
            else matshowimg(i,j)= cvtsrcimgchar1(ppi,ppj);
        }
    }
    QImage showimg=mat2qimage(matshowimg,0);
    ui->show1->setPixmap(QPixmap::fromImage(showimg));

    if (type==1) cv::resize(imchar2,cvtsrcimgchar2,cv::Size(width,height),scalerate,scalerate,CV_INTER_AREA);
    matshowimg = cvCreateMat(ui->show2->height(),ui->show2->width(),CV_8UC1);

    for (int i=0;i<matshowimg.rows;i++)
    {
        for (int j=0;j<matshowimg.cols;j++)
        {
            int ppi = h_center*cvtsrcimgchar2.rows+i-matshowimg.rows/2;
            int ppj = w_center*cvtsrcimgchar2.cols+j-matshowimg.cols/2;
            if (ppi<0 || ppj<0 || ppi>=cvtsrcimgchar2.rows || ppj>=cvtsrcimgchar2.cols)
            {
                matshowimg(i,j)=46;
            }
            else matshowimg(i,j)= cvtsrcimgchar2(ppi,ppj);
        }
    }
    showimg=mat2qimage(matshowimg,0);
    ui->show2->setPixmap(QPixmap::fromImage(showimg));
}


void ui_compare_show::r_2_imgshort(cv::Mat_<unsigned short> im1, cv::Mat_<unsigned short> im2,double scale)
{
    curScale = scale*100;
    if (curScale>maxScale) curScale = maxScale;
    if (curScale<minScale) curScale = minScale;

    imchar1 = Mat(im1.rows,im1.cols,CV_8UC1);
    imchar2 = Mat(im2.rows,im2.cols,CV_8UC1);
    for (int i=0;i<im1.rows;i++)
    {
        for (int j=0;j<im1.cols;j++)
        {
            imchar1(i,j) = unsigned char(double(im1(i,j))/65535*255);
        }
    }
    for (int i=0;i<im2.rows;i++)
    {
        for (int j=0;j<im2.cols;j++)
        {
            imchar2(i,j) = unsigned char(double(im2(i,j))/65535*255);
        }
    }
    show_image();
}
