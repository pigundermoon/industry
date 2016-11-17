#ifndef SHOWIMGWINDOW_H
#define SHOWIMGWINDOW_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "hist.h"
#include "QImage"
#include "QPaintEvent"
#include "QResizeEvent"
#include "QMouseEvent"
#include "QGraphicsDropShadowEffect"

using namespace cv;
using namespace std;

namespace Ui {
class showimgwindow;
}

class showimgwindow : public QDialog
{
    Q_OBJECT

public:
    explicit showimgwindow(QWidget *parent = 0);
    ~showimgwindow();

private slots:
    void r_img(QImage img);

signals:
    void s_bigshow_close();

private:
    Ui::showimgwindow *ui;
    QImage qshowimg;
    void showimg();

protected:
    void resizeEvent(QResizeEvent * event);
    void closeEvent(QCloseEvent *event);
};

#endif // SHOWIMGWINDOW_H
