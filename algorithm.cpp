#include "algorithm.h"
#include "ui_hist_hdr.h"


extern bool ifinvert = false;

//ˮƽ��ת
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

//��ֱ��ת
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



//�Աȶȵ���
//�����ͬ��ǿ�ȷֱ𾭹�+100��-50�ĶԱȶȵ������ֵ
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


//���ģ��PS�Աȶ� ����PS����ĵ�����Χ-50<=contrast<=100
void tcontrast(Mat_<unsigned short> & input, Mat_<unsigned short> & output, int contrast)
{
    int upbound[65536];
    int lowbound[65536];
    for (int i=0;i<65536;i++)
    {
        upbound[i]=int(double(tc_up_bound[int(double(i)/65535*255)])/255*65535);
        lowbound[i]=int(double(tc_low_bound[int(double(i)/65535*255)])/255*65535);
    }
    input.copyTo(output);
    if (contrast == 0) return;
    contrast = MAX(-100, MIN(100, contrast));

    double ratio = abs(contrast) / 100.0;
    //�����̶ȹ���contrast�������Եģ��ö�����������ϣ��õ��仯����ratio
    ratio = 0.4 * ratio + 0.6 * (1 - (1 - ratio) * (1 - ratio));

    //���Ե����Աȶ�
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


//���б任
extern void homotransfer(Mat_<unsigned short> & s, float degree)
{
    qDebug("%f\n",degree);
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


//����
extern void emboss(Mat_<unsigned short> &s, int dis, int range)
{
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
    QString fullfilename = filepath.split('\\').back();
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
    QString fullfilename = filepath.split('\\').back();
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

//�������н���
//$0:0.1,0.2,0.3,0.4($�ָ����������ָ�����������������ָ�����)
//0:���� 1��ˮƽ��ת 2����ֱ��ת 3:����ɫ�� 4:�����Աȶ� 5:���б任 6:����
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
                ifinvert = !ifinvert;
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
                QStringList paralist = cur_para.split(",");

                QString tmp = paralist.at(0);
                if (tmp.isEmpty())
                {
                    cerr<<"contrast para wrong!"<<endl;
                    return img;
                }
                contrast_level = QString::fromLocal8Bit(tmp.toLocal8Bit().data()).toInt();
                tcontrast(img,img,contrast_level);
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
            else
            {
                cerr<<"operation type error!"<<endl;
                return img;
            }
        }
    }

    return img;
}



//ɫ�׵���
extern unsigned short indark=0;
extern unsigned short inwhite=65535;
extern unsigned short outdark=0;
extern unsigned short outwhite=65535;
extern int inout_interval=10;

//�������ɫӳ���
void getLevelMap(unsigned short map[65536], unsigned short inputDark,unsigned short inputGray, unsigned short inputLight,  unsigned short outDark, unsigned short outLight)
{
    //�������������ʹ:
    //0<=inputDark<inputGray<inputLight<=255
    //0<=outDark,outLight<=65535
    inputDark = MIN(MAX(inputDark, 0), 65533);
    inputLight = MAX(MIN(MAX(inputLight, 2), 65535),inputDark+2);
    inputGray = MIN(MAX(inputGray, inputDark + 1), inputLight - 1);
    outDark = MIN(MAX(outDark, 0), 65535);
    outLight = MIN(MAX(outLight, 0), 65535);

    double gamma = log(0.5) / log((double)(inputGray - inputDark) / (inputLight - inputDark));

    //����ӳ�����inputGray��inputDark��inputLight���м�ֵʱ��gamma=1,�˻�Ϊ�򵥵�����ӳ�䡢
    //��inputDark���µ�ֵ��Ϊ0
    for (int i = 0; i < inputDark; i++) map[i] = outDark;
    for (int i = inputDark; i <= inputLight; i++)
    {
        //��inputDark��inputLight֮���ֵ����gammaӳ�䵽0-65535֮��
        //Ȼ���ٹ�һ����outDark~outLight֮��
        double temp = outDark + (outLight - outDark) * pow(((i*1.0 - inputDark) / (inputLight - inputDark)), gamma);
        map[i] = unsigned short(MIN(MAX(temp, 0), 65535));
    }
    //��inputLight���ϵ�ֵ��Ϊ65535
    for (int i = inputLight + 1; i < 65536; i++) map[i] = outLight;
}

//��RGB��ͨ���ֱ�ʹ��rMap��gMap��bMap��ӳ���
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

//ɫ�ײ���
//PS����ɫ�׵ĺڰ׻������α�ֱ��Ӧ�����inputDark��inputLight��inputGray
//PS���ɫ�׵ĺڰ������α�ֱ��Ӧ�����outDark��outLight
//channel: 0��1��2��3�ֱ����PSͨ�����RGB���졢�̺���
void levelAdjustment(Mat_<unsigned short>  input, Mat_<unsigned short> & output, unsigned short inputDark, unsigned short inputGray,unsigned short inputLight,  unsigned short outDark, unsigned short outLight)
{
    unsigned short curveMap[65536];	//����ֱ��ͼ��ӳ���
    getLevelMap(curveMap, inputDark,inputGray, inputLight,  outDark, outLight);	//��õ���ֱ��ͼ��ӳ���

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



//ȥ���㷨

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
