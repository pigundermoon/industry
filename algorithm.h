#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "QImage"
#include "QPoint"
#include "QtConcurrent/QtConcurrent"
#include "algorithm"
#include "QString"
#include "QStringList"
#include "fstream"
#include "QDateTime"
#include "QStringList"
#include "vector"
#include "QDebug"
#include "ZQ_StructureFromTexture.h"
#include "ZQ_StructureFromTextureOptions.h"
#include "ZQ_ImageIO.h"
#include <time.h>
#include <iostream>


using namespace ZQ;
using namespace cv;
using namespace std;

#ifndef ALGORITHM
#define ALGORITHM

class rawfile : public QObject
{
    Q_OBJECT
public:
    void readfile(QString filepath);
    void resavedcm(QString filepath, QDateTime dt, QString tgtpath);
    QString scanid;
    int height;
    int width;
    cv::Mat_<unsigned short> img;
};

extern bool ifinvert;

extern void homotransfer(Mat_<unsigned short> & s, float degree);

extern void emboss(Mat_<unsigned short> &s, int dis, int range);



//操作序列解析
//$0:1,2,3,4($分隔操作，：分隔函数名与参数，，分隔参数)
extern cv::Mat_<unsigned short> parse_operationstr(QString operation, cv::Mat_<unsigned short> img);



//自动设定窗宽窗位算法
extern void auto_reset_graparas(Mat_<unsigned short>srcimgshort,unsigned short &indark,unsigned short &inwhite);

//色阶调整算法
#define kernelSize 4;
#define pi 3.14159265;
#define RGB 0
#define RED 1
#define GREEN 2
#define BLUE 3

extern unsigned short indark, inwhite, outdark, outwhite;
extern int inout_interval;
extern void getLevelMap(unsigned short map[65536], unsigned short inputDark,unsigned short inputGray, unsigned short inputLight,  unsigned short outDark, unsigned short outLight);
extern void curve(Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);
extern void divcurve(int istart,int iend,int jstart,int jend, Mat_<unsigned short>  input, Mat_<unsigned short> & output,unsigned short bMap[65536]);
extern void levelAdjustment(Mat_<unsigned short>  input, Mat_<unsigned short> & output, unsigned short inputDark, unsigned short inputGray,unsigned short inputLight,  unsigned short outDark, unsigned short outLight);


//金字塔分割
extern Mat_<unsigned short> doPyrSegmentation( Mat_<unsigned short> src);

//去噪算法
//双边滤波
extern Mat_<unsigned short> bidenoise(Mat img);
//傅里叶
extern Mat_<unsigned short> fdenoise(Mat img,float D0 = 2 * 200 * 400);
//RTV
extern Mat_<unsigned short> RTV(Mat_<unsigned short> img, int degree);

//extern Mat_<unsigned short> RTV_denoise(Mat_<unsigned short> input, float lambda = 0.01, float sigma = 3, float sharpness = 0.02, int maxIter = 4);
//extern void computeTextureWeights(Mat_<float> fin, float sigma, float sharpness, Mat_<float> wtbx, Mat_<float> wtby);
//extern Mat_<float> libfilter(Mat_<float> fin, float sigma);
//extern Mat_<float> solveLinearEquation(Mat_<float> IN, Mat_<float> wx, Mat_<float> wy, float lambda);















#endif // ALGORITHM

