
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QImage"
#include "QPoint"
#include "QtConcurrent/QtConcurrent"
#include "algorithm"

using namespace cv;

#ifndef ALGORITHM
#define ALGORITHM


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

