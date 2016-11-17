#ifndef HOMO_CLIP_H
#define HOMO_CLIP_H

#include <QDialog>
#include "opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "QImage"
#include "QDateTime"
#include "algorithm.h"
#include "hist.h"
#include "QImage"
#include "database.h"

using namespace cv;

namespace Ui {
class homo_clip;
}

class homo_clip : public QDialog
{
    Q_OBJECT

public:
    explicit homo_clip(QWidget *parent = 0,QString tfilepath = "", QDateTime tdt = QDateTime::currentDateTime(), imageitem& tmpitem = imageitem(), database& tdb = database());
    ~homo_clip();
    QString filepath;
    QDateTime dt;
    QString tgtpath;
    QString scanid;
    QString filename;
    imageitem dataitem;
    database datadb;
    bool initialize();

private slots:
    void r_para(QString filepath, QDateTime dt, QString tgtpath);

    void on_autohomo_stateChanged(int arg1);

    void on_autoclip_stateChanged(int arg1);

    void on_ok_clicked();

    void on_degree_textEdited(const QString &arg1);

    void on_top_textEdited(const QString &arg1);

    void on_bottom_textEdited(const QString &arg1);

    void on_left_textEdited(const QString &arg1);

    void on_right_textEdited(const QString &arg1);

    void on_autoname_clicked();

    void on_clipname_textEdited(const QString &arg1);

private:
    Ui::homo_clip *ui;
    Mat_<unsigned short> rawimg;
    Mat_<unsigned short> clipimg(Mat_<unsigned short> homoimg);
    void showimg(Mat_<unsigned short> img);
    bool ifinitial;
    bool ifrotate;




};

#endif // HOMO_CLIP_H
