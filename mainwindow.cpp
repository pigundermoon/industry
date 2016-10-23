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



using namespace std;

extern int maxback;

extern int inout_interval;

extern bool ifinvert;

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
    if (!srcimgshort.empty())
    {
        if (cur_item.exist)
        {
            industry_db.update_imageitem(cur_item);
        }
        else
        {
            industry_db.insert_imageitem(cur_item);
        }
    }
    free(rgvector);
    rgvector=NULL;
    delete ui;
}
void MainWindow::initialize()
{

    draw_height = 20;
    draw_dhwnum = 32;
    graruler=false;
    ui->winshowimg->setMouseTracking(true);
    rstatus = rsta_translation;
    drawst=false;
    industry_db.initialize();

    cur_item.exist=false;

    listchangedflag=false;

    vectornum=0;
    rgvector=NULL;
    dcmFile = new dcmtkfile();
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

    QAction *ref = ui->toolBar->actions().at(3);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(11);
    font.setWeight(QFont::Normal);

//工具栏

    {

    //取消
    QToolButton *button = new QToolButton();
    button->setToolTip(ui->back->toolTip());
    connect(button, SIGNAL(released()), ui->back, SLOT(trigger()));
    ButtonActionAdapter *adapter = new ButtonActionAdapter(this, ui->back, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {margin: 5px; border-image: url(:/img/img/cancel_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/cancel_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/cancel_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/cancel_disabled.png);}");

    ui->toolBar->addWidget(button);
    //分隔符
    ui->toolBar->addSeparator();
    //
    button = new QToolButton();
    connect(button, SIGNAL(released()), this, SLOT(on_drawrect_triggered()));
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/resize_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/resize_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/resize_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/resize_disabled.png);}");
    adapter = new ButtonActionAdapter(this, ui->drawrect, button);
    adapter->local_connect();
    ui->toolBar->addWidget(button);

    //选择窗宽窗位模式
    QComboBox *box = new QComboBox(ui->toolBar);
    box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    box->setFixedWidth(140);
    box->setFont(font);
    //TODO: 实现不同模式
    box->addItem("Auto");
    box->addItem("FullImage");
    box->setCurrentIndex(-1);
    ui->toolBar->addWidget(box);
    //窗宽
    QLabel *label = new QLabel(QString::fromLocal8Bit("窗宽"), ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin: 5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->addWidget(label);
    //数值
    QLineEdit *line = new QLineEdit(ui->toolBar);
    line->setAlignment(Qt::AlignRight);
    line->setValidator(new QIntValidator(1, 65535));
    line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    line->setFixedWidth(70);
    line->setFont(font);
    ui->toolBar->addWidget(line);
    //窗位
    label = new QLabel(QString::fromLocal8Bit("窗位"), ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin: 5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->addWidget(label);
    //数值
    line = new QLineEdit(ui->toolBar);
    line->setAlignment(Qt::AlignRight);
    line->setValidator(new QIntValidator(1, 65535));
    line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    line->setFixedWidth(70);
    line->setFont(font);
    ui->toolBar->addWidget(line);
    //分隔符
    ui->toolBar->addSeparator();
    //
    button = new QToolButton();
    connect(button, SIGNAL(released()), this, SLOT(on_zoom_in_triggered()));
    adapter = new ButtonActionAdapter(this, ui->zoom_in, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/zoomin_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/zoomin_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/zoomin_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/zoomin_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    connect(button, SIGNAL(released()), this, SLOT(on_zoom_out_triggered()));
    adapter = new ButtonActionAdapter(this, ui->zoom_out, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/zoomout_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/zoomout_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/zoomout_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/zoomout_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    connect(button, SIGNAL(released()), this, SLOT(on_zoom_triggered()));
    adapter = new ButtonActionAdapter(this, ui->zoom, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/zoom_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/zoom_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/zoom_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/zoom_disabled.png);}");
    ui->toolBar->addWidget(button);

    //缩放比例
    pRate = new QLineEdit(ui->toolBar);
    pRate->setAlignment(Qt::AlignRight);
    pRate->setFrame(false);
    pRate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    pRate->setFixedWidth(50);
    pRate->setFont(font);
    ui->toolBar->addWidget(pRate);
    connect(pRate, SIGNAL(editingFinished()), this, SLOT(on_rate_editingFinished()));
    //百分号
    label = new QLabel("%", ui->toolBar);
    label->setStyleSheet("color: rgb(255, 255, 255);"
                         "margin-top:5px;"
                         "margin-bottom:5px;"
                         "margin-right:5px;"
                         "border-bottom-width: 2px;");
    label->setFont(font);
    ui->toolBar->addWidget(label);
    //分隔符
    ui->toolBar->addSeparator();
    //
    button = new QToolButton();
    button->setToolTip(ui->contrast->toolTip());
    connect(button, SIGNAL(released()), ui->contrast, SLOT(trigger()));
    adapter = new ButtonActionAdapter(this, ui->contrast, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/contrast_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/contrast_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/contrast_down.png);}"
                     "QToolButton:disabled {border-image: url(:/img/img/contrast_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setToolTip(ui->invert->toolTip());
    connect(button, SIGNAL(released()), ui->invert, SLOT(trigger()));
    adapter = new ButtonActionAdapter(this, ui->invert, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/invert_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/invert_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/invert_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/invert_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setToolTip(ui->turn_horizontal->toolTip());
    connect(button, SIGNAL(released()), ui->turn_horizontal, SLOT(trigger()));
    adapter = new ButtonActionAdapter(this, ui->turn_horizontal, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/horizontal_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/horizontal_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/horizontal_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/horizontal_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setToolTip(ui->turn_vertical->toolTip());
    connect(button, SIGNAL(released()), ui->turn_vertical, SLOT(trigger()));
    adapter = new ButtonActionAdapter(this, ui->turn_vertical, button);
    adapter->local_connect();
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/vertical_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/vertical_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/vertical_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/vertical_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setEnabled(false);
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/rotate_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/rotate_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/rotate_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/rotate_disabled.png);}");
    ui->toolBar->addWidget(button);
    //分隔符
    ui->toolBar->addSeparator();
    //
    button = new QToolButton();
    button->setEnabled(false);
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/text_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/text_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/text_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/text_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setEnabled(false);
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/arrow_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/arrow_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/arrow_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/arrow_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setEnabled(false);
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/coordinate_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/coordinate_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/coordinate_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/coordinate_disabled.png);}");
    ui->toolBar->addWidget(button);
    //
    button = new QToolButton();
    button->setEnabled(false);
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/length_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/length_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/length_down.png);}"
                          "QToolButton:disabled {border-image: url(:/img/img/length_disabled.png);}");
    ui->toolBar->addWidget(button);
    //分隔符
    ui->toolBar->addSeparator();
    //
    button = new QToolButton();
    connect(button, SIGNAL(released()), ui->about, SLOT(trigger()));
    button->setStyleSheet("QToolButton {border-image: url(:/img/img/help_normal.png);}"
                     "QToolButton:hover:!pressed {border-image: url(:/img/img/help_hover.png);}"
                     "QToolButton:hover:pressed {border-image: url(:/img/img/help_down.png);}");
    ui->toolBar->addWidget(button);

    }

    dataset_model = new QStandardItemModel(ui->fileexplorer);
    dataset_model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("图片名")<<QStringLiteral("创建时间")<<QStringLiteral("路径"));
    ui->fileexplorer->setModel(dataset_model);
    ui->fileexplorer->setEditTriggers(QAbstractItemView::NoEditTriggers);

    refresh_dataset();

    enableaction();
    disableaction();
}


