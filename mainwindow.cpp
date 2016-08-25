#include "dcmtkfile.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTextCodec"
#include "QString"
#include "QFileDialog"
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "hist.h"
#include "c_gradation.h"
#include "string"
#include "cv.h"
#include "highgui.h"
#include "iostream"
#include "tiffio.h"
#include "queue"
#include "fstream"
#include "QFile"
#include "QFileDialog"
#include "QTextStream"
#include "QGraphicsDropShadowEffect"
#include "QDirModel"
#include "vector"


using namespace std;

extern int maxback;

extern int inout_interval;

extern unsigned short indark, inwhite, outdark, outwhite;

extern void getLevelMap(unsigned short map[65536], unsigned short inputDark,unsigned short inputGray, unsigned short inputLight,  unsigned short outDark, unsigned short outLight);

extern void curve(Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);

extern void divcurve(int istart,int iend,int jstart,int jend, Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);

extern void levelAdjustment(Mat_<unsigned short>  input, Mat_<unsigned short> & output, unsigned short inputDark, unsigned short inputGray,unsigned short inputLight,  unsigned short outDark, unsigned short outLight);




#define max(a,b) (a>b)?a:b
#define min(a,b) (a>b)?b:a




static QString recfilename;
static vector<QString> filelist;
static dcmtkfile* dcmFile;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

}

