#ifndef DCMTKFILE_H
#define DCMTKFILE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "QDebug"
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QTextCodec"
#include "string"


using namespace std;

class dcmtkfile
{
public:
    dcmtkfile();
    bool checkfile(QString path);
    cv::Mat loadfile(QString path);
    Uint16 height;
    Uint16 width;
    OFString name;
    OFString id;
    OFString date;
    OFString time;


    bool flag;
};

#endif // DCMTKFILE_H