void MainWindow::on_fileexplorer_doubleClicked(const QModelIndex &index)
{
    QString path;
    if (index.column() == 2) path = index.data().toString();
    else
    {
        path = index.sibling(index.row(),2).data().toString();
    }
    openfile(path,1);
}

void MainWindow::refresh_dataset()
{
    dataset_model->clear();
    dataset_model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("图片名")<<QStringLiteral("创建时间")<<QStringLiteral("路径"));
    QStringList pathset = industry_db.query_all();
    for (int i=0;i<pathset.size();i++)
    {
        QString path = pathset.at(i);
        imageitem tmpitem = industry_db.query_imageitem(path);
        QStandardItem* itemimage = new QStandardItem(tmpitem.name);
        dataset_model->appendRow(itemimage);
        dataset_model->setItem(dataset_model->indexFromItem(itemimage).row(),1,new QStandardItem(tmpitem.date));
        dataset_model->setItem(dataset_model->indexFromItem(itemimage).row(),2,new QStandardItem(tmpitem.path));
    }
}

//type=0，无变化，只平移，type=1，有变化，缩放/改变，type=2，要划线，notcgra专为色阶窗口设计
void MainWindow::show_image(cv::Mat_<unsigned short> s, int type, bool notcgra)
{
    int height = ((double)curScale)/100*s.rows;
    int width = ((double)curScale)/100*s.cols;
    double scalerate = ((double)curScale)/100;
    if (type==1)
    {
        cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(s.rows, s.cols, CV_16UC1);
        double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
        ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));
        if (notcgra) levelAdjustment(s,timg,indark,ingray,inwhite,outdark,outwhite);
        else s.copyTo(timg);

        if (ifinvert) timg = 65535 - timg;
        for (int i=0;i<s.rows;i++)
        {
            for (int j=0;j<s.cols;j++)
            {
                srcimgchar(i,j)=unsigned char(double(timg(i,j))/65535*255);
            }
        }
        if(!cvtsrcimgchar.empty()) cvtsrcimgchar.release();
        cv::Mat tmp;
        cv::resize(srcimgchar,tmp,cv::Size(width,height),scalerate,scalerate,CV_INTER_AREA);
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

    QPainter* showpainter = new QPainter(&showimg);
    showpainter->setCompositionMode(QPainter::CompositionMode_SourceIn);


    //改变画笔和字体
    QPen pen = showpainter->pen();
    pen.setColor(Qt::white);
    QFont font = showpainter->font();
    font.setBold(true);//加粗
    font.setPixelSize(20);//改变字体大小
    font.setFamily("Microsoft YaHei UI");

    showpainter->setPen(pen);
    showpainter->setFont(font);

    showpainter->drawText(showimg.width()/31/2,showimg.height()/2,label_loc[label_loc_ptr[0]]);
    showpainter->drawText(showimg.width()/2,showimg.width()/31,label_loc[label_loc_ptr[1]]);
    showpainter->drawText(showimg.width()-showimg.width()/31,showimg.height()/2,label_loc[label_loc_ptr[2]]);
    showpainter->drawText(showimg.width()/2,showimg.height()-showimg.width()/30,label_loc[label_loc_ptr[3]]);

    font.setBold(false);
    font.setPixelSize(15);
    showpainter->setFont(font);
    pen.setColor(Qt::red);
    showpainter->setPen(pen);
    if (type==2)
    {
        showpainter->drawLine(rgstpos,rgedpos);
    }
    drawpaint(showpainter);







    ui->winshowimg->setPixmap(QPixmap::fromImage(showimg));
    showpainter->end();
}

void MainWindow::on_rulergra_triggered()
{
    graruler = !graruler;
    show_image(srcimgshort,0);
}

