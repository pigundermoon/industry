#include "algorithm.h"
#include "ui_hist_hdr.h"


extern bool ifinvert = false;

//金字塔分割
Mat_<unsigned short> doPyrSegmentation(Mat_<unsigned short> src )
{
//    // 检查处理图像大小是否符合规格 (规格的解释见下)
    if (!(src.rows%2 == 0 && src.cols%2 == 0))
    {cout<<1<<endl;return src;}


    Mat_<unsigned char> t1 = Mat(src.rows,src.cols,CV_8UC1);
    for (int i=0;i<src.rows;i++)
    {
        for (int j=0;j<src.cols;j++)
        {
            t1(i,j) = double(src(i,j))/65535*255;
        }
    }
    // 定义分割参数
    int level = 2;
    double threshold1 = 30 ;
    double threshold2 = 20 ;
    CvMemStorage * stoage = cvCreateMemStorage(0) ;
    CvSeq* comp=NULL;


    IplImage* iimg = &IplImage(t1);

    // 分割
    cvPyrSegmentation(iimg,iimg, stoage,&comp,level, threshold1,threshold2);

    Mat t2 = Mat(iimg,false);


    t2.convertTo(t2,CV_16UC1);
    t2=t2*255;
    return t2;
}


//水平翻转
Mat_<unsigned short> turn_horizonal(Mat_<unsigned short> img)
{
    Mat_<unsigned short> timg=Mat_<unsigned short>(img.rows, img.cols, CV_16UC1);
    for (int i=0;i<img.rows;i++)
    {
        for (int j=0;j<img.cols;j++)
        {
            timg(i,j)=img(i,img.cols-1-j);
        }
    }
    return timg;

}

//竖直翻转
Mat_<unsigned short> turn_vertical(Mat_<unsigned short> img)
{
    Mat_<unsigned short> timg=Mat_<unsigned short>(img.rows, img.cols, CV_16UC1);
    for (int i=0;i<img.rows;i++)
    {
        for (int j=0;j<img.cols;j++)
        {
            timg(i,j)=img(img.rows-1-i,j);
        }
    }
    return timg;

}



//对比度调整
//打表，不同的强度分别经过+100、-50的对比度调整后的值
int tc_up_bound[256] = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 13, 13, 14,
14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 54, 55, 56, 57, 59, 60, 61, 62, 63, 65, 66, 67, 69, 70, 71, 73, 74, 75, 77, 78,
79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 94, 95, 97, 99, 100, 102, 103, 105, 106, 108, 110, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 128,
130, 132, 134, 135, 137, 139, 140, 142, 144, 145, 147, 149, 150, 152, 153, 155, 156, 158, 160, 161, 163, 164, 166, 167, 169, 170, 171, 173, 174, 176, 177, 178,
180, 181, 182, 184, 185, 186, 188, 189, 190, 192, 193, 194, 195, 196, 198, 199, 200, 201, 202, 203, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216,
217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 226, 227, 228, 229, 230, 231, 231, 232, 233, 234, 234, 235, 236, 237, 237, 238, 239, 239, 240, 241, 241, 242,
242, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252, 253, 253, 253, 254, 254, 254, 254, 255, 255 };
int tc_low_bound[256] = { 0, 1, 3, 4, 5, 7, 8, 10, 11, 12, 14, 15, 16, 17, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31, 33, 34, 35, 36, 38, 39, 40, 41,
42, 43, 45, 46, 47, 48, 49, 50, 52, 53, 54, 55, 56, 57, 58, 59, 60, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 98, 99, 100, 101, 102, 103, 103, 104, 105,
106, 107, 107, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 116, 117, 118, 119, 119, 120, 121, 121, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128,
128, 129, 130, 130, 131, 132, 132, 133, 134, 134, 135, 136, 136, 137, 138, 139, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 147, 148, 148, 149, 150, 151,
152, 152, 153, 154, 155, 156, 157, 157, 158, 159, 160, 161, 162, 163, 164, 165, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 195, 196, 197, 198, 199, 200, 201, 202, 203, 205, 206, 207, 208, 209, 210, 212, 213, 214, 215,
216, 217, 219, 220, 221, 222, 224, 225, 226, 227, 229, 230, 231, 232, 234, 235, 236, 238, 239, 240, 241, 243, 244, 245, 247, 248, 250, 251, 252, 254, 255 };


