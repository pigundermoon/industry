#include "algorithm.h"
#include "ui_hist_hdr.h"


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
