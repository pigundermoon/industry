#include "homo_clip.h"
#include "ui_homo_clip.h"

extern bool auto_homotransfer;
extern bool auto_clip;
extern bool auto_name;

extern float degree;
extern int clip_left;
extern int clip_top;
extern int clip_right;
extern int clip_bottom;
extern int clip_cnt;
extern QString clip_name;

homo_clip::homo_clip(QWidget *parent, QString tfilepath, QDateTime tdt, imageitem& tmpitem, database& tdb) :
    QDialog(parent),
    ui(new Ui::homo_clip)
{
    ui->setupUi(this);

    filepath = tfilepath;
    dt = tdt;
    ifrotate = false;
    dataitem = tmpitem;
    datadb = tdb;
}

bool homo_clip::initialize()
{
    QString fullfilename = filepath.split('/').back();
    filename = fullfilename.split('.').first();
    scanid = fullfilename.split('_').first();
    QString wandh = filename.split('_').at(1);
    int width = QString(wandh.split('x').first()).toInt();
    int height = QString(wandh.split('x').back()).toInt();


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba = filepath.toLocal8Bit();
    ifstream rfile(ba.data(),ios::binary);

    long long size = width*height*2;
    char *buffer = new char[size];
    rfile.read(buffer,size);
    unsigned short *pimg = (unsigned short*) buffer;
    rfile.close();

    if (width < height)
    {
        ifrotate = true;
        rawimg = Mat_<unsigned short>(width,height,CV_16UC1);
        for (int i=0;i<rawimg.rows;i++)
        {
            for (int j=0;j<rawimg.cols;j++)
            {
                rawimg(i,j) = 65535 - pimg[j*rawimg.rows+rawimg.rows-i-1];
            }
        }
    }
    else
    {
        ifrotate = false;
        rawimg = Mat_<unsigned short>(height,width,CV_16UC1);
        for (int i=0;i<rawimg.rows;i++)
        {
            for (int j=0;j<rawimg.cols;j++)
            {
                rawimg(i,j) = 65535 - pimg[i*rawimg.cols+j];
            }
        }
    }
    ifinitial = true;
    if (auto_homotransfer && auto_clip && auto_name && clip_cnt%8!=0)
    {
        on_ok_clicked();
        return false;
    }
    else
    {
        ui->autoclip->setChecked(auto_clip);
        ui->autohomo->setChecked(auto_homotransfer);
        ui->autoname->setChecked(auto_name);
        ui->degree->setText(QString::number(degree,'f',2));
        ui->top->setText(QString::number(clip_top));
        ui->bottom->setText(QString::number(clip_bottom));
        ui->left->setText(QString::number(clip_left));
        ui->right->setText(QString::number(clip_right));
        ui->clipname->setText(clip_name);


        Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
        rawimg.copyTo(tmp);
        homotransfer(tmp,degree);
        Mat res = clipimg(tmp);
        showimg(res);
    }

    ifinitial = false;
    return true;
}

homo_clip::~homo_clip()
{
    delete ui;
}

void homo_clip::r_para(QString filepath, QDateTime dt, QString tgtpath)
{

}

Mat_<unsigned short> homo_clip::clipimg(Mat_<unsigned short> homoimg)
{
    if (clip_left<0 || clip_left>=homoimg.cols) clip_left = 0;
    if (clip_right<0 || clip_right + clip_left >= homoimg.cols) clip_right = 0;
    if (clip_top<0 || clip_top>=homoimg.rows) clip_top = 0;
    if (clip_bottom<0 || clip_top + clip_bottom >= homoimg.rows) clip_bottom = 0;

    Mat_<unsigned short> resimg = Mat_<unsigned short>(homoimg.rows - clip_bottom - clip_top, homoimg.cols - clip_left - clip_right,CV_16UC1);

    for (int i=clip_top;i<homoimg.rows - clip_bottom;i++)
    {
        for (int j=clip_left;j<homoimg.cols-clip_right;j++)
        {
            resimg(i-clip_top,j-clip_left) = homoimg(i,j);
        }
    }
    return resimg;
}