//打表法模拟PS对比度 按照PS里面的调整范围-50<=contrast<=100
void tcontrast(Mat_<unsigned short> & input, Mat_<unsigned short> & output, int contrast, int point = 32768)
{
    int upbound[65536];
    int lowbound[65536];

    input.copyTo(output);
    if (contrast == 0) return;
    contrast = MAX(-100, MIN(100, contrast));


    for (int i=0;i<65536;i++)
    {
        upbound[i]=int(double(tc_up_bound[int(double(i)/65535*255)])/255*65535);
        lowbound[i]=int(double(tc_low_bound[int(double(i)/65535*255)])/255*65535);
    }

    if (point != 32768)
    {
        if (point<1) point =1;
        if (point >65534) point = 65534;
        for (int i=0; i<65536;i++)
        {
            if (i<=point)
            {
                int correpoint = float(float(i*32768)/float(point))/255;
                upbound[i] = float(tc_up_bound[correpoint]*point)/32768;
                if (upbound[i]<0) upbound[i]=0;
                if (upbound[i]>255) upbound[i] = 255;
                upbound[i]*=255;
                lowbound[i] = float(tc_low_bound[correpoint]*point)/32768;
                if (lowbound[i]<0) lowbound[i]=0;
                if (lowbound[i]>255) lowbound[i] = 255;
                lowbound[i]*=255;
            }
            else
            {
                int correpoint =  float( (float(i-point)/float(65535-point))*32767+32768) / 255;
                upbound[i] = float(tc_up_bound[correpoint]-128)*float(65535-point)/32767+float(point)/255;
                if (upbound[i]<0) upbound[i]=0;
                if (upbound[i]>255) upbound[i] = 255;
                upbound[i]*=255;
                lowbound[i] =float(tc_low_bound[correpoint]-128)*float(65535-point)/32767+float(point)/255;
                if (lowbound[i]<0) lowbound[i]=0;
                if (lowbound[i]>255) lowbound[i] = 255;
                lowbound[i]*=255;
            }
        }
    }



    double ratio = abs(contrast) / 100.0;
    //调整程度关于contrast不是线性的，用二次曲线来拟合，得到变化比率ratio
    ratio = 0.4 * ratio + 0.6 * (1 - (1 - ratio) * (1 - ratio));

    //线性调整对比度
    unsigned short vmap[65536];
    for (int i = 0; i < 65536; i++){
        if (contrast > 0) {
            vmap[i] = round(i + (upbound[i] - i) * ratio);
        }
        else{
            vmap[i] = round(i + (lowbound[i] - i) * ratio);
        }
    }

    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
        {
            output(i, j) =vmap[input(i,j)] ;
        }


}

//双边滤波
extern Mat_<unsigned short> bidenoise(Mat_<unsigned short> img)
{
    cv::Mat temp=cv::Mat(img.size(),CV_32F);
    for (int i=0;i<img.rows;i++)
    {
        for (int j=0;j<img.cols;j++)
        {
            float t=float(img(i,j));
            temp.at<float>(i,j)=float(img(i,j));
            t=temp.at<float>(i,j);
        }
    }
    cv::Mat temp1;

    cv::bilateralFilter(temp, temp1, 20, 500.0f, 250.0f);

    temp1.convertTo(img,CV_16UC1);
    return img;

}

//傅里叶变换
extern Mat_<unsigned short> fdenoise(Mat img,float D0)
{
    Mat dftInput1, dftImage1, inverseDFT, inverseDFTconverted;
    img.convertTo(dftInput1, CV_32F);

    for (int i = 0; i<dftInput1.rows; i++)        //中心化
    {
        float *p = dftInput1.ptr<float>(i);
        for (int j = 0; j<dftInput1.cols; j++)
        {
            p[j] = p[j] * pow(-1, i + j);
        }
    }

    dft(dftInput1, dftImage1, cv::DFT_COMPLEX_OUTPUT);	 // Applying DFT

    //高斯低通滤波， 高斯高通滤波
    //multiply(complexI, gaussianBlur, gaussianBlur);
    //multiply(complexI, gaussianSharpen, gaussianSharpen);

    Mat gaussianBlur(dftImage1.size(), CV_32FC2);

    for (int i = 0; i<dftImage1.rows; i++)
    {
        float *p = gaussianBlur.ptr<float>(i);
        for (int j = 0; j<dftImage1.cols; j++)
        {
            float d = pow(i - dftImage1.rows / 2, 2) + pow(j - dftImage1.cols / 2, 2);
            p[2 * j] = expf(-d / D0);
            p[2 * j + 1] = expf(-d / D0);
        }
    }

    //高斯低通滤波
    multiply(dftImage1, gaussianBlur, dftImage1);

    ////傅里叶反变换
    idft(gaussianBlur, gaussianBlur, CV_DXT_INVERSE);
    //dft(gaussianSharpen, gaussianSharpen, CV_DXT_INVERSE);

    // Reconstructing original imae from the DFT coefficients
    idft(dftImage1, inverseDFT, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT); // Applying IDFT

    for (int i = 0; i<inverseDFT.rows; i++)        //中心化
    {
        float *p = inverseDFT.ptr<float>(i);
        for (int j = 0; j<inverseDFT.cols; j++)
        {
            p[j] = p[j] * pow(-1, i + j);
        }
    }

    inverseDFT = inverseDFT;

    inverseDFT.convertTo(inverseDFTconverted, CV_16U);
    return inverseDFTconverted;

//    imshow("Output", inverseDFTconverted);

//    //show the image
//    imshow("Original Image", img);
//    // Wait until user press some key
//    waitKey(0);

//    imwrite("result.tiff", inverseDFTconverted);
}