MainWindow::~MainWindow()
{

    delete ui;
}
void MainWindow::initialize()
{
    dcmFile = new dcmtkfile();
    rgflag=false;
    label_loc[0]=QString("[L]");
    label_loc[1]=QString("[H]");
    label_loc[2]=QString("[R]");
    label_loc[3]=QString("[F]");
    label_loc_ptr[0]=0;
    label_loc_ptr[1]=1;
    label_loc_ptr[2]=2;
    label_loc_ptr[3]=3;

    ui->imagelist->setResizeMode(QListView::Adjust);
    ui->imagelist->setMovement(QListView::Static);
    ui->imagelist->setSpacing(0);

    ui->toolBar->setIconSize(QSize(30, 30));

    ui->scrollshowimg->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollshowimg->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollclicked=false;

    ui->winshowimg->installEventFilter(this);
    double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
    ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));

    for(int i = 0; i < ui->toolBar->actions().length(); i++){
        ui->toolBar->widgetForAction(ui->toolBar->actions().at(i))->setObjectName(ui->toolBar->actions().at(i)->objectName());
    }

    QAction *ref = ui->toolBar->actions().at(3);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(11);
    font.setWeight(QFont::Normal);

    //选择窗宽窗位模式
    QComboBox *box = new QComboBox(ui->toolBar);
    box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    box->setFixedWidth(140);
    box->setFont(font);
    //TODO: 实现不同模式
    box->addItem("Auto");
    box->addItem("FullImage");
    box->setCurrentIndex(-1);
    ui->toolBar->insertWidget(ref, box);
    //窗宽
    QLabel *label = new QLabel(QString::fromLocal8Bit("窗宽"), ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin: 5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->insertWidget(ref, label);
    //数值
    QLineEdit *line = new QLineEdit(ui->toolBar);
    line->setAlignment(Qt::AlignRight);
    line->setValidator(new QIntValidator(1, 65535));
    line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    line->setFixedWidth(70);
    line->setFont(font);
    ui->toolBar->insertWidget(ref, line);
    //窗位
    label = new QLabel(QString::fromLocal8Bit("窗位"), ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin: 5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->insertWidget(ref, label);
    //数值
    line = new QLineEdit(ui->toolBar);
    line->setAlignment(Qt::AlignRight);
    line->setValidator(new QIntValidator(1, 65535));
    line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    line->setFixedWidth(70);
    line->setFont(font);
    ui->toolBar->insertWidget(ref, line);

    //缩放比例
    ref = ui->toolBar->actions().at(12);
    pRate = new QLineEdit(ui->toolBar);
    pRate->setAlignment(Qt::AlignRight);
    pRate->setFrame(false);
    pRate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    pRate->setFixedWidth(50);
    pRate->setFont(font);
    ui->toolBar->insertWidget(ref, pRate);
    connect(pRate, SIGNAL(editingFinished()), this, SLOT(on_rate_editingFinished()));
    disableaction();
    //百分号
    label = new QLabel("%", ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin-top:5px;"
                         "margin-bottom:5px;"
                         "margin-right:5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->insertWidget(ref, label);

    QDirModel *model = new QDirModel;
    ui->fileexplorer->setModel(model);
    ui->fileexplorer->setRootIndex(model->index(""));
}

//type=0，无变化，只平移，type=1，有变化，缩放/改变
void MainWindow::show_image(cv::Mat_<unsigned short> s, int type)
{
    int height = ((double)curScale)/100*s.rows;
    int width = ((double)curScale)/100*s.cols;
    double rate = ((double)curScale)/100;
    if (type==1)
    {
        for (int i=0;i<srcimgshort.rows;i++)
        {
            for (int j=0;j<srcimgshort.cols;j++)
            {
                srcimgchar(i,j)=unsigned char(double(s(i,j))/65535*255);
            }
        }
        if(!cvtsrcimgchar.empty()) cvtsrcimgchar.release();
        cv::Mat tmp;
        cv::resize(srcimgchar,tmp,cv::Size(width,height),rate,rate,CV_INTER_AREA);
        tmp.copyTo(cvtsrcimgchar);
    }

    cv::Mat_<unsigned char> matshowimg = cvCreateMat(ui->winshowimg->size().height(),ui->winshowimg->size().width(),CV_8UC1);
    for (int i=0;i<matshowimg.rows;i++)
    {
        for (int j=0;j<matshowimg.cols;j++)
        {
            int ppi = h_center*cvtsrcimgchar.rows+i-matshowimg.rows/2;
            int ppj = w_center*cvtsrcimgchar.cols+j-matshowimg.cols/2;
            if (ppi<0 || ppj<0 || ppi>=cvtsrcimgchar.rows || ppj>=cvtsrcimgchar.cols)
            {
                matshowimg(i,j)=46;
            }
            else matshowimg(i,j)= cvtsrcimgchar(ppi,ppj);
        }
    }

    showimg=mat2qimage(matshowimg,0);

    showimg=showimg.convertToFormat(QImage::Format_ARGB32);
    QPainter painter(&showimg); //为这个QImage构造一个QPainter
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);


    //改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    QFont font = painter.font();
    font.setBold(true);//加粗
    font.setPixelSize(20);//改变字体大小

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(20,showimg.height()/2,label_loc[label_loc_ptr[0]]);
    painter.drawText(showimg.width()/2,20,label_loc[label_loc_ptr[1]]);
    painter.drawText(showimg.width()-50,showimg.height()/2,label_loc[label_loc_ptr[2]]);
    painter.drawText(showimg.width()/2,showimg.height()-20,label_loc[label_loc_ptr[3]]);


    ui->winshowimg->setPixmap(QPixmap::fromImage(showimg));

}

bool MainWindow::eventFilter(QObject *target, QEvent *e)
{
    if (target == ui->winshowimg && !srcimgshort.empty() && !rgflag)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                scrollpos=ev->globalPos();
                scrollclicked=true;
                return true;
            }
            else if (ev->button()==Qt::RightButton)
            {
                cpos=ev->globalPos();
                cclicked=true;
                return true;
            }
        }
        else if(e->type() == QEvent::MouseMove)  
        {

            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (scrollclicked == true)
            {
                QPoint temp = ev->globalPos();
                int xvalue = temp.x()-scrollpos.x();
                int yvalue = temp.y()-scrollpos.y();
                w_center -= ((double)xvalue)/((double)curScale*10)/2;
                h_center -= ((double)yvalue)/((double)curScale*10)/2;
                if (w_center>0.99) w_center=0.99;
                if (w_center<0.01) w_center=0.01;
                if (h_center>0.99) h_center=0.99;
                if (h_center<0.01) h_center=0.01;
                scrollpos=temp;
                show_image(srcimgshort,0);
                return true;
            }
            else if (cclicked == true && !srcimgshort.empty())
            {

                QPoint temp = ev->globalPos();
                int xvalue = temp.x()-cpos.x();
                int yvalue = temp.y()-cpos.y();
                cpos=temp;

                int h_inc=inout_interval*xvalue;
                int tindark = indark-h_inc;
                int tinwhite = inwhite + h_inc;
                if (tindark<0) tindark=0;
                if (tinwhite>65535) tinwhite = 65535;
                if (tindark>=tinwhite-1) tindark = tinwhite - 2;

                int v_inc=inout_interval*yvalue;
                tindark = tindark - v_inc;
                tinwhite = tinwhite - v_inc;
                if (tindark<0) tindark=0;
                if (tinwhite>65535) tinwhite = 65535;
                if (tindark>=tinwhite-1) tindark = tinwhite - 2;



                indark = tindark;
                inwhite = tinwhite;
                double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
                ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));
                cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
                levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
                show_image(timg,1);
            }

        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                scrollclicked=false;
            }
            else if (ev->button()==Qt::RightButton)
            {
                cclicked=false;
            }
            return true;
        }
        else if(e->type() == QEvent::Wheel)
        {
            QWheelEvent* ev = static_cast<QWheelEvent*>(e);
            int num=ev->delta()/50;
            setCurScale(curScale + num);     
            return true;
        }
    }
    else if(target == ui->winshowimg && !srcimgshort.empty() && rgflag)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                rgstpos=ev->pos();
                return true;
            }
        }
        else if(e->type() == QEvent::MouseMove)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                rgedpos=ev->pos();

                QPainter painter(&showimg); //为这个QImage构造一个QPainter
                painter.setCompositionMode(QPainter::CompositionMode_SourceIn);


                //改变画笔和字体
                QPen pen = painter.pen();
                pen.setColor(Qt::white);
                QFont font = painter.font();
                font.setBold(true);//加粗
                font.setPixelSize(20);//改变字体大小

                painter.setPen(pen);
                painter.setFont(font);
                return true;
            }
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {


            rgflag=false;
            setCursor(Qt::ArrowCursor);
        }
    }

    return QMainWindow::eventFilter(target, e);
}