void homo_clip::showimg(Mat_<unsigned short> img)
{
    Mat_<unsigned char> timg = cvCreateMat(img.rows,img.cols,CV_8UC1);
    for (int i=0;i<timg.rows;i++)
    {
        for (int j=0;j<timg.cols;j++)
        {
            timg(i,j) = double(img(i,j))/65535*255;
        }
    }
    QImage showimg=mat2qimage(timg);
    showimg = showimg.scaled(882,302,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->showimage->setPixmap(QPixmap::fromImage(showimg));
}


void homo_clip::on_autohomo_stateChanged(int arg1)
{

    if (ui->autohomo->isChecked()) auto_homotransfer = true;
    else auto_homotransfer = false;
}

void homo_clip::on_autoclip_stateChanged(int arg1)
{
    if (ui->autoclip->isChecked()) auto_clip = true;
    else auto_clip = false;
}

void homo_clip::on_autoname_clicked()
{
    if (ui->autoname->isChecked()) auto_name = true;
    else auto_name = false;
    clip_cnt = 0;
}


void homo_clip::on_ok_clicked()
{
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat_<unsigned short> res = clipimg(tmp);
    showimg(res);

    Mat_<unsigned short> savemat;
    if (ifrotate)
    {
        savemat = Mat_<unsigned short>(res.cols,res.rows);
        for (int i=0;i<savemat.rows;i++)
        {
            for (int j=0;j<savemat.cols;j++)
            {
                savemat(i,j) = res(savemat.cols-j-1,i);
            }
        }
    }
    else
    {
        res.copyTo(savemat);
    }

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba;
    DcmFileFormat fileformat;

    dataitem.id = QString("%1").arg((clip_cnt++),8,10,QChar('0'));
    dataitem.name = clip_name+dataitem.id;
    dataitem.path = dataitem.path+dataitem.name;

    ba = dataitem.name.toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_PatientName,ba.data());
    ba = dataitem.id.toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_PatientID,ba.data());
    ba = dt.toString("yyyy/MM/dd").toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_StudyDate,ba.data());
    ba = dt.toString("hh:mm:ss").toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_StudyTime,ba.data());

    fileformat.getDataset()->putAndInsertUint16(DCM_Rows,savemat.rows);
    fileformat.getDataset()->putAndInsertUint16(DCM_Columns,savemat.cols);

    Uint16* pData = new Uint16[savemat.rows*savemat.cols];
    for (int i=0;i<savemat.rows;i++)
    {
        for (int j=0;j<savemat.cols;j++)
        {
            pData[i*savemat.cols+j] = 65535 - savemat(i,j);
        }
    }
    fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData,pData,savemat.rows*savemat.cols);
    ba = dataitem.path.toLocal8Bit();
    OFCondition status = fileformat.saveFile(ba.data(),EXS_LittleEndianImplicit,EET_UndefinedLength,EGL_withoutGL);
    if (status.good())
    {
        datadb.insert_imageitem(dataitem);
    }
    this->close();
}

void homo_clip::on_degree_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    degree = arg1.toFloat();
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat res = clipimg(tmp);
    showimg(res);
}

void homo_clip::on_top_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    clip_top = arg1.toInt();
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat res = clipimg(tmp);
    showimg(res);
}

void homo_clip::on_bottom_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    clip_bottom = arg1.toInt();
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat res = clipimg(tmp);
    showimg(res);
}

void homo_clip::on_left_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    clip_left = arg1.toInt();
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat res = clipimg(tmp);
    showimg(res);
}

void homo_clip::on_right_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    clip_right = arg1.toInt();
    Mat_<unsigned short> tmp = Mat_<unsigned short>(rawimg.rows,rawimg.cols,CV_16UC1);
    rawimg.copyTo(tmp);
    homotransfer(tmp,degree);
    Mat res = clipimg(tmp);
    showimg(res);
}


void homo_clip::on_clipname_textEdited(const QString &arg1)
{
    if (ifinitial) return;
    clip_name = arg1;
    clip_cnt = 0;
}