//RTV
extern Mat_<unsigned short> RTV(Mat_<unsigned short> img, int degree)
{
    ZQ_DImage<double> input,output;

    if(!ZQ_ImageIO::loadMat(input,img))
    {
        qDebug("failed to load\n");
        return img;
    }

    char** argv[3];
    char* argv1[]={"weight" , "0.05" , "methodType" , "PENALTY_GRADIENT_WEIGHT" , "penaltyweighttype" , "WEIGHT_RTV_MIX" , "nSolverIteration" , "1" , "nOuterIteration" , "1", NULL};
    char* argv2[]={"weight" , "0.1" , "methodType" , "PENALTY_GRADIENT_WEIGHT" , "penaltyweighttype" , "WEIGHT_RTV_MIX" , "nSolverIteration" , "1" , "nOuterIteration" , "1", NULL};
    char* argv3[]={"weight" , "0.2" , "methodType" , "PENALTY_GRADIENT_WEIGHT" , "penaltyweighttype" , "WEIGHT_RTV_MIX" , "nSolverIteration" , "1" , "nOuterIteration" , "1", NULL};

    argv[0] = argv1;
    argv[1] = argv2;
    argv[2] = argv3;
    ZQ_StructureFromTextureOptions opt;
    if(!opt.HandleParas(10,argv[degree]))
    {
        printf("failed to handle args\n");
        opt.showArgs();
        return img;
    }
    clock_t t1 = clock();
    if(!ZQ_StructureFromTexture::StructureFromTexture(input,output,opt))
    {
        printf("failed to extract structure from texture\n");
        return img;
    }
    clock_t t2 = clock();
    double*& im_Data = output.data();

    Mat_<unsigned short> resimg = Mat_<unsigned short>(img.rows,img.cols);
    for(int i = 0;i < img.rows;i++)
    {
        for(int j = 0;j < img.cols;j++)
        {
                resimg(i,j) = im_Data[i*img.cols+j] * 65535.0;
        }
    }
    return resimg;
}


//错切变换
extern void homotransfer(Mat_<unsigned short> & s, float degree)
{
    if (degree<=-90 || degree>=90) return ;
    Mat_<unsigned  short> src;
    if (s.rows<s.cols)
    {
        src = Mat_<unsigned short>(s.cols,s.rows,CV_16UC1);
        for (int i=0;i<src.rows;i++)
        {
            for (int j=0; j < src.cols; j++)
            {
                src(i,j) = s(s.rows-j-1,i);
            }
        }
    }
    else
    {
        src =s;
    }
    Point2f srctri[3];
    Point2f dsttri[3];

    srctri[0] = Point2f(0,0);
    srctri[1] = Point2f(src.cols/3,0);
    srctri[2] = Point2f(src.cols/2,src.rows/2);

    dsttri[0] = Point2f(0,0);
    dsttri[1] = Point2f(src.cols/3,(src.cols/3)*tan(degree/180*3.14));
    dsttri[2] = Point2f(src.cols/2,src.rows/2+((src.cols/2)*tan(degree/180*3.14)));

    Mat warp_mat( 2, 3, CV_32FC1 );
    warp_mat = getAffineTransform( srctri, dsttri);
    warpAffine( src, src, warp_mat, src.size() );

    if (s.rows<s.cols)
    {
        for (int i=0;i<src.rows;i++)
        {
            for (int j=0; j < src.cols; j++)
            {
                s(s.rows-j-1,i) = src(i,j);
            }
        }
    }
}