void MainWindow::drawpaint(QPainter *painter)
{
    if (cur_chartlist.chartlist.empty()) return;
    for (int i=0;i<cur_chartlist.chartlist.size();i++)
    {
        QPoint p1=(drawchart(cur_chartlist.chartlist.at(i))).p1;
        QPoint p2=(drawchart(cur_chartlist.chartlist.at(i))).p2;

        if ((drawchart(cur_chartlist.chartlist.at(i))).type==0)
        {
            painter->drawRect(p1.x(),p1.y(),p2.x()-p1.x(),p2.y()-p1.y());

            float j1 = w_center + ((double)(p1.x())-ui->winshowimg->width()/2)/(((double)curScale)/100) / srcimgshort.cols;
    //        if (j1<0.01||j1>0.99) j1 =0.01;

            float j2 = w_center + ((double)(p2.x())-ui->winshowimg->width()/2)/(((double)curScale)/100) / srcimgshort.cols;
    //        if (j2<0.01||j2>0.99) j2 =0.99;

            float i1 = h_center + ((double)(p1.y())-ui->winshowimg->height()/2)/(((double)curScale)/100) / srcimgshort.rows;
    //        if (i1<0.01||i1>0.99) i1 =0.01;

            float i2 = h_center + ((double)(p2.y())-ui->winshowimg->height()/2)/(((double)curScale)/100) / srcimgshort.rows;
    //        if (i2<0.01||i2>0.99) i2 =0.99;

            int gray = 0;
            int cnt = 0;

    //        cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    //        double rate=log(0.5) / log(((double)(32767) - (double)0) / ((double)65535 - (double)0));
    //        ingray=indark+(int)((double)(inwhite-indark)*pow(double(2.718),log(0.5)/rate));
    //        levelAdjustment(srcimgshort,timg,indark,ingray,inwhite,outdark,outwhite);
            for (int i=i1*srcimgchar.rows;i<=i2*srcimgchar.rows;i++)
            {
                if (i<0||i>=srcimgchar.rows) continue;
                for (int j=j1*srcimgchar.cols;j<=j2*srcimgchar.cols;j++)
                {
                    if (j<0||j>=srcimgchar.cols) continue;
                    gray = float(gray*cnt)/(cnt+1)+float(srcimgchar(i,j))/(cnt+1);
                }
            }
            painter->drawText(p1.x(),p1.y()-12,QString::number(gray));
        }
        else if ((drawchart(cur_chartlist.chartlist.at(i))).type==1)
        {
            QPen recpen = painter->pen();
            QBrush recbrush = painter->brush();

            if (graruler)
            {
                QPen tpen = painter->pen();
                tpen.setColor(Qt::white);
                painter->setPen(Qt::NoPen);

                for (int i=0;i<=draw_dhwnum;i++)
                {
                    int value = i*256/draw_dhwnum;
                    if (value>255) value = 255;
                    painter->setBrush(QBrush(QColor(value,value,value),Qt::SolidPattern));
                    painter->setPen(tpen);
                    painter->drawText(p1.x()+i*draw_dhw,p1.y()-8,QString::number(value));
                    painter->setPen(Qt::NoPen);
                    painter->drawRect(p1.x()+i*draw_dhw,p1.y(),draw_dhw,draw_height);
                    if (i==draw_dhwnum && p1.x()+i*draw_dhw+draw_dhw<ui->winshowimg->width() && p1.x()+i*draw_dhw+1.5*draw_dhw>ui->winshowimg->width())
                    {
                        painter->drawRect(p1.x()+(i+1)*draw_dhw,p1.y(),ui->winshowimg->width()-p1.x()+(i+1)*draw_dhw,draw_height);
                    }
                }
            }

            painter->setPen(recpen);
            painter->setBrush(recbrush);
        }
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *e)
{
    QMouseEvent* tev = static_cast<QMouseEvent*>(e);
    if (rstatus == rsta_translation)
    {
        if (cur_chartlist.ifonrec(tev->pos()))
        {
            setCursor(Qt::SizeFDiagCursor);
            oldchart = cur_chartlist.findonrec(tev->pos());
        }
        else if (cur_chartlist.ifonshape(tev->pos()))
        {
            setCursor(Qt::OpenHandCursor);
            oldchart = cur_chartlist.findonshape(tev->pos());
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }

    if (target == ui->winshowimg && !srcimgshort.empty() && rstatus == rsta_translation)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                if (cur_chartlist.ifonrec(tev->pos()))
                {
                    rstatus = rsta_changerect;
                    stdrawpos = oldchart.p1;
                    prepos = oldchart.p2;
                    return true;
                }
                else if(cur_chartlist.ifonshape(tev->pos()))
                {
                    rstatus = rsta_dragrect;
                    prepos = ev->pos();
                    setCursor(Qt::ClosedHandCursor);
                    return true;
                }
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
                w_center -= ((double)xvalue)/((double)curScale*10)/4;
                h_center -= ((double)yvalue)/((double)curScale*10)/4;
//                if (w_center>0.99) w_center=0.99;
//                if (w_center<0.01) w_center=0.01;
//                if (h_center>0.99) h_center=0.99;
//                if (h_center<0.01) h_center=0.01;
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
                show_image(srcimgshort,1);
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
            if (cur_chartlist.ifonshape(tev->pos(),1))
            {
                draw_dhw+=ev->delta()/10;
                if (draw_dhw>200) draw_dhw=200;
                if (draw_dhw<30) draw_dhw=30;
                for (int i=0;i<cur_chartlist.chartlist.size();i++)
                {
                    if ((drawchart(cur_chartlist.chartlist.at(i))).type==1)
                    {
                        drawchart oldc = (drawchart(cur_chartlist.chartlist.at(i)));
                        drawchart newc = oldc;
                        newc.p2.setX(newc.p1.x()+draw_dhw*(draw_dhwnum+1));
                        cur_chartlist.update(oldc,newc);
                        oldchart = newc;
                    }
                }
                show_image(srcimgshort,0);
            }
            else
            {
                int num=ev->delta()/50;
                setCurScale(curScale + num);
            }


            return true;
        }
    }
    else if(target == ui->winshowimg && !srcimgshort.empty() && rstatus == rsta_mark)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            if (rgvector!=NULL)
            {
                free(rgvector);
                rgvector=NULL;
            }
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::LeftButton)
            {
                drawst=true;
                rgstpos=ev->pos();
                return true;
            }
        }
        else if(e->type() == QEvent::MouseMove && drawst)
        {

            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            rgedpos=ev->pos();
            show_image(srcimgshort,2);

            return true;

        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            rstatus = rsta_translation;
            setCursor(Qt::ArrowCursor);

            float strate = w_center+
            ((double)((rgstpos.x()<rgedpos.x()?rgstpos.x():rgedpos.x())-ui->winshowimg->width()/2)/(((double)curScale)/100) / srcimgshort.cols);
            if (strate<0.01||strate>0.99) strate =0.01;

            float edrate = w_center+
            ((double)((rgstpos.x()>rgedpos.x()?rgstpos.x():rgedpos.x())-ui->winshowimg->width()/2)/(((double)curScale)/100) / srcimgshort.cols);
            if ((!(edrate>strate))||edrate>1) edrate=0.99;


            rgvector = (int *)malloc(srcimgshort.rows*sizeof(int));
            if (rgvector==NULL)
            {
                cerr<<"allocate error!"<<endl;
                return false;
            }
            vectornum = srcimgshort.rows;

            memset(rgvector,0,sizeof(rgvector));

            int ttsum=0;
            for (int i=0;i<srcimgshort.rows;i++)
            {
                int tsum=0;
                int cnt=0;
                for (int j=strate*srcimgshort.cols;j<edrate*srcimgshort.cols;j++)
                {
                    cnt++;
                    tsum+=srcimgshort[i][j];
                }
                rgvector[i]=tsum/cnt;
                ttsum+=rgvector[i];
            }
            ttsum/=vectornum;
            for (int i=0;i<srcimgshort.rows;i++)
            {
                rgvector[i]=ttsum - rgvector[i];
            }

            drawst=false;
            show_image(srcimgshort,0);

        }
    }
    else if(target == ui->winshowimg && !srcimgshort.empty() && rstatus == rsta_drawrect)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {

            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            if (ev->button()==Qt::RightButton)
            {
                rstatus = rsta_translation;
                setCursor(Qt::ArrowCursor);
            }
            else if (ev->button()==Qt::LeftButton)
            {
                drawst=true;
                stdrawpos = ev->pos();
                pardraw = 10000;
            }
        }
        else if (e->type() == QEvent::MouseMove && drawst )
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();
            if (pardraw == 10000)
            {
                pardraw=0;
                int x1,x2,y1,y2;
                drawchart tmpc;
                tmpc.type=0;

                x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
                x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
                y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
                y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();

                tmpc.p1.setX(x1);tmpc.p1.setY(y1);tmpc.p2.setX(x2);tmpc.p2.setY(y2);
                cur_chartlist.insert(tmpc);

            }

            else
            {


                drawchart oldc,newc;
                oldc.type=0;newc.type=0;
                int x1,x2,y1,y2;
                x1 = stdrawpos.x()<prepos.x()?stdrawpos.x():prepos.x();
                x2 = stdrawpos.x()>prepos.x()?stdrawpos.x():prepos.x();
                y1 = stdrawpos.y()<prepos.y()?stdrawpos.y():prepos.y();
                y2 = stdrawpos.y()>prepos.y()?stdrawpos.y():prepos.y();
                oldc.p1.setX(x1);oldc.p1.setY(y1);oldc.p2.setX(x2);oldc.p2.setY(y2);

                x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
                x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
                y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
                y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();
                newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

                cur_chartlist.update(oldc,newc);
            }

            prepos=tmp;
            show_image(srcimgshort,0);
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();
            if (pardraw == 10000)
            {
                pardraw=0;
                int x1,x2,y1,y2;
                drawchart tmpc;
                tmpc.type=0;

                x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
                x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
                y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
                y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();

                tmpc.p1.setX(x1);tmpc.p1.setY(y1);tmpc.p2.setX(x2);tmpc.p2.setY(y2);
                cur_chartlist.insert(tmpc);

            }

            else
            {

                drawchart oldc,newc;
                oldc.type=0;newc.type=0;
                int x1,x2,y1,y2;
                x1 = stdrawpos.x()<prepos.x()?stdrawpos.x():prepos.x();
                x2 = stdrawpos.x()>prepos.x()?stdrawpos.x():prepos.x();
                y1 = stdrawpos.y()<prepos.y()?stdrawpos.y():prepos.y();
                y2 = stdrawpos.y()>prepos.y()?stdrawpos.y():prepos.y();
                oldc.p1.setX(x1);oldc.p1.setY(y1);oldc.p2.setX(x2);oldc.p2.setY(y2);

                x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
                x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
                y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
                y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();
                newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

                cur_chartlist.update(oldc,newc);
            }

            prepos=tmp;
            show_image(srcimgshort,0);
            drawst=false;
            rstatus = rsta_translation;
            setCursor(Qt::ArrowCursor);
        }
    }
    else if(target == ui->winshowimg && !srcimgshort.empty() && rstatus == rsta_changerect)
    {
        if (e->type() == QEvent::MouseMove)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();

            drawchart oldc,newc;
            oldc.type=0;newc.type=0;
            int x1,x2,y1,y2;
            x1 = stdrawpos.x()<prepos.x()?stdrawpos.x():prepos.x();
            x2 = stdrawpos.x()>prepos.x()?stdrawpos.x():prepos.x();
            y1 = stdrawpos.y()<prepos.y()?stdrawpos.y():prepos.y();
            y2 = stdrawpos.y()>prepos.y()?stdrawpos.y():prepos.y();
            oldc.p1.setX(x1);oldc.p1.setY(y1);oldc.p2.setX(x2);oldc.p2.setY(y2);

            x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
            x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
            y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
            y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();
            newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

            cur_chartlist.update(oldc,newc);

            prepos=tmp;
            show_image(srcimgshort,0);
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();

            drawchart oldc,newc;
            oldc.type=0;newc.type=0;
            int x1,x2,y1,y2;
            x1 = stdrawpos.x()<prepos.x()?stdrawpos.x():prepos.x();
            x2 = stdrawpos.x()>prepos.x()?stdrawpos.x():prepos.x();
            y1 = stdrawpos.y()<prepos.y()?stdrawpos.y():prepos.y();
            y2 = stdrawpos.y()>prepos.y()?stdrawpos.y():prepos.y();
            oldc.p1.setX(x1);oldc.p1.setY(y1);oldc.p2.setX(x2);oldc.p2.setY(y2);

            x1 = stdrawpos.x()<tmp.x()?stdrawpos.x():tmp.x();
            x2 = stdrawpos.x()>tmp.x()?stdrawpos.x():tmp.x();
            y1 = stdrawpos.y()<tmp.y()?stdrawpos.y():tmp.y();
            y2 = stdrawpos.y()>tmp.y()?stdrawpos.y():tmp.y();
            newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

            cur_chartlist.update(oldc,newc);

            prepos=tmp;
            show_image(srcimgshort,0);
            rstatus = rsta_translation;
        }
    }
    else if(target == ui->winshowimg && !srcimgshort.empty() && rstatus == rsta_dragrect)
    {
        if (e->type() == QEvent::MouseMove)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();

            drawchart newc;
            newc.type=oldchart.type;
            int x1,x2,y1,y2;
            int dx = tmp.x() - prepos.x();
            int dy = tmp.y() - prepos.y();
            prepos = tmp;

            x1 = oldchart.p1.x()+dx;
            x2 = oldchart.p2.x()+dx;
            y1 = oldchart.p1.y()+dy;
            if (y1>ui->winshowimg->height()-draw_height)
            {
                y1=ui->winshowimg->height()-draw_height;
            }
            if (y1<20)
            {
                y1=20;
            }

            y2 = y1+draw_height;
            newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

            cur_chartlist.update(oldchart,newc);
            oldchart = newc;

            prepos=tmp;
            show_image(srcimgshort,0);
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* ev = static_cast<QMouseEvent*>(e);
            QPoint tmp = ev->pos();

            drawchart newc;
            newc.type=oldchart.type;
            int x1,x2,y1,y2;
            int dx = tmp.x() - prepos.x();
            int dy = tmp.y() - prepos.y();
            prepos = tmp;

            x1 = oldchart.p1.x()+dx;
            x2 = oldchart.p2.x()+dx;
            y1 = oldchart.p1.y()+dy;
            if (y1>ui->winshowimg->height()-draw_height)
            {
                y1=ui->winshowimg->height()-draw_height;
            }
            if (y1<15)
            {
                y1=15;
            }

            y2 = oldchart.p2.y()+dy;
            newc.p1.setX(x1);newc.p1.setY(y1);newc.p2.setX(x2);newc.p2.setY(y2);

            cur_chartlist.update(oldchart,newc);
            oldchart = newc;

            prepos=tmp;
            show_image(srcimgshort,0);
            rstatus = rsta_translation;
        }
    }

    return QMainWindow::eventFilter(target, e);
}