void MainWindow::enableaction()
{
    ui->hist->setEnabled(true);
    ui->zoom->setEnabled(true);
    ui->zoom_in->setEnabled(true);
    ui->zoom_out->setEnabled(true);
    ui->turn_horizontal->setEnabled(true);
    ui->turn_vertical->setEnabled(true);
    ui->invert->setEnabled(true);
    ui->back->setEnabled(true);
    ui->hdr->setEnabled(true);
    ui->localadaptive_hdr->setEnabled(true);
    ui->hist_hdr->setEnabled(true);
    ui->resave->setEnabled(true);
    ui->info->setEnabled(true);
    ui->turn->setEnabled(true);
    ui->contrast->setEnabled(true);
    ui->denoise->setEnabled(true);
    ui->rgradation->setEnabled(true);
    pRate->setEnabled(true);
}
void MainWindow::disableaction()
{
    ui->hist->setEnabled(false);
    ui->zoom->setEnabled(false);
    ui->zoom_in->setEnabled(false);
    ui->zoom_out->setEnabled(false);
    ui->turn_horizontal->setEnabled(false);
    ui->turn_vertical->setEnabled(false);
    ui->invert->setEnabled(false);
    ui->back->setEnabled(false);
    ui->hdr->setEnabled(false);
    ui->localadaptive_hdr->setEnabled(false);
    ui->hist_hdr->setEnabled(false);
    ui->resave->setEnabled(false);
    ui->info->setEnabled(false);
    ui->turn->setEnabled(false);
    ui->contrast->setEnabled(false);
    ui->denoise->setEnabled(false);
    ui->rgradation->setEnabled(false);
    pRate->setEnabled(false);
}

void MainWindow::reset()
{
    indark=0;
    inwhite=65535;
    outdark=0;
    outwhite=65535;
    inout_interval=10;
    double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
    ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));
}

//更改放缩显示数字
void MainWindow::settext(QString arg)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    pRate->setText(arg);
}
//另存文件
void MainWindow::on_resave_triggered()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QString filename1 = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("另存为"),"","Images(*.tiff)");
    if (filename1.isEmpty()) return;
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    TIFF* tif = TIFFOpen(code->fromUnicode(filename1).data(), "w");
//    TIFF* ttif = TIFFOpen(code->fromUnicode(filename).data(), "r");
        if (tif)
        {
            int h=srcimgshort.rows, w=srcimgshort.cols;
             uint32* temp;

//             TIFFGetField(ttif,TIFFTAG_IMAGEWIDTH,temp);
             TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

             TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,w);
//             TIFFGetField(ttif,TIFFTAG_IMAGELENGTH,temp);
             TIFFSetField(tif, TIFFTAG_IMAGELENGTH,h);
//             TIFFGetField(ttif,TIFFTAG_BITSPERSAMPLE,temp);
             TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,16);
//             TIFFGetField(ttif,TIFFTAG_SAMPLESPERPIXEL,temp);
             TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL,1);