//浮雕化
extern void emboss(Mat_<unsigned short> &s, int dis, int range)
{
//    cout<< dis << ' ' << range << endl;
    if (dis<1) dis=1;
    if (dis>10) dis=10;
    if (range <1) range =1;
    if (range >500) range =500;

    Mat_<int> timg = Mat(s.rows,s.cols,CV_32SC1);
    for (int i=0;i<s.rows;i++)
    {
        for (int j=0;j<s.cols;j++)
        {
            if (i-dis/2>=0 && j-dis/2>=0 && i+dis-dis/2<s.rows && j+dis-dis/2<s.cols)
            {
                timg(i,j) = (int)s(i+dis-dis/2,j+dis-dis/2)-(int)s(i-dis/2,j-dis/2);
            }
            else
            {
                timg(i,j) = 0;
            }
            timg(i,j)=(timg(i,j)*float(range)/100+65535)/2;
        }
    }
    for (int i=0;i<s.rows;i++)
    {
        for (int j=0;j<s.cols;j++)
        {
            if (timg(i,j)>65535) timg(i,j) = 65535;
            if (timg(i,j)<0) timg(i,j) = 0;
            if (s(i,j)<=32768) s(i,j) = (timg(i,j)*s(i,j))/32768;
            else s(i,j) = 65535 - (65535 - timg(i,j))*(65535 - s(i,j))/32768;
        }
    }
}


void rawfile::readfile(QString filepath)
{
    QString fullfilename = filepath.split('/').back();
    QString filename = fullfilename.split('.').first();
    scanid = fullfilename.split('_').first();
    QString wandh = filename.split('_').back();
    width = QString(wandh.split('x').first()).toInt();
    height = QString(wandh.split('x').back()).toInt();


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba = filepath.toLocal8Bit();
    ifstream rfile(ba.data(),ios::binary);

    long long size = width*height*2;
    char *buffer = new char[size];
    img = cv::Mat_<unsigned short>(height,width,CV_16UC1);
    rfile.read(buffer,size);
    unsigned short *pimg = (unsigned short*) buffer;
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
        {
            img(i,j) = 65535 - pimg[i*width+j];
        }
    }
    rfile.close();
}

void rawfile::resavedcm(QString filepath, QDateTime dt, QString tgtpath)
{

    QString fullfilename = filepath.split('/').back();
    QString filename = fullfilename.split('.').first();
    scanid = fullfilename.split('_').first();
    QString wandh = filename.split('_').at(1);
    width = QString(wandh.split('x').first()).toInt();
    height = QString(wandh.split('x').back()).toInt();


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba = filepath.toLocal8Bit();
    ifstream rfile(ba.data(),ios::binary);

    long long size = width*height*2;
    char *buffer = new char[size];
    rfile.read(buffer,size);
    unsigned short *pimg = (unsigned short*) buffer;




    DcmFileFormat fileformat;
    ba = filename.toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_PatientName,ba.data());
    ba = scanid.toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_PatientID,ba.data());
    ba = dt.toString("yyyy-MM-dd").toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_StudyDate,ba.data());
    ba = dt.toString("hh:mm:ss").toLocal8Bit();
    fileformat.getDataset()->putAndInsertString(DCM_StudyTime,ba.data());
    fileformat.getDataset()->putAndInsertUint16(DCM_Rows,height);
    fileformat.getDataset()->putAndInsertUint16(DCM_Columns,width);

    Uint16* pData = new Uint16[width*height];
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
        {
            pData[i*width+j] = pimg[i*width+j];
        }
    }
    fileformat.getDataset()->putAndInsertUint16Array(DCM_PixelData,pData,width*height);
    ba = tgtpath.toLocal8Bit();
    fileformat.saveFile(ba.data(),EXS_LittleEndianImplicit,EET_UndefinedLength,EGL_withoutGL);
    rfile.close();
}


//局部hdr
extern int local_hdr_div_w;
extern int local_hdr_div_h;

static Mat src;
static Mat map_img;
static Mat tmap_img;
static cv::Mat dst;
static cv::Mat_<unsigned char> res;
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

void atlinearTonemapping(float _gamma, Mat src, Mat & dst)
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

void atmapLuminance(Mat src, Mat & dst, Mat lum, Mat new_lum, float saturation)
{
    dst = src.mul(1.0f / lum);
    pow(dst, saturation, dst);
    dst = dst.mul(new_lum);
}

