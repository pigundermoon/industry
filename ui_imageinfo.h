#ifndef UI_IMAGEINFO_H
#define UI_IMAGEINFO_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

namespace Ui {
class ui_imageinfo;
}

class ui_imageinfo : public QDialog
{
    Q_OBJECT

public:
    explicit ui_imageinfo(QWidget *parent = 0);
    ~ui_imageinfo();
private slots:
    void r_imageshort(cv::Mat_<unsigned short> a);
private:
    Ui::ui_imageinfo *ui;
};

#endif // UI_IMAGEINFO_H
