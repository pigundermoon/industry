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
#include "ui_sliderchoose.h"
#include "ui_emboss.h"
#include "showimgwindow.h"
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
#include "QTimer"
#include "QMutex"
#include "QtConcurrent/QtConcurrent"
#include "homo_clip.h"
#include "map"
#include "ui_compare_show.h"
#include "QDesktopWidget"

enum right_workstatus{rsta_translation=0,rsta_mark=1,rsta_drawrect=2,rsta_changerect=3,rsta_dragrect=4,rsta_dragbarrect=5,rsta_dragcontrast=6};


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

    void on_rulergra_triggered();

    void r_degree(float degree);

    void r_ok_degree(float degree);

    void r_emboss_value(int dis, int contrast);

    void r_ok_emboss_value(int dis, int contrast);

    void on_homotran_triggered();

    void on_emboss_triggered();

    void on_scan_triggered();

    void on_grawidlock_triggered();

    void on_timeout_load();

    void r_ctimer();

    void on_loaddata_triggered();

    void on_loaddata_reset_triggered();

    void on_resetgra_triggered();

    void on_cascade_triggered();

    void on_autogra_triggered();

    void on_basicdenoise_triggered();

    void on_cycle_strong_triggered();

    void on_cycle_mid_triggered();

    void on_cycle_weak_triggered();

    void on_RTV_strong_triggered();

    void on_RTV_mid_triggered();

    void on_RTV_weak_triggered();

    void on_rawimg_triggered();

    void r_denoise_close_ok();

    void on_bigshow_triggered();

    void r_bigshow_close();

    void on_compareshow_triggered();

signals:
    void s_imageshort(cv::Mat_<unsigned short>);
    void s_imageshort_hist(cv::Mat_<unsigned short>, unsigned short indark,unsigned short ingray,unsigned short inwhite,unsigned short outdark, unsigned short outwhite);
    void s_hist(cv::Mat_<unsigned char>);
    void s_number(int);
    void s_imageinfo(recdcmtkfile* file);
    void ctimer();
    void s_homoclip_para(QString filepath, QDateTime dt, QString tgtpath);
    void s_contrast_para(int pos);
    void s_contrast_para_self(int pos);
    void s_denoise_close();
    void s_bigshowimg(QImage img);
    void s_2_imgshort(cv::Mat_<unsigned short> im1, cv::Mat_<unsigned short> im2,double scale);

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
    ui_sliderchoose* w6;
    ui_emboss* w7;
    ui_contrast* w_contrast;
    ui_denoise* w_denoise;
    showimgwindow* w_bigshow;
    ui_compare_show* w_compareshow;

    QString label_loc[4];
    int label_loc_ptr[4];

    cv::Mat_<unsigned char> srcimgchar;
    cv::Mat_<unsigned short> raw_srcimgshort;
    cv::Mat_<unsigned short> srcimgshort;
    cv::Mat_<unsigned short> cvtsrcimgchar;
//    std::deque<cv::Mat_<unsigned short>> backup;
    void enableaction();
    void disableaction();
    void show_image(cv::Mat_<unsigned short> s, int type = 1, bool notcgra = true);

    QPoint scrollpos;
    bool scrollclicked;

    QPoint cpos;
    bool cclicked;

    right_workstatus rstatus;

    //同步显示
    bool ifbigshow;

    //用于色阶
    double rate;

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

    //扫描软件及数据库归档
    QMutex scanflag;
    bool scanrun;
    QTimer* scantime;
    void execscan();
    bool ifresave;
    homo_clip* homow;

    //生成实时路径
    QString genpath();


    //current item changed
    bool listchangedflag;

    //绘图相关
    int draw_dhw;
    int draw_height;
    int draw_dhwnum;
    drawcharlist cur_chartlist;
    drawchart oldchart;
    void update_chartlist(QString chart);
    QPoint stdrawpos;
    QPoint prepos;
    int pardraw;
    void drawpaint(QPainter* painter);
    bool drawst;

    bool graruler;//标尺


    //对比度
    void r_contrast_para_self(int pos);

    //色阶相关
    bool lock_grawidth;
    unsigned short ingray;

    //去噪
    void denoise_methods(int type=0, int degree = 0);

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