//             TIFFGetField(ttif,TIFFTAG_ROWSPERSTRIP,temp);
//             TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,*temp);
//             TIFFGetField(ttif,TIFFTAG_COMPRESSION,temp);
//             TIFFSetField(tif, TIFFTAG_COMPRESSION,*temp);
//             TIFFGetField(ttif,TIFFTAG_PHOTOMETRIC,temp);
//             TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,*temp);
//             TIFFGetField(ttif,TIFFTAG_FILLORDER,temp);
//             TIFFSetField(tif, TIFFTAG_FILLORDER,*temp);
//             TIFFGetField(ttif,TIFFTAG_PLANARCONFIG,temp);
//             TIFFSetField(tif, TIFFTAG_PLANARCONFIG,*temp);
//             TIFFGetField(ttif,TIFFTAG_XRESOLUTION,temp);
//             TIFFSetField(tif, TIFFTAG_XRESOLUTION,*temp);
//             TIFFGetField(ttif,TIFFTAG_YRESOLUTION,temp);
//             TIFFSetField(tif, TIFFTAG_YRESOLUTION,*temp);
//             TIFFGetField(ttif,TIFFTAG_RESOLUTIONUNIT,temp);
//             TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT,*temp);

             tsize_t aa=TIFFScanlineSize(tif);
             tdata_t buf=_TIFFmalloc(TIFFScanlineSize(tif));
             uint16* data;
             data=(uint16*)buf;
             for (int row = 0; row < h; row++)
             {

                 for (int j=0;j<w;j++)
                     data[j]=(uint16)(65535-srcimgshort(row,j));

                 TIFFWriteScanline(tif,buf,(uint32)row, 0);
             }
//            TIFFClose(ttif);
            TIFFClose(tif);
        }
}

void MainWindow::on_imagelist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == previous) return;
    int num=ui->imagelist->currentRow();
    QString filename=filelist[num];
    openfile(filename,2);
}
//type=1,列表第一个，打开并显示，type=0，只在列表里显示，type=2，列表切换显示大图
void MainWindow::openfile(QString filename, int type)
{
    if (type!=2)
    {
        for ( vector<QString>::iterator it = filelist.begin();it<filelist.end();it++)
        {
            if (*it == filename) return;
        }
    }


    if (filename.isEmpty()) return;

    QTextCodec *code = QTextCodec::codecForName("gb18030");

    cv::Mat_<unsigned short> srcimgshort_temp;
    TIFF* tif = TIFFOpen(code->fromUnicode(filename).data(), "r");
    if (tif)
    {
        uint32 w, h;
        tdata_t buf;
        uint32 row;
        uint32 config;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        srcimgshort_temp=cv::Mat(h,w,CV_16UC1);
        TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
//            tsize_t aa=TIFFScanlineSize(tif);
        buf = _TIFFmalloc(TIFFScanlineSize(tif));



        uint16  nsamples;
        uint16* data;
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);

            for (row = 0; row < h; row++)
            {
                TIFFReadScanline(tif, buf, row, 0);
                data = (uint16*)buf;
                for (uint32 j=0;j < w;j++)
                {
                    srcimgshort_temp(row,j)=65535-data[j];
                }

                //printArray(data, imagelength);
            }
            // printArray(data,imagelength,height);


        _TIFFfree(buf);
        TIFFClose(tif);
    }
    else
    {
       if (!dcmFile->checkfile(filename)) return;
       srcimgshort_temp = dcmFile->loadfile(filename);
    }

    recfilename = filename;

    enableaction();
    if (srcimgshort_temp.rows>srcimgshort_temp.cols)
    {
        label_loc_ptr[0]=1;label_loc_ptr[1]=2;label_loc_ptr[2]=3;label_loc_ptr[3]=0;
        srcimgshort=cv::Mat(srcimgshort_temp.cols,srcimgshort_temp.rows,CV_16UC1);
        for (int i=0;i<srcimgshort_temp.rows;i++)
        {
            for (int j=0;j<srcimgshort_temp.cols;j++)
            {
                srcimgshort(srcimgshort_temp.cols-1-j,i)=srcimgshort_temp(i,j);
            }
        }
        srcimgshort_temp.release();
    }
    else
    {
        label_loc_ptr[0]=0;label_loc_ptr[1]=1;label_loc_ptr[2]=2;label_loc_ptr[3]=3;
        srcimgshort = srcimgshort_temp;
    }
    srcimgchar=cv::Mat(srcimgshort.rows,srcimgshort.cols,CV_8UC1);
    for (int i=0;i<srcimgshort.rows;i++)
    {
        for (int j=0;j<srcimgshort.cols;j++)
        {
            srcimgchar(i,j)=unsigned char(double(srcimgshort(i,j))/65535*255);
        }
    }
    if (type==0||type==1)
    {
        QImage tshowimage=mat2qimage(srcimgchar);
        QImage timage=tshowimage.scaled(ui->imagelist->width()-5,ui->imagelist->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        filelist.push_back(filename);

//        QListWidgetItem* Item = new QListWidgetItem(QIcon(QPixmap::fromImage(timage)),QString::number(++ptr ));
//        ui->imagelist->setIconSize(QSize(timage.width(),timage.height()));
//        ui->imagelist->addItem(Item);
        QListWidgetItem *item = new QListWidgetItem();
        ui->imagelist->addItem(item);

        QWidget *widget = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;

        QLabel *label = new QLabel;
        label->setPixmap(QPixmap::fromImage(timage));
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        layout->addWidget(label);

        label = new QLabel;
        label->setText(filename.split('/').back());
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: white;"
                             "height: 40;"
                             "font-family: Arial;"
                             "font-size: 12px;");
        layout->addWidget(label);

        widget->setLayout(layout);
        item->setSizeHint(QSize(ui->imagelist->width(), ui->imagelist->width() - 80));
        ui->imagelist->setItemWidget(item, widget);
        ui->imagelist->setCurrentItem(item);
    }


    if (type==2||type==1)
    {
//        emit s_imageshort(srcimgshort);
        showimg=mat2qimage(srcimgchar);

        int slidermax,slidermin,sliderpos;

        int winw=ui->winshowimg->width();
        int winh=ui->winshowimg->height();
        ow=showimg.width();
        oh=showimg.height();
        double wrate=double(winw)/ow;
        double hrate=double(winh)/oh;

        double rate=min(wrate,hrate);
        settext(QString::number((int)(rate*100)));

        if(rate>1)
        {
            sliderpos=100;
            slidermin=50;
            slidermax=400;
        }
        else
        {
            sliderpos=100*rate;
            slidermin=0.5*sliderpos;
            slidermax=200;
        }
        maxScale = slidermax;
        minScale = slidermin;
        curScale = sliderpos;
        pRate->setValidator(new QIntValidator(slidermin, slidermax));

        h_center=0.5;
        w_center=0.5;

        show_image(srcimgshort,1);

        QGraphicsDropShadowEffect *e1=new QGraphicsDropShadowEffect;
        e1->setColor(QColor(0,0,0));
        e1->setBlurRadius(10);
        e1->setOffset(1,1);
        ui->winshowimg->setGraphicsEffect(e1);
    }

    reset();
    backup.clear();
    ui->back->setEnabled(false);


}

