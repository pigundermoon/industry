#ifndef UI_LOCAL_HDR_H
#define UI_LOCAL_HDR_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QCloseEvent"

namespace Ui {
class ui_local_hdr;
}

class ui_local_hdr : public QDialog
{
    Q_OBJECT

public:
    explicit ui_local_hdr(QWidget *parent = 0);
    ~ui_local_hdr();
private slots:
    void r_imageshort(cv::Mat_<unsigned short> a);


    void on_ok_button_clicked();

    void on_cancel_button_clicked();

    void on_v1_valueChanged(int value);

    void on_v1text_editingFinished();

    void on_v2_valueChanged(int value);

    void on_v2text_editingFinished();

    void on_v1_sliderReleased();

    void on_v2_sliderReleased();

    void on_minus_clicked();

    void on_plus_clicked();

    void on_minus_2_clicked();

    void on_plus_2_clicked();

signals:
   void s_imagechar(cv::Mat_<unsigned char>);
   void s_cancel();
   void s_ok(cv::Mat_<unsigned short>);
   void s_dst(cv::Mat);
protected:
   void closeEvent(QCloseEvent *);
private:
    Ui::ui_local_hdr *ui;
    cv::Mat_<unsigned short> srcimg;
    float value1;
    float value2;
};

#endif // UI_LOCAL_HDR_H
