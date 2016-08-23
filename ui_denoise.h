#ifndef UI_DENOISE_H
#define UI_DENOISE_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QCloseEvent"

namespace Ui {
class ui_denoise;
}

class ui_denoise : public QDialog
{
    Q_OBJECT

public:
    explicit ui_denoise(QWidget *parent = 0);
    ~ui_denoise();
private slots:
    void r_imageshort(cv::Mat_<unsigned short> a);

    void on_windowsize_valueChanged(int value);

    void on_smooth_valueChanged(int value);

    void on_smoothedit_editingFinished();

    void on_ok_button_clicked();

    void on_cancel_button_clicked();

    void on_windowsizeedit_editingFinished();

signals:
    void s_imagechar(cv::Mat_<unsigned char>);
    void s_imageshort(cv::Mat_<unsigned short>);
    void s_cancel();
    void s_ok(cv::Mat_<unsigned short>);
private:
    Ui::ui_denoise *ui;
    cv::Mat outputimg;
};

#endif // UI_DENOISE_H
