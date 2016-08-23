#include "hist.h"


QImage mat2qimage(cv::Mat_<unsigned char>& img, int type)
{
    QImage img1;
    if (img.channels()==3)
    {
        cvtColor(img,img,CV_BGR2RGB);
        img1 = QImage((const unsigned char*)(img.data),img.cols,img.rows,
                      img.cols*img.channels(),QImage::Format_RGB888);
    }
    else if(img.channels()==1)
    {
        if (type==1)
        {
            cv::Mat timg;
            cvtColor(img,timg,CV_GRAY2RGBA);
            img1 = QImage((const unsigned char*)(timg.data),timg.cols,timg.rows,
                          timg.cols*timg.channels(),QImage::Format_RGBA8888);
            img1.convertToFormat(QImage::Format_ARGB32);
        }
        else
        {

            QVector<QRgb>  colorTable;

                for(int k=0;k<256;++k)
                {

                       colorTable.push_back( qRgb(k,k,k) );

                }


            img1 = QImage((const unsigned char*)(img.data),img.cols,img.rows,
                        img.cols*img.channels(),QImage::Format_Indexed8);
            img1.setColorTable(colorTable);
        }


    }
    else
    {
        img1 = QImage((const unsigned char *)(img.data), img.cols, img.rows,
                        img.cols*img.channels(), QImage::Format_RGB888);
    }
    return img1;
}