//打开文件
void MainWindow::on_openfile_triggered()
{

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QStringList filelist = QFileDialog::getOpenFileNames(this,QString::fromLocal8Bit("打开"),"","Images (*.tif *.tiff);;(*.*)");
     for(QStringList::Iterator it=filelist.begin();it!=filelist.end();it++)
    {
        if (it==filelist.begin())
            openfile(*it,1);
        else
            openfile(*it,0);
    }
}

void MainWindow::setCurScale(int scale){
    scale = scale>minScale?scale:minScale;
    scale = scale<maxScale?scale:maxScale;
    if(curScale != scale){
        curScale = scale;
        settext(QString::number(curScale));
        double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
        ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));
        cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
        levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
        show_image(timg,1);
    }
}

//更改放缩大小文本框
void MainWindow::on_rate_editingFinished()
{
    int position=pRate->text().toInt();
    if (position==curScale) return;
    setCurScale(position);
}




//触发调整色阶按钮
void MainWindow::on_hist_triggered()
{
    if (srcimgshort.empty()) return;
    w1=new c_gradation();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QObject::connect(this,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),w1,SLOT(r_imageshort(cv::Mat_<unsigned short>)));
    QObject::connect(w1,SIGNAL(s_imagechar(cv::Mat_<unsigned char>)),this,SLOT(r_imagechar(cv::Mat_<unsigned char>)));
    QObject::connect(w1,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w1,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));
    QObject::connect(this,SIGNAL(s_hist(cv::Mat_<unsigned char>)),w1,SLOT(r_hist(cv::Mat_<unsigned char>)));
    QObject::connect(w1,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),this,SLOT(r_imageshort(cv::Mat_<unsigned short>)));

    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);

    emit s_imageshort(timg);
    emit s_hist(srcimgchar);
    w1->setWindowTitle(QString::fromLocal8Bit("色阶调整"));
    w1->setWindowFlags(Qt::WindowCloseButtonHint);
    w1->setGeometry(x()+100,y()+100,491,389);
    w1->setFixedWidth(491);
    w1->setFixedHeight(389);
    w1->exec();
}

