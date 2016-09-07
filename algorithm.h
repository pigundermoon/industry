
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QImage"
#include "QPoint"
#include "QtConcurrent/QtConcurrent"
#include "algorithm"
#include "QString"
#include "QStringList"


using namespace cv;
using namespace std;

#ifndef ALGORITHM
#define ALGORITHM


extern bool ifinvert;

//操作序列解析
//$0:1,2,3,4($分隔操作，：分隔函数名与参数，，分隔参数)
extern cv::Mat_<unsigned short> parse_operationstr(QString operation, cv::Mat_<unsigned short> img);


















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


//去噪算法

//extern Mat_<unsigned short> RTV_denoise(Mat_<unsigned short> input, float lambda = 0.01, float sigma = 3, float sharpness = 0.02, int maxIter = 4);
//extern void computeTextureWeights(Mat_<float> fin, float sigma, float sharpness, Mat_<float> wtbx, Mat_<float> wtby);
//extern Mat_<float> libfilter(Mat_<float> fin, float sigma);
//extern Mat_<float> solveLinearEquation(Mat_<float> IN, Mat_<float> wx, Mat_<float> wy, float lambda);















#endif // ALGORITHM

