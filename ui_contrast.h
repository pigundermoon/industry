#ifndef UI_CONTRAST_H
#define UI_CONTRAST_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QCloseEvent"
#include "QDebug"

using namespace std;

namespace Ui {
class ui_contrast;
}

class ui_contrast : public QDialog
{
    Q_OBJECT

public:
    explicit ui_contrast(QWidget *parent = 0);
    ~ui_contrast();
private slots:

    void r_imageshort(cv::Mat_<unsigned short> a);

    void on_level_valueChanged(int value);

    void on_lineEdit_editingFinished();

    void on_ok_button_clicked();

    void on_cancel_button_clicked();

    void on_minus_clicked();

    void on_plus_clicked();

    void r_contrast_para(int pos);

signals:
    void s_imageshort(cv::Mat_<unsigned short>);
    void s_cancel();
    void s_ok(cv::Mat_<unsigned short>, QString opt);
private:
    Ui::ui_contrast *ui;
    cv::Mat_<unsigned short> srcimg;
    cv::Mat_<unsigned short> outputimg;
    int midpoint = 32768;
};

#endif // UI_CONTRAST_H