//触发直方图均衡化hdr
void MainWindow::on_hist_hdr_triggered()
{
    if (srcimgshort.empty()) return;
    w2=new ui_hist_hdr();
    QObject::connect(this,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),w2,SLOT(r_imageshort(cv::Mat_<unsigned short>)));
    QObject::connect(w2,SIGNAL(s_imagechar(cv::Mat_<unsigned char>)),this,SLOT(r_imagechar(cv::Mat_<unsigned char>)));
    QObject::connect(w2,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w2,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));

    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);

    emit s_imageshort(timg);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w2->setWindowTitle(QString::fromLocal8Bit("直方图均衡化HDR调整"));
    w2->setWindowFlags(Qt::WindowCloseButtonHint);
    w2->setGeometry(x()+100,y()+100,332,100);
    w2->setFixedWidth(332);
    w2->setFixedHeight(100);
    w2->exec();
}
//局部调整hdr
void MainWindow::on_localadaptive_hdr_triggered()
{
    if (srcimgshort.empty()) return;
    w3=new ui_local_hdr();
    QObject::connect(this,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),w3,SLOT(r_imageshort(cv::Mat_<unsigned short>)));
    QObject::connect(w3,SIGNAL(s_imagechar(cv::Mat_<unsigned char>)),this,SLOT(r_imagechar(cv::Mat_<unsigned char>)));
    QObject::connect(w3,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w3,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));
    QObject::connect(w3,SIGNAL(s_dst(cv::Mat)),this,SLOT(r_lhdr_dst(cv::Mat)));

    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);

    emit s_imageshort(timg);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w3->setWindowTitle(QString::fromLocal8Bit("局部适应HDR调整"));
    w3->setWindowFlags(Qt::WindowCloseButtonHint);
    w3->setGeometry(x()+100,y()+100,316,152);
    w3->setFixedWidth(316);
    w3->setFixedHeight(152);
    w3->exec();
}
//对比度
void MainWindow::on_contrast_triggered()
{
    if (srcimgshort.empty()) return;
    w_contrast=new ui_contrast();
    QObject::connect(this,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),w_contrast,SLOT(r_imageshort(cv::Mat_<unsigned short>)));
    QObject::connect(w_contrast,SIGNAL(s_imagechar(cv::Mat_<unsigned char>)),this,SLOT(r_imagechar(cv::Mat_<unsigned char>)));
    QObject::connect(w_contrast,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w_contrast,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));
    QObject::connect(w_contrast,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),this,SLOT(r_imageshort(cv::Mat_<unsigned short>)));

    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);

    emit s_imageshort(timg);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w_contrast->setWindowTitle(QString::fromLocal8Bit("对比度调整"));
    w_contrast->setWindowFlags(Qt::WindowCloseButtonHint);
    w_contrast->setGeometry(x()+100,y()+100,369,105);
    w_contrast->setFixedWidth(369);
    w_contrast->setFixedHeight(105);
    w_contrast->exec();
}
//去噪
void MainWindow::on_denoise_triggered()
{
    w_process = new processing();
    QObject::connect(this,SIGNAL(s_number(int)),w_process,SLOT(r_number(int)));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w_process->setWindowTitle(QString::fromLocal8Bit("去噪处理中"));
    w_process->setWindowFlags(windowFlags() &~ Qt::WindowCloseButtonHint);
    w_process->setGeometry(x()+100,y()+100,276,68);
    w_process->setFixedWidth(276);
    w_process->setFixedHeight(68);
    w_process->show();

    QTime time;
    time.start();
    while(time.elapsed()<500)
    QCoreApplication::processEvents();

    emit(s_number(0));
    ui->back->setEnabled(true);
    if ((int)backup.size()<maxback)
    {
        backup.push_back(srcimgshort-0);
    }
    else
    {
        backup.pop_front();
        backup.push_back(srcimgshort-0);
    }


    levelAdjustment(srcimgshort,srcimgshort,indark,ingray,inwhite,outdark,outwhite);

    cv::Mat temp=cv::Mat(srcimgshort.size(),CV_32F);
    for (int i=0;i<srcimgshort.rows;i++)
    {
        for (int j=0;j<srcimgshort.cols;j++)
        {
            float t=float(srcimgshort(i,j));
            temp.at<float>(i,j)=float(srcimgshort(i,j));
            t=temp.at<float>(i,j);
        }
    }
    emit(s_number(10));
    cv::Mat temp1;

    cv::bilateralFilter(temp, temp1, 10, 50.0f, 50.0f);
    emit(s_number(80));
    double d1,d2;
    cv::Mat div=temp-temp1;
    cv::minMaxIdx(div,&d1,&d2);
    float t=temp1.at<float>(0,0);
    temp1.convertTo(srcimgshort,CV_16UC1);
    show_image(srcimgshort,1);
    emit(s_number(100));

    reset();


}
//图片信息
void MainWindow::on_info_triggered()
{
    if (srcimgshort.empty()) return;

    recdcmtkfile* recdfile = new recdcmtkfile();
    recdfile->height=srcimgshort.rows;
    recdfile->width=srcimgshort.cols;
    recdfile->flag=false;

    dcmtkfile* tmpdfile = new dcmtkfile();
    if (tmpdfile->checkfile(recfilename))
    {
        recdfile->flag=true;
        recdfile->date=QString::fromLocal8Bit(tmpdfile->date.data());
        recdfile->time=QString::fromLocal8Bit(tmpdfile->time.data());
        recdfile->id=QString::fromLocal8Bit(tmpdfile->id.data());
        recdfile->name=QString::fromLocal8Bit(tmpdfile->name.data());
    }

    w4=new ui_imageinfo();
    QObject::connect(this,SIGNAL(s_imageinfo(recdcmtkfile*)),w4,SLOT(r_imageinfo(recdcmtkfile*)));
    emit s_imageinfo(recdfile);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w4->setWindowTitle(QString::fromLocal8Bit("文件信息"));
    w4->setWindowFlags(Qt::WindowCloseButtonHint);
    w4->setGeometry(x()+100,y()+100,200,273);
    w4->setFixedWidth(200);
    w4->setFixedHeight(273);
    w4->exec();
}
//关于
void MainWindow::on_about_triggered()
{
    w5=new ui_about();
    w5->setWindowTitle(QString::fromLocal8Bit("关于本软件"));
    w5->setWindowFlags(Qt::WindowCloseButtonHint);
    w5->setGeometry(x()+100,y()+100,310,130);
    w5->setFixedWidth(310);
    w5->setFixedHeight(130);
    w5->exec();
}

