#ifndef HIST_H
#define HIST_H

#include "QImage"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

QImage mat2qimage(cv::Mat_<unsigned char> &img, int type=0);



#endif // HIST_H