void MainWindow::enableaction()
{
    ui->rulergra->setEnabled(true);
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
    ui->drawrect->setEnabled(true);
    ui->resetdraw->setEnabled(true);
    pRate->setEnabled(true);
    ui->homotran->setEnabled(true);
    ui->emboss->setEnabled(true);
}
void MainWindow::disableaction()
{
    ui->rulergra->setEnabled(false);
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
    ui->drawrect->setEnabled(false);
    ui->resetdraw->setEnabled(false);
    pRate->setEnabled(false);
    ui->homotran->setEnabled(false);
    ui->emboss->setEnabled(false);
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
    QString filename1 = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("另存为"),"","Images (*.tiff);;dicom (*.)");
    if (filename1.isEmpty()) return;
    if (!QString(filename1.split('.').back()).compare("tiff"))
    {
        QTextCodec *code = QTextCodec::codecForName("gb18030");
        TIFF* tif = TIFFOpen(code->fromUnicode(filename1).data(), "w");
        if (tif)
        {
            int h=srcimgshort.rows, w=srcimgshort.cols;
             uint32* temp;
             TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
             TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,w);
             TIFFSetField(tif, TIFFTAG_IMAGELENGTH,h);
             TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,16);
             TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL,1);
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
            TIFFClose(tif);
        }
    }
    else
    {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
        QByteArray ba;
        DcmFileFormat fileformat;

        ba = cur_item.name.toLocal8Bit();
        fileformat.getDataset()->putAndInsertString(DCM_PatientName,ba.data());
        ba = cur_item.id.toLocal8Bit();
        fileformat.getDataset()->putAndInsertString(DCM_PatientID,ba.data());
        ba = cur_item.date.toLocal8Bit();
        fileformat.getDataset()->putAndInsertString(DCM_StudyDate,ba.data());
        ba = cur_item.date.toLocal8Bit();
        fileformat.getDataset()->putAndInsertString(DCM_StudyTime,ba.data());
        fileformat.getDataset()->putAndInsertUint16(DCM_Rows,srcimgshort.rows);
        fileformat.getDataset()->putAndInsertUint16(DCM_Columns,srcimgshort.cols);

        Uint16* pData = new Uint16[srcimgshort.rows*srcimgshort.cols];
        for (int i=0;i<srcimgshort.rows;i++)
        {
            for (int j=0;j<srcimgshort.cols;j++)
            {
                pData[i*srcimgshort.cols+j] = 65536 - srcimgshort(i,j);
            }
        }
        fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData,pData,srcimgshort.rows*srcimgshort.cols);
        ba = filename1.toLocal8Bit();
        fileformat.saveFile(ba.data(),EXS_LittleEndianImplicit,EET_UndefinedLength,EGL_withoutGL);
    }

}