//触发正负片按钮
void MainWindow::on_invert_triggered()
{
    ui->back->setEnabled(true);
    if ((int)backup.size()<maxback)
    {
        backup.push_back(srcimgshort-0);
    }
    else
    {
        backup.pop_front();
        backup.push_back(srcimgshort-0);
    }

    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);

    srcimgshort=65535-timg;

    show_image(srcimgshort,1);

    reset();
}
//触发水平翻转按钮
void MainWindow::on_turn_horizontal_triggered()
{
    ui->back->setEnabled(true);
    if ((int)backup.size()<maxback)
    {
        backup.push_back(srcimgshort-0);
    }
    else
    {
        backup.pop_front();
        backup.push_back(srcimgshort-0);
    }
    int w=srcimgshort.cols;
    int h=srcimgshort.rows;
    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
    for (int i=0;i<timg.rows;i++)
    {
        for (int j=0;j<timg.cols;j++)
        {
            srcimgshort(i,j)=timg(i,w-1-j);
        }
    }


    int tmplabel = label_loc_ptr[0];
    label_loc_ptr[0] = label_loc_ptr[2];
    label_loc_ptr[2] = tmplabel;
    show_image(srcimgshort);

    reset();
}
//触发竖直翻转按钮
void MainWindow::on_turn_vertical_triggered()
{
    ui->back->setEnabled(true);
    if ((int)backup.size()<maxback)
    {
        backup.push_back(srcimgshort-0);
    }
    else
    {
        backup.pop_front();
        backup.push_back(srcimgshort-0);
    }
    int w=srcimgshort.cols;
    int h=srcimgshort.rows;
    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
    for (int i=0;i<timg.rows;i++)
    {
        for (int j=0;j<timg.cols;j++)
        {
            srcimgshort(i,j)=timg(h-1-i,j);
        }
    }
    int tmplabel = label_loc_ptr[1];
    label_loc_ptr[1] = label_loc_ptr[3];
    label_loc_ptr[3] = tmplabel;
    show_image(srcimgshort);

    reset();
}
//接收到处理完图片的槽函数
void MainWindow::r_imagechar(cv::Mat_<unsigned char> img)
{

    int height = ((double)curScale)/100*img.rows;
    int width = ((double)curScale)/100*img.cols;
    double rate = ((double)curScale)/100;
    if(!cvtsrcimgchar.empty()) cvtsrcimgchar.release();
    cv::Mat tmp;
    cv::resize(img,tmp,cv::Size(width,height),rate,rate,CV_INTER_AREA);
    tmp.copyTo(cvtsrcimgchar);


    cv::Mat_<unsigned char> matshowimg = cvCreateMat(ui->winshowimg->size().height(),ui->winshowimg->size().width(),CV_8UC1);
    for (int i=0;i<matshowimg.rows;i++)
    {
        for (int j=0;j<matshowimg.cols;j++)
        {
            int ppi = h_center*cvtsrcimgchar.rows+i-matshowimg.rows/2;
            int ppj = w_center*cvtsrcimgchar.cols+j-matshowimg.cols/2;
            if (ppi<0 || ppj<0 || ppi>=cvtsrcimgchar.rows || ppj>=cvtsrcimgchar.cols)
            {
                matshowimg(i,j)=46;
            }
            else matshowimg(i,j)= cvtsrcimgchar(ppi,ppj);
        }
    }
    showimg=mat2qimage(matshowimg,0);

    showimg=showimg.convertToFormat(QImage::Format_ARGB32);
    QPainter painter(&showimg); //为这个QImage构造一个QPainter
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。

    //改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    QFont font = painter.font();
    font.setBold(true);//加粗
    font.setPixelSize(20);//改变字体大小

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(20,showimg.height()/2,label_loc[label_loc_ptr[0]]);
    painter.drawText(showimg.width()/2,20,label_loc[label_loc_ptr[1]]);
    painter.drawText(showimg.width()-50,showimg.height()/2,label_loc[label_loc_ptr[2]]);
    painter.drawText(showimg.width()/2,showimg.height()-20,label_loc[label_loc_ptr[3]]);


    ui->winshowimg->setPixmap(QPixmap::fromImage(showimg));
}

