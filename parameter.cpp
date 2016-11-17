#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QString"

cv::Mat_<unsigned short> tempimg;
int hist_hdr_div_w=1;
int hist_hdr_div_h=30;

int local_hdr_div_w=1;
int local_hdr_div_h=30;

int maxback=3;


//归档时错切变换参数，裁剪参数
extern bool auto_homotransfer=false;
extern bool auto_clip=false;
extern bool auto_name = false;

extern float degree = 0.0;
extern int clip_left = 0;
extern int clip_top = 0;
extern int clip_right = 0;
extern int clip_bottom = 0;
extern int clip_cnt = 0;
extern QString clip_name = "";