void MainWindow::on_imagelist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == previous || previous==NULL)
    {
        listchangedflag=false;
        return;
    }
    if (listchangedflag == true)
    {
        listchangedflag = false;
        return;
    }
    int num=ui->imagelist->currentRow();
    QString filename=filelist[num];
    openfile(filename,2);
}
//type=1,列表第一个，打开并显示，type=0，只在列表里显示，type=2，列表切换显示大图
void MainWindow::openfile(QString filename, int type)
{
    bool nullflag = false;
    if (srcimgshort.empty()) nullflag=true;

    if (type!=2)
    {
        for ( vector<QString>::iterator it = filelist.begin();it<filelist.end();it++)
        {
            if (*it == filename) return;
        }
    }

    if (!srcimgshort.empty() && (type==1 || type==2))
    {
        if (cur_item.exist)
        {
            industry_db.update_imageitem(cur_item);
        }
        else
        {
            industry_db.insert_imageitem(cur_item);
            refresh_dataset();
        }
    }

    if (filename.isEmpty()) return;

    QString filename_suffix = filename.split('.').back();
    rawfile tmpraw;

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
    else if(!QString::fromLocal8Bit(filename_suffix.toLocal8Bit().data()).compare(("raw")))
    {
        tmpraw.readfile(filename);
        tmpraw.img.copyTo(srcimgshort_temp);
    }
    else
    {
       if (!dcmFile->checkfile(filename)) return;
       srcimgshort_temp = dcmFile->loadfile(filename);
    }

    recfilename = filename;

    disableaction();
    enableaction();
    if (srcimgshort_temp.rows>srcimgshort_temp.cols)
    {
        label_loc_ptr[0]=1;label_loc_ptr[1]=2;label_loc_ptr[2]=3;label_loc_ptr[3]=0;
        srcimgshort=cv::Mat(srcimgshort_temp.cols,srcimgshort_temp.rows,CV_16UC1);
        raw_srcimgshort=cv::Mat(srcimgshort_temp.cols,srcimgshort_temp.rows,CV_16UC1);
        for (int i=0;i<srcimgshort_temp.rows;i++)
        {
            for (int j=0;j<srcimgshort_temp.cols;j++)
            {
                srcimgshort(srcimgshort_temp.cols-1-j,i)=srcimgshort_temp(i,j);
                raw_srcimgshort(srcimgshort_temp.cols-1-j,i)=srcimgshort_temp(i,j);
            }
        }
        srcimgshort_temp.release();
    }
    else
    {
        label_loc_ptr[0]=0;label_loc_ptr[1]=1;label_loc_ptr[2]=2;label_loc_ptr[3]=3;
        srcimgshort = srcimgshort_temp;
        raw_srcimgshort=cv::Mat(srcimgshort_temp.cols,srcimgshort_temp.rows,CV_16UC1);
        srcimgshort.copyTo(raw_srcimgshort);
    }

    if (type == 1 || type == 2)
    {
        cur_item = industry_db.query_imageitem(filename);
        if (!cur_item.exist)
        {
            cur_item.path=filename;
            cur_item.name=filename.split('/').back();

            QDateTime dt;
            QTime time;
            QDate date;
            dt.setTime(time.currentTime());
            dt.setDate(date.currentDate());
            cur_item.date=dt.toString("yyyy/MM/dd hh:mm:ss");

            if (!QString::fromLocal8Bit(filename_suffix.toLocal8Bit().data()).compare(("raw")))
            {
                cur_item.id = tmpraw.scanid;
            }
            else
            {
                cur_item.id = "tmp";
            }
            cur_item.operation = "";
            cur_item.chart = "";
            update_chartlist(cur_item.chart);
        }
        else
        {
            ifinvert = false;
            srcimgshort=parse_operationstr(cur_item.operation, srcimgshort);
            update_chartlist(cur_item.chart);
        }

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
        cv::Mat_<uchar> tcharimg = cv::Mat(raw_srcimgshort.rows, raw_srcimgshort.cols, CV_8UC1);
        for (int i=0;i<raw_srcimgshort.rows;i++)
        {
            for (int j=0;j<raw_srcimgshort.cols;j++)
            {
                tcharimg(i,j)=unsigned char(double(raw_srcimgshort(i,j))/65535*255);
            }
        }
        QImage tshowimage=mat2qimage(tcharimg);
        QImage timage=tshowimage.scaled(ui->imagelist->width()-5,ui->imagelist->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        filelist.push_back(filename);

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
        item->setSizeHint(QSize(ui->imagelist->width(), ui->imagelist->width() /2));
        ui->imagelist->setItemWidget(item, widget);
        if(type == 1)
        {
            listchangedflag = true;
            ui->imagelist->setCurrentItem(item);
        }
    }


    if (type==2||type==1)
    {
//        emit s_imageshort(srcimgshort);
        showimg=mat2qimage(srcimgchar);

        int slidermax,slidermin,sliderpos;

        int winw=ui->winshowimg->width()*0.93;
        int winh=ui->winshowimg->height()*0.93;
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
        if (nullflag)
        {
            reset();
            curScale = sliderpos;
            h_center=0.5;
            w_center=0.5;
        }
        if (curScale>slidermax) curScale=slidermax;
        maxScale = slidermax;
        minScale = slidermin;

        pRate->setValidator(new QIntValidator(slidermin, slidermax));

        show_image(srcimgshort,1);

        QGraphicsDropShadowEffect *e1=new QGraphicsDropShadowEffect;
        e1->setColor(QColor(0,0,0));
        e1->setBlurRadius(10);
        e1->setOffset(1,1);
        ui->winshowimg->setGraphicsEffect(e1);
    }

}

//解析图形串
//$0:x1,y1,x2,y2
void MainWindow::update_chartlist(QString chart)
{
    cur_chartlist.clear();

    draw_dhw = ui->winshowimg->width()/(draw_dhwnum+1);
    drawchart tmpchart;
    tmpchart.type=1;
    tmpchart.p1=QPoint(0,ui->winshowimg->height()-draw_height);
    tmpchart.p2=QPoint(ui->winshowimg->width(),ui->winshowimg->height());
    cur_chartlist.insert(tmpchart);

    QStringList charts = chart.split('$');
    for (int i=0;i<charts.size();i++)
    {
        QString cur_chartchar = charts.at(i);
        QString type = cur_chartchar.split(':').first();
        if (type=="0")
        {
            QString nums = cur_chartchar.split(':').back();
            drawchart tmpchar;
            tmpchar.type=0;
            QString tmp;
            tmp = nums.split(',').at(0);
            tmpchar.p1.setX(tmp.toInt());
            tmp = nums.split(',').at(1);
            tmpchar.p1.setY(tmp.toInt());
            tmp = nums.split(',').at(2);
            tmpchar.p2.setX(tmp.toInt());
            tmp = nums.split(',').at(3);
            tmpchar.p2.setY(tmp.toInt());

            cur_chartlist.insert(tmpchar);
        }
    }

}

void MainWindow::on_drawrect_triggered()
{
    rstatus = rsta_drawrect;
    setCursor(Qt::CrossCursor);
}

void MainWindow::on_resetdraw_triggered()
{
    cur_chartlist.clear();
    drawchart tmpchart;
    tmpchart.type=1;
    tmpchart.p1=QPoint(0,ui->winshowimg->height()-draw_height);
    tmpchart.p2=QPoint(ui->winshowimg->width(),ui->winshowimg->height());
    cur_chartlist.insert(tmpchart);

    show_image(srcimgshort,0);
}

//打开文件
void MainWindow::on_openfile_triggered()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QStringList filelist = QFileDialog::getOpenFileNames(this,QString::fromLocal8Bit("打开"),"","Images (*.tif *.tiff);;(*.raw);;(*.*)");
     for(QStringList::Iterator it=filelist.begin();it!=filelist.end();it++)
    {
        if (it==filelist.begin())
        {
            openfile(*it,1);
        }
        else
        {
            openfile(*it,0);
        }

    }
}

