#ifndef UI_HIST_HDR_H
#define UI_HIST_HDR_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QCloseEvent"

extern int hist_hdr_div_w;
extern int hist_hdr_div_h;



namespace Ui {
class ui_hist_hdr;
}

class ui_hist_hdr : public QDialog
{
    Q_OBJECT

public:
    explicit ui_hist_hdr(QWidget *parent = 0);
    ~ui_hist_hdr();
private slots:
    void r_imageshort(cv::Mat_<unsigned short> a);


    void on_ok_button_clicked();

    void on_cancel_button_clicked();

    void on_horizontalSlider_valueChanged(int position);

    void on_shownum_editingFinished();

    void on_minus_clicked();

    void on_plus_clicked();

signals:
   void s_imagechar(cv::Mat_<unsigned char>);
   void s_cancel();
   void s_ok(cv::Mat_<unsigned short>, QString opt);
protected:
   void closeEvent(QCloseEvent *);
private:
    Ui::ui_hist_hdr *ui;
    cv::Mat_<unsigned short> srcimg;
    cv::Mat_<unsigned char> outputimg;

    float value;
    void histogramTonemapping(cv::Mat img, cv::Mat_<unsigned char> &dst, float _contrast = 1.0f, float _anti_constrast = 0.01f);
};

#endif // UI_HIST_HDR_H
