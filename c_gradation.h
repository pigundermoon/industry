#ifndef C_GRADATION_H
#define C_GRADATION_H



#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QMouseEvent"
#include "QCloseEvent"
#include "algorithm.h"



namespace Ui {
class c_gradation;
}

class c_gradation : public QDialog
{
    Q_OBJECT

public:
    explicit c_gradation(QWidget *parent = 0);
    ~c_gradation();
    void getparent(QWidget *pp);

private slots:
    void r_imageshort(cv::Mat_<unsigned short> a);

    void on_b_cancel_clicked();

    void on_b_ok_clicked();

    void r_hist(cv::Mat_<unsigned char> a);

    void on_outb_poschanged(int num);

    void on_outw_poschanged(int num);

    void on_input_black_valueChanged(int arg1);

    void on_input_gray_valueChanged(int arg1);

    void on_input_white_valueChanged(int arg1);

    void on_output_black_valueChanged(int arg1);

    void on_outpu_white_valueChanged(int arg1);

    void on_inb_poschanged(int num);

    void on_ing_poschanged(int num);

    void on_inw_poschanged(int num);

signals:
   void s_imagechar(cv::Mat_<unsigned char>);
   void s_imageshort(cv::Mat_<unsigned short>);
   void s_cancel();
   void s_ok(cv::Mat_<unsigned short>);
protected:
   void closeEvent(QCloseEvent *);
private:
    Ui::c_gradation *ui;
    cv::Mat_<unsigned short> srcimg;
    cv::Mat_<unsigned short> output;
    double rate;
    int flag;
//    void mouseMoveEvent(QMouseEvent *ev);
};

#endif // C_GRADATION_H