void MainWindow::setCurScale(int scale)
{
    scale = scale>minScale?scale:minScale;
    scale = scale<maxScale?scale:maxScale;
    if(curScale != scale)
    {
        curScale = scale;
        settext(QString::number(curScale));
        show_image(srcimgshort,1);
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
    QObject::connect(this,SIGNAL(s_imageshort_hist(cv::Mat_<unsigned short>,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short))
                     ,w1,SLOT(r_imageshort_hist(cv::Mat_<unsigned short>,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short)));
    QObject::connect(w1,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w1,SIGNAL(s_ok_hist(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short)),this,
                     SLOT(r_ok_hist(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short)));
    QObject::connect(this,SIGNAL(s_hist(cv::Mat_<unsigned char>)),w1,SLOT(r_hist(cv::Mat_<unsigned char>)));
    QObject::connect(w1,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),this,SLOT(r_imageshort_cgra(cv::Mat_<unsigned short>)));

    emit s_imageshort_hist(srcimgshort,indark,ingray,inwhite,outdark,outwhite);

    cv::Mat_<unsigned char> timg = cv::Mat(raw_srcimgshort.rows,raw_srcimgshort.cols,CV_8UC1);
    for (int i=0;i<raw_srcimgshort.rows;i++)
    {
        for (int j=0;j<raw_srcimgshort.cols;j++)
        {
            timg(i,j) = unsigned char(double(raw_srcimgshort(i,j))/65535*255);
        }
    }
    emit s_hist(timg);
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
//    QObject::connect(w2,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));

    emit s_imageshort(srcimgshort);
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
//    QObject::connect(w3,SIGNAL(s_ok(cv::Mat_<unsigned short>)),this,SLOT(r_ok(cv::Mat_<unsigned short>)));



    emit s_imageshort(srcimgshort);
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
    if(!ui->contrast->isEnabled())
        return;
    if (srcimgshort.empty()) return;
    w_contrast=new ui_contrast();
    QObject::connect(this,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),w_contrast,SLOT(r_imageshort(cv::Mat_<unsigned short>)));
    QObject::connect(w_contrast,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w_contrast,SIGNAL(s_ok(cv::Mat_<unsigned short>,QString)),this,SLOT(r_ok(cv::Mat_<unsigned short>,QString)));
    QObject::connect(w_contrast,SIGNAL(s_imageshort(cv::Mat_<unsigned short>)),this,SLOT(r_imageshort(cv::Mat_<unsigned short>)));

    emit s_imageshort(srcimgshort);
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
//    float t=temp1.at<float>(0,0);
    temp1.convertTo(srcimgshort,CV_16UC1);
    show_image(srcimgshort,1);
    emit(s_number(100));



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
    if(srcimgshort.empty())  return;
    ui->back->setEnabled(true);

    ifinvert=!ifinvert;

    show_image(srcimgshort,1);

    cur_item.operation+="$0:";

}
//触发水平翻转按钮
void MainWindow::on_turn_horizontal_triggered()

