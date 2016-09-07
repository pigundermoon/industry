#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "c_gradation.h"
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "deque"
#include "ui_hist_hdr.h"
#include "ui_local_hdr.h"
#include "ui_imageinfo.h"
#include "ui_contrast.h"
#include "ui_denoise.h"
#include "ui_about.h"
#include "QListWidget"
#include "processing.h"
#include <QtConcurrent/QtConcurrent>
#include "qscrollbar.h"
#include "QWheelEvent"
#include "QPainter"
#include "recdcmtkfile.h"
#include "QLineEdit"
#include "QComboBox"
#include "QMessageBox"
#include "QFileSystemModel"
#include "database.h"
#include "QToolButton"
#include "buttonactionadapter.h"
#include "QStandardItemModel"
#include "vector"

enum right_workstatus{rsta_translation=0,rsta_mark=1,rsta_drawrect=2,rsta_changerect=3,rsta_dragrect=4};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void settext(QString arg);
    void setCurScale(int scale);
    explicit MainWindow(QWidget *parent = 0);
    void initialize();
    ~MainWindow();

private slots:

    void r_imageshort(cv::Mat_<unsigned short> img);
    void r_imageshort_cgra(cv::Mat_<unsigned short> img);

    void r_imagechar(cv::Mat_<unsigned char> img);

//    void on_showimgscale_sliderMoved(int position);

    void r_cancel();

    void r_ok(cv::Mat_<unsigned short> a, QString opt);

    void r_ok_hist(unsigned short tindark, unsigned short tingray, unsigned short tinwhite, unsigned short toutdark, unsigned short toutwhite);

    void on_openfile_triggered();

    void on_hist_triggered();

    void on_invert_triggered();

    void on_back_triggered();

    void on_hist_hdr_triggered();

    void on_localadaptive_hdr_triggered();

    void on_resave_triggered();

    void on_info_triggered();

    void on_turn_horizontal_triggered();

    void on_turn_vertical_triggered();

    void on_contrast_triggered();

    void on_denoise_triggered();

    void on_about_triggered();

    void on_zoom_in_triggered();

    void on_zoom_out_triggered();

    void on_exit_triggered();

    void on_imagelist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_rate_editingFinished();



    void on_zoom_triggered();


    void on_mark_triggered();

    void on_removegrade_triggered();

    void on_fileexplorer_doubleClicked(const QModelIndex &index);

    void on_drawrect_triggered();

    void on_resetdraw_triggered();

signals:
    void s_imageshort(cv::Mat_<unsigned short>);
    void s_imageshort_hist(cv::Mat_<unsigned short>, unsigned short indark,unsigned short ingray,unsigned short inwhite,unsigned short outdark, unsigned short outwhite);
    void s_hist(cv::Mat_<unsigned char>);
    void s_number(int);
    void s_imageinfo(recdcmtkfile* file);

private:
    Ui::MainWindow *ui;
    QImage showimg;
    QImage tshowimg;
    c_gradation* w1;
    ui_hist_hdr* w2;
    ui_local_hdr* w3;
    ui_imageinfo* w4;
    processing* w_process;
    ui_about* w5;
    ui_contrast* w_contrast;
    ui_denoise* w_denoise;

    QString label_loc[4];
    int label_loc_ptr[4];

    cv::Mat_<unsigned char> srcimgchar;
    cv::Mat_<unsigned short> raw_srcimgshort;
    cv::Mat_<unsigned short> srcimgshort;
    cv::Mat_<unsigned short> cvtsrcimgchar;
    std::deque<cv::Mat_<unsigned short>> backup;
    void enableaction();
    void disableaction();
    void show_image(cv::Mat_<unsigned short> s, int type = 1, bool notcgra = true);

    QPoint scrollpos;
    bool scrollclicked;

    QPoint cpos;
    bool cclicked;

    right_workstatus rstatus;

    //用于去阶梯化
    int rgxmin,rgxmax;
    QPoint rgstpos;
    QPoint rgedpos;
    int* rgvector;
    int vectornum;

    //数据库相关
    database industry_db;
    imageitem cur_item;
    QStandardItemModel* dataset_model;//数据库浏览
    void refresh_dataset();

    //current item changed
    bool listchangedflag;

    //绘图相关
    drawcharlist cur_chartlist;
    drawchart oldchart;
    void update_chartlist(QString chart);
    QPoint stdrawpos;
    QPoint prepos;
    int pardraw;
    void drawpaint(QPainter* painter);
    bool drawst;






    int ingray;

    int ow;
    int oh;


    void openfile(QString filename, int type);

    void reset();

    float h_center;
    float w_center;

    int curScale;
    int maxScale;
    int minScale;
    QLineEdit *pRate;

protected:
    bool eventFilter(QObject *target, QEvent *e);


};

#endif // MAINWINDOW_H