void MainWindow::r_lhdr_dst(cv::Mat a)
{
    cv::Mat_<unsigned char> img;
    a.convertTo(img, CV_8UC1, 255);
    int height = ((double)curScale)/100*img.rows;
    int width = ((double)curScale)/100*img.cols;
    double rate = ((double)curScale)/100;
    if(!cvtsrcimgchar.empty()) cvtsrcimgchar.release();
    cv::Mat tmp;
    cv::resize(img,tmp,cv::Size(width,height),rate,rate,CV_INTER_AREA);
    tmp.copyTo(cvtsrcimgchar);


    cv::Mat_<unsigned char> matshowimg = cvCreateMat(ui->winshowimg->size().height(),ui->winshowimg->size().width(),CV_8UC1);
    for (int i=0;i<matshowimg.rows;i++)
    {
        for (int j=0;j<matshowimg.cols;j++)
        {
            int ppi = h_center*cvtsrcimgchar.rows+i-matshowimg.rows/2;
            int ppj = w_center*cvtsrcimgchar.cols+j-matshowimg.cols/2;
            if (ppi<0 || ppj<0 || ppi>=cvtsrcimgchar.rows || ppj>=cvtsrcimgchar.cols)
            {
                matshowimg(i,j)=46;
            }
            else matshowimg(i,j)= cvtsrcimgchar(ppi,ppj);
        }
    }
    showimg=mat2qimage(matshowimg,0);

    showimg=showimg.convertToFormat(QImage::Format_ARGB32);
    QPainter painter(&showimg); //为这个QImage构造一个QPainter
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。

    //改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    QFont font = painter.font();
    font.setBold(true);//加粗
    font.setPixelSize(20);//改变字体大小

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(20,showimg.height()/2,label_loc[label_loc_ptr[0]]);
    painter.drawText(showimg.width()/2,20,label_loc[label_loc_ptr[1]]);
    painter.drawText(showimg.width()-50,showimg.height()/2,label_loc[label_loc_ptr[2]]);
    painter.drawText(showimg.width()/2,showimg.height()-20,label_loc[label_loc_ptr[3]]);


    ui->winshowimg->setPixmap(QPixmap::fromImage(showimg));
}

//16位
void MainWindow::r_imageshort(cv::Mat_<unsigned short> img)
{
    show_image(img,1);
}

//处理取消，还原
void MainWindow::r_cancel()
{
    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
    show_image(timg,1);

}
//处理确认，变更
void MainWindow::r_ok(cv::Mat_<unsigned short> a)
{
    ui->back->setEnabled(true);
    if ((int)backup.size() < maxback)
    {
        backup.push_back(srcimgshort);
    }
    else
    {
        backup.pop_front();
        backup.push_back(srcimgshort);
    }
    srcimgshort=a;
    for (int i=0;i<srcimgshort.rows;i++)
    {
        for (int j=0;j<srcimgshort.cols;j++)
        {
            srcimgchar(i,j)=unsigned char(double(srcimgshort(i,j))/65535*255);
        }
    }

    reset();
    show_image(srcimgshort,1);
}
//回退申请
void MainWindow::on_back_triggered()
{
    if (backup.empty()) return;

    srcimgshort=backup.back();
    backup.pop_back();
    if (backup.empty())
    {
        ui->back->setEnabled(false);
    }
    if (srcimgshort.empty())
    {
        ui->winshowimg->clear();
    }
    else
    {

        show_image(srcimgshort,1);

        reset();
    }

}
void MainWindow::on_exit_triggered()
{
    this->close();
}

void MainWindow::on_zoom_out_triggered()
{
    if (curScale-1>=minScale && curScale-1<=maxScale)
        setCurScale(curScale - 1);
}

void MainWindow::on_zoom_in_triggered()
{
    if (curScale + 1 >= minScale && curScale + 1 <= maxScale)
        setCurScale(curScale + 1);
}

void MainWindow::on_zoom_triggered()
{
    setCurScale(100);
}


void MainWindow::on_mark_triggered()
{
    rgflag=true;
    setCursor(Qt::CrossCursor);
}