void atprebilateralTonemapping1(Mat src, Mat & dst, float _gamma = 1.0f, float _contrast = 4.0f, float _saturation = 1.0f, float _sigma_color = 2.0f, float _sigma_space = 2.0f)
{

    float sigma_space = _sigma_color;
    float sigma_color = _sigma_space;


    dst = Mat(src.size(), CV_32F);
    img=Mat(src.size(), CV_32F);
    atlinearTonemapping(1.0f, src, img); //变为0-1范围内的float形


    log_(img, log_img);

    bilateralFilter(log_img, map_img, -1, sigma_color, sigma_space);


    minMaxLoc(map_img, &minv, &maxv);
    tmap=log_img-map_img*1.0f;
    ttmap=map_img/ static_cast<float>(maxv - minv);

}
void atdivhdr(int pos0,int pos1,int pos2,int pos3)
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

void athdrsolve(float _gamma = 1.0f, float _contrast = 4.0f, float _saturation = 1.0f)
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
            QtConcurrent::run(atdivhdr,position[0],position[1],position[2],position[3]);
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


//操作序列解析
//$0:0.1,0.2,0.3,0.4($分隔操作，：分隔函数名与参数，，分隔参数)
//0:正负(单独记录) 1：水平翻转 2：竖直翻转 3:调整色阶 4:调整对比度 5:错切变换 6:浮雕化 7:局部hdr 8：直方图hdr
extern cv::Mat_<unsigned short> parse_operationstr(QString operation, cv::Mat_<unsigned short> img)
{
    QStringList opset = operation.split('$');
    for(int i = 0; i < opset.size(); i++)
    {
        QString cur_op = opset.at(i);
        cur_op = QString::fromLocal8Bit(cur_op.toLocal8Bit().data());
        if (cur_op.isEmpty())
        {
            continue;
        }
        else
        {
            QStringList type_and_para = cur_op.split(':');
            QString cur_type = type_and_para.first();
            cur_type = QString::fromLocal8Bit(cur_type.toLocal8Bit().data());
            QString cur_para = type_and_para.back();
            cur_para = QString::fromLocal8Bit(cur_para.toLocal8Bit().data());
            if (cur_type == "0")
            {
                QStringList paralist = cur_para.split(",");
                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"invert para wrong!"<<endl;
                    return img;
                }
                if (tmp=="1")
                {
                    ifinvert = true;
                }
                else
                {
                    ifinvert = false;
                }
            }
            else if (cur_type == "1")
            {
                img = turn_horizonal(img);
            }
            else if (cur_type == "2")
            {
                img = turn_vertical(img);
            }
            else if (cur_type == "3")
            {
                unsigned short inputDark, inputGray, inputLight, outDark, outLight;
                QStringList paralist = cur_para.split(",");

                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"gradation para wrong!"<<endl;
                    return img;
                }
                inputDark = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toUShort();

                tmp = paralist.at(1);
                if (tmp.isEmpty())
                {
                    cerr<<"gradation para wrong!"<<endl;
                    return img;
                }
                inputGray = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toUShort();

                tmp = paralist.at(2);
                if (tmp.isEmpty())
                {
                    cerr<<"gradation para wrong!"<<endl;
                    return img;
                }
                inputLight = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toUShort();

                tmp = paralist.at(3);
                if (tmp.isEmpty())
                {
                    cerr<<"gradation para wrong!"<<endl;
                    return img;
                }
                outDark = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toUShort();

                tmp = paralist.at(4);
                if (tmp.isEmpty())
                {
                    cerr<<"gradation para wrong!"<<endl;
                    return img;
                }
                outLight = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toUShort();

                levelAdjustment(img,img,inputDark,inputGray,inputLight,outDark,outLight);
            }
            else if (cur_type == "4")
            {
                int contrast_level;
                int midpos;
                QStringList paralist = cur_para.split(",");

                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"contrast para wrong!"<<endl;
                    return img;
                }
                contrast_level = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();
                tmp = paralist.at(1);
                if (tmp.isEmpty())
                {
                    cerr<<"emboss para2 wrong!"<<endl;
                    return img;
                }
                midpos = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();
                tcontrast(img,img,contrast_level,midpos);
            }

            else if (cur_type == "5")
            {
                float degree;
                QStringList paralist = cur_para.split(",");

                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"homo para wrong!"<<endl;
                    return img;
                }
                degree = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toFloat();
                homotransfer(img,degree);
            }
            else if (cur_type == "6")
            {
                int dis,contrast;
                QStringList paralist = cur_para.split(",");

                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"emboss para1 wrong!"<<endl;
                    return img;
                }
                dis = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();

                tmp = paralist.at(1);
                if (tmp.isEmpty())
                {
                    cerr<<"emboss para2 wrong!"<<endl;
                    return img;
                }
                contrast = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();
                emboss(img,dis,contrast);
            }
            else if (cur_type == "7")
            {

                float value1,value2;
                QStringList paralist = cur_para.split(",");
                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"local hdr para1 wrong!"<<endl;
                    return img;
                }
                value1 =  QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();
                tmp = paralist.at(1);
                if (tmp.isEmpty())
                {
                    cerr<<"local hdr para2 wrong!"<<endl;
                    return img;
                }
                value2 =  QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();

                Mat srcimg=img;
                src=Mat(srcimg.size(), CV_32F);
                dst=Mat(srcimg.size(), CV_32F);
                res=Mat(srcimg.size(), CV_8UC1);
                srcimg.convertTo(src,CV_32F);

                atprebilateralTonemapping1(src, dst,value1,value2);
                athdrsolve(value1,value2);
                for (int i=0;i<res.rows;i++)
                {
                    for (int j=0;j<res.cols;j++)
                    {
                        img(i,j) = res(i,j)*255;
                    }
                }
            }
            else
            {
                cerr<<"operation type error!"<<endl;
                return img;
            }
        }
    }

    return img;
}