{
    if(srcimgshort.empty()) return;
    ui->back->setEnabled(true);

    int w=srcimgshort.cols;
    int h=srcimgshort.rows;
    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    srcimgshort.copyTo(timg);
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
    show_image(srcimgshort,1);
    cur_item.operation+="$1:";
}

//触发竖直翻转按钮
void MainWindow::on_turn_vertical_triggered()
{
    if(srcimgshort.empty()) return;
    ui->back->setEnabled(true);

    int w=srcimgshort.cols;
    int h=srcimgshort.rows;
    cv::Mat_<unsigned short> timg=cv::Mat_<unsigned short>(srcimgshort.rows, srcimgshort.cols, CV_16UC1);
    srcimgshort.copyTo(timg);
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
    show_image(srcimgshort,1);
    cur_item.operation+="$2:";
}
//接收到处理完图片的槽函数
void MainWindow::r_imagechar(cv::Mat_<unsigned char> img)
{
    cv::Mat_<ushort> timg = cv::Mat(img.rows,img.cols,CV_16UC1);
    for (int i=0;i<img.rows;i++)
    {
        for (int j=0;j<img.cols;j++)
        {
            timg(i,j) = img(i,j)*255;
        }
    }
    show_image(timg,1);
}

//16位
void MainWindow::r_imageshort(cv::Mat_<unsigned short> img)
{
    show_image(img,1);
}

//16位
void MainWindow::r_imageshort_cgra(cv::Mat_<unsigned short> img)
{
    show_image(img,1,false);
}

//处理取消，还原
void MainWindow::r_cancel()
{
    show_image(srcimgshort,1);
}

//调整色阶处理确认，变更
void MainWindow::r_ok_hist(unsigned short tindark, unsigned short tingray, unsigned short tinwhite, unsigned short toutdark, unsigned short toutwhite)
{
    indark = tindark;
    ingray=tingray;
    inwhite=tinwhite;
    outdark=toutdark;
    outwhite=toutwhite;

    show_image(srcimgshort,1);

}

//处理确认，变更
void MainWindow::r_ok(cv::Mat_<unsigned short> a,QString opt)
{
    ui->back->setEnabled(true);
    srcimgshort=a;
    show_image(srcimgshort,1);
    cur_item.operation+=opt;
}

//回退申请
void MainWindow::on_back_triggered()
{
    if (srcimgshort.empty()) return;
    else
    {
        cur_item.operation="";
        raw_srcimgshort.copyTo(srcimgshort);
        show_image(srcimgshort,1);
    }
}

void MainWindow::on_exit_triggered()
{
    this->close();
}

