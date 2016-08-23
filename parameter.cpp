#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

cv::Mat_<unsigned short> tempimg;
int hist_hdr_div_w=1;
int hist_hdr_div_h=30;

int local_hdr_div_w=1;
int local_hdr_div_h=30;

int maxback=3;