//自动设定窗宽窗位
void auto_reset_graparas(Mat_<unsigned short>srcimgshort,unsigned short &indark,unsigned short &inwhite)
{
    indark =0 ;
    inwhite = 65535;
    int calbin[65536] {0};
    int sumbin[65536] {0};
    int embin[65536] {0};

    for (int i=0;i<srcimgshort.rows;i++)
    {
        for (int j=0;j<srcimgshort.cols;j++)
        {
            calbin[srcimgshort(i,j)]++;
        }
    }
    for (int i=0;i<512;i++) calbin[i] = 0;
    for (int i=65000;i<65536;i++) calbin[i] = 0;
    int total = srcimgshort.cols*srcimgshort.rows;
    int st0 = 0, st1 = 0, ed0 = 0, ed1 = 1;
    for (int i=10;i<65535;i++)
    {
        sumbin[i] = sumbin[i-1]+calbin[i];
    }
    for (int i=10;i<65500;i++)
    {
        embin[i] = sumbin[i+20]-sumbin[i];
    }
    for (int i=2000;i<65500;i++)
    {
        if(embin[i]>=80&&embin[i-1]<80)
        {
            st1 = i;
            ed1 = i+1;
        }
        else if(embin[i]>=80&&embin[i-1]>=80)
        {
            ed1 = i;
        }
        else if(embin[i]<80&&embin[i-1]>=80)
        {
            if (ed1 - st1 > ed0 - st0)
            {
                st0=st1;
                ed0=ed1;
//                 cout<<"1 "<<st0<<' '<<ed0<<endl;
            }
        }
    }
//    cout<<"2 "<<st0<<' '<<ed0<<endl;
    if (abs(ed0 - st0)<1000 || abs(ed0 - st0)>60000)
    {
        for (int i=2000;i<65535;i++)
        {
            if (sumbin[i]>0.05*total)
            {
                st0 = i;
                break;
            }
        }
        for (int i=65535;i>=0;i--)
        {
            if (sumbin[i]<0.95*total)
            {
                ed0 = i;
                break;
            }
        }
    }
//     cout<<"3 "<<st0<<' '<<ed0<<endl;
    indark = st0;
    inwhite = ed0;

}

//色阶调整
extern unsigned short indark=0;
extern unsigned short inwhite=65535;
extern unsigned short outdark=0;
extern unsigned short outwhite=65535;
extern int inout_interval=10;