void MainWindow::on_zoom_out_triggered()
{
    if (srcimgshort.empty()) return;
    if (curScale-1>=minScale && curScale-1<=maxScale)
        setCurScale(curScale - 1);
}

void MainWindow::on_zoom_in_triggered()
{
    if (srcimgshort.empty()) return;
    if (curScale + 1 >= minScale && curScale + 1 <= maxScale)
        setCurScale(curScale + 1);
}

void MainWindow::on_zoom_triggered()
{
    if (srcimgshort.empty()) return;
    setCurScale(100);
}


void MainWindow::on_mark_triggered()
{
    rstatus = rsta_mark;
    setCursor(Qt::CrossCursor);
}

void MainWindow::on_removegrade_triggered()
{
    ui->back->setEnabled(true);
    if (rgvector!=NULL && srcimgshort.rows==vectornum)
    {
        for (int i=0;i<srcimgshort.rows;i++)
        {
            for (int j=0;j<srcimgshort.cols;j++)
            {
                srcimgshort(i,j)+=rgvector[i];
            }
        }
    }

    show_image(srcimgshort,1);
}

//错切变换
void MainWindow::on_homotran_triggered()
{
    if (srcimgshort.empty()) return;

    w6=new ui_sliderchoose();
    QObject::connect(w6,SIGNAL(s_value(float)),this,SLOT(r_degree(float)));
    QObject::connect(w6,SIGNAL(s_ok_value(float)),this,SLOT(r_ok_degree(float)));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w6->setWindowTitle(QString::fromLocal8Bit("错切变换"));
    w6->setWindowFlags(Qt::WindowCloseButtonHint);
    w6->setGeometry(x()+100,y()+100,201,91);
    w6->setFixedWidth(201);     
    w6->setFixedHeight(93);
    w6->exec();
}

void MainWindow::r_degree(float degree)
{
    Mat_<unsigned short> timg;
    srcimgshort.copyTo(timg);
    homotransfer(timg,degree);
    show_image(timg,1);
}

void MainWindow::r_ok_degree(float degree)
{
    homotransfer(srcimgshort,degree);
    show_image(srcimgshort,1);
    if (abs(degree)>0.01) cur_item.operation+="$5:" + QString::number(degree);
}

void MainWindow::on_emboss_triggered()
{

    if (srcimgshort.empty()) return;
    w7=new ui_emboss();
    QObject::connect(w7,SIGNAL(s_cancel()),this,SLOT(r_cancel()));
    QObject::connect(w7,SIGNAL(s_ok_value(int,int)),SLOT(r_ok_emboss_value(int,int)));
    QObject::connect(w7,SIGNAL(s_value(int,int)),this,SLOT(r_emboss_value(int,int)));

    Mat_<unsigned short> timg;
    srcimgshort.copyTo(timg);
    emboss(timg,3,100);
    show_image(timg,1);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w7->setWindowTitle(QString::fromLocal8Bit("浮雕化"));
    w7->setWindowFlags(Qt::WindowCloseButtonHint);
    w7->setGeometry(x()+100,y()+100,404,150);
    w7->setFixedWidth(404);
    w7->setFixedHeight(150);
    w7->exec();
}

void MainWindow::r_emboss_value(int dis, int contrast)
{
    Mat_<unsigned short> timg;
    srcimgshort.copyTo(timg);
    emboss(timg,dis,contrast);
    show_image(timg,1);
}

void MainWindow::r_ok_emboss_value(int dis, int contrast)
{
    emboss(srcimgshort,dis,contrast);
    show_image(srcimgshort,1);
    cur_item.operation+="$6:"+QString::number(dis)+","+QString::number(contrast);
}

void MainWindow::on_scan_triggered()
{
    QFile file("path.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }

        QByteArray line = file.readLine();
        QString execname = QString::fromLocal8Bit(line);
        QProcess::execute(execname);

        line = file.readLine();
        QString filepath = QString::fromLocal8Bit(line);
        QDir dir;
        dir.setPath(filepath);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Size | QDir::Reversed);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            if (!fileInfo.suffix().compare("raw",Qt::CaseInsensitive))
            {
                imageitem tmp;
                tmp = industry_db.query_imageitem("data\\" + QString(fileInfo.fileName().split('.').first()));
                if (tmp.exist) continue;
                else
                {
                    tmp.path = "data\\" + QString(fileInfo.fileName().split('.').first());
                    tmp.name = fileInfo.fileName().split('.').first();
                    tmp.id = tmp.name.split('_').first();

                    QDateTime dt;
                    QTime time;
                    QDate date;
                    dt.setTime(time.currentTime());
                    dt.setDate(date.currentDate());
                    tmp.date=dt.toString("yyyy/MM/dd hh:mm:ss");

                    rawfile tmpraw;
                    tmpraw.resavedcm(fileInfo.filePath(),dt,tmp.path);

                    tmp.operation = "";
                    tmp.chart = "";

                    industry_db.insert_imageitem(tmp);
                    refresh_dataset();
                }
            }

        }

}







//if (!fileInfo.suffix().compare("TIFF",Qt::CaseInsensitive)|| !fileInfo.suffix().compare("TIF",Qt::CaseInsensitive) || !fileInfo.suffix().compare("raw",Qt::CaseInsensitive))
//{

//    imageitem tmp;
//    tmp = industry_db.query_imageitem(fileInfo.filePath());
//    if (tmp.exist) continue;
//    else
//    {


//        tmp.path=fileInfo.filePath();
//        tmp.name=fileInfo.fileName();

//        QDateTime dt;
//        QTime time;
//        QDate date;
//        dt.setTime(time.currentTime());
//        dt.setDate(date.currentDate());
//        tmp.date=dt.toString("yyyy/MM/dd hh:mm:ss");

//        if (!QString(QString::fromLocal8Bit(QString(tmp.name.split('.').back()).toLocal8Bit().data()).data()).compare(("raw")))
//        {
//            tmp.id = tmp.name.split('_').first();
//        }
//        else
//        {
//            tmp.id = "tmp";
//        }

//        tmp.operation = "";
//        tmp.chart = "";

//        industry_db.insert_imageitem(tmp);
//        refresh_dataset();
//    }
//}