//计算出颜色映射表
void getLevelMap(unsigned short map[65536], unsigned short inputDark,unsigned short inputGray, unsigned short inputLight,  unsigned short outDark, unsigned short outLight)
{
    //调整输入参数，使:
    //0<=inputDark<inputGray<inputLight<=255
    //0<=outDark,outLight<=65535
    inputDark = MIN(MAX(inputDark, 0), 65533);
    inputLight = MAX(MIN(MAX(inputLight, 2), 65535),inputDark+2);
    inputGray = MIN(MAX(inputGray, inputDark + 1), inputLight - 1);
    outDark = MIN(MAX(outDark, 0), 65535);
    outLight = MIN(MAX(outLight, 0), 65535);

    double gamma = log(0.5) / log((double)(inputGray - inputDark) / (inputLight - inputDark));

    //计算映射表，当inputGray是inputDark和inputLight的中间值时，gamma=1,退化为简单的线性映射、
    //将inputDark以下的值置为0
    for (int i = 0; i < inputDark; i++) map[i] = outDark;
    for (int i = inputDark; i <= inputLight; i++)
    {
        //将inputDark到inputLight之间的值按照gamma映射到0-65535之间
        //然后再归一化到outDark~outLight之间
        double temp = outDark + (outLight - outDark) * pow(((i*1.0 - inputDark) / (inputLight - inputDark)), gamma);
        map[i] = unsigned short(MIN(MAX(temp, 0), 65535));
    }
    //将inputLight以上的值置为65535
    for (int i = inputLight + 1; i < 65536; i++) map[i] = outLight;
}

//对RGB三通道分别使用rMap、gMap和bMap的映射表
void curve(Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536])
{

    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
        {
            output(i, j) = bMap[input(i, j)];
        }
}

void divcurve(int istart,int iend,int jstart,int jend, Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536])
{
    for (int i = istart; i < iend; i++)
        for (int j = jstart; j < jend; j++)
        {
            output(i, j) = bMap[input(i, j)];
        }
}

//色阶操作
//PS输入色阶的黑白灰三个游标分别对应这里的inputDark、inputLight和inputGray
//PS输出色阶的黑白两个游标分别对应这里的outDark和outLight
//channel: 0、1、2、3分别代表PS通道里的RGB、红、绿和蓝
void levelAdjustment(Mat_<unsigned short>  input, Mat_<unsigned short> & output, unsigned short inputDark, unsigned short inputGray,unsigned short inputLight,  unsigned short outDark, unsigned short outLight)
{
    unsigned short curveMap[65536];	//调整直方图的映射表
    getLevelMap(curveMap, inputDark,inputGray, inputLight,  outDark, outLight);	//获得调整直方图的映射表

    curve(input, output, curveMap);
//    int di=input.rows/div_h;
//    int dj=input.cols/div_w;
//    for (int iw=0;iw<div_w;iw++)
//    {
//        for (int ih=0;ih<div_h;ih++)
//        {
//            int istart=(ih==0?0:(ih*di+1));
//            int iend=(ih==div_h?(input.rows-1):(ih+1)*di);
//            int jstart=(iw==0?0:(iw*dj+1));
//            int jend=(iw==div_w?(input.cols-1):(iw+1)*dj);
//            QtConcurrent::run(a,di);
//        }
//    }
}



//去噪算法

//Mat_<unsigned short> RTV_denoise(Mat_<unsigned short> input, float lambda = 0.01, float sigma = 3, float sharpness = 0.02, int maxIter = 4)
//{
//    Mat I = Mat(input.size(),CV_32F);
//    for (int i = 0; i < input.rows; i++)
//    {
//        for (int j=0; j < input.cols; j++)
//        {
//            I(i,j) = ((float)input(i,j))/65535;
//        }
//    }
//    Mat x;
//    I.copyTo(x);
//    float sigma_iter = sigma;
//    lambda = lambda/2.0;
//    float dec = 2.0;
//    for (int iter = 1; iter <= maxIter; iter++)
//    {
//        Mat wx = Mat(x.size(),CV_32F);
//        Mat wy = Mat(x.size(),CV_32F);
//        computeTextureWeights(x,sigma_iter,sharpness,wx,wy);
//        x=solveLinearEquation(I,wx,wy,lambda);





//    }

//}

//Mat_<float> solveLinearEquation(Mat_<float> IN, Mat_<float> wx, Mat_<float> wy, float lambda)
//{
//    int r = IN.rows;
//    int c = IN.cols;
//    int k = r*c;
//    Mat_<float> dx;
//    dx.create(k,1,CV_32F);
//    Mat_<float> B;
//    B.create(k,2,CV_32F);
//    for (int i=0; i<c; i++)
//    {
//        for (int j=0; j<r; j++)
//        {
//            dx(i*r+j)=-lambda*wx(j,i);
//            B(i*r+j,1)=-lambda*wx(j,i);
//        }
//    }

//    Mat_<float> dy;
//    dy.create(k,1,cv_32F);
//    for (int i=0; i<c; i++)
//    {
//        for (int j=0; j<r; j++)
//        {
//            dy(i*r+j)=-lambda*wy(j,i);
//            B(i*r+j,2)=-lambda*wy(j,i);
//        }
//    }

//    Mat_<float> d;
//    d.create(1,2,CV_32F);
//    d(1,1)=-r;d(1,2)=-1;




//}

//void computeTextureWeights(Mat_<float> fin, float sigma, float sharpness, Mat_<float> wtbx, Mat_<float> wtby)
//{
//    Mat fx;
//    fx.create(fin.rows,fin.cols,cv_32F);
//    for (int i=0;i<fin.rows;i++)
//    {
//        for (int j=1; j<fin.cols;j++)
//        {
//            fx(i,j-1)=fin(i,j)-fin(i,j-1);
//        }
//        fx(i,fin.cols-1)=0;
//    }

//    Mat fy;
//    fy.create(fin.rows,fin.cols,cv_32F);
//    for (int j=0;j<=fin.rows;j++)
//    {
//        for (int i=1;i<fin.cols;i++)
//        {
//            fy(i-1,j)=fin(i,j)-fin(i-1,j);
//        }
//        fy(fin.cols-1,j)=0;
//    }

//    float vareps_s = sharpness;
//    float vareps = 0.001;

//    Mat wto = Mat(fin.size(),CV_32F);
//    wto = fx.mul(fx)+fy.mul(fy);
//    for (int i=0; i<wto.rows; i++ )
//    {
//        for (int j=0; j<wto.cols; j++)
//        {
//            if (wto(i,j) < vareps_s) wto(i,j) = vareps_s;
//            wto(i,j) = 1/wto(i,j);
//        }
//    }

//    Mat fbin;
//    fbin =libfilter(fin, sigma);

//    for (int i=0;i<fbin.rows;i++)
//    {
//        for (int j=1; j<fbin.cols;j++)
//        {
//            fx(i,j-1)=fbin(i,j)-fbin(i,j-1);
//        }
//        fx(i,fbin.cols-1)=0;
//    }
//    for (int j=0;j<fbin.rows;j++)
//    {
//        for (int i=1;i<fbin.cols;i++)
//        {
//            fy(i-1,j)=fbin(i,j)-fbin(i-1,j);
//        }
//        fy(fbin.cols-1,j)=0;
//    }


//    for (int i=0; i<wtbx.rows; i++ )
//    {
//        for (int j=0; j<wtbx.cols; j++)
//        {
//            if (fx(i,j) < vareps) fx(i,j) = vareps;
//            wtbx(i,j) = 1/fx(i,j);
//        }
//    }
//    for (int i=0; i<wtby.rows; i++ )
//    {
//        for (int j=0; j<wtby.cols; j++)
//        {
//            if (fy(i,j) < vareps) fy(i,j) = vareps;
//            wtby(i,j) = 1/fy(i,j);
//        }
//    }
//    wtbx=wtbx.mul(wto);
//    wtby=wtby.mul(wto);

//    for (int i=0;i<wtbx.rows;i++)
//    {
//        wtbx(i,wtbx.cols-1)=0;
//    }
//    for (int j=0;j<wtby.cols;j++)
//    {
//        wtby(wtby.rows-1,j)=0;
//    }

//}


//Mat_<float> libfilter(Mat_<float> fin, float sigma)
//{
//    int ksize = (round(5*sigma) | 1);

//    int c = ksize / 2;
//    Mat K(1, ksize, CV_32FC1);
//    float s2 = 2.0 * sigma * sigma;
//    for(int i = (-c); i <= c; i++)
//    {
//        int m = i + c;
//        for (int j = (-c); j <= c; j++)
//        {
//            int n = j + c;
//            float v = exp(-(1.0*i*i + 1.0*j*j) / s2);
//            K.ptr<double>(m)[n] = v;
//        }
//    }
//    Scalar all = sum(K);
//    Mat gaussK;
//    K.convertTo(gaussK, CV_32FC1, (1/all[0]));
//    all = sum(gaussK);

//    Mat fbin;
//    cv::Point anchor(-1, -1);
//    filter2D(fin,fbin,CV_32F,gaussK,anchor,0,cv::BORDER_REFLECT101);
//    filter2D(fbin,fbin,CV_32F,gaussK,anchor,0,cv::BORDER_REFLECT101);

//    return fbin;

//}
