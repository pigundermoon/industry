#include "ui_contrast.h"
#include "ui_ui_contrast.h"
using namespace cv;
//打表，不同的强度分别经过+100、-50的对比度调整后的值
int c_up_bound[256] = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 13, 13, 14,
14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 54, 55, 56, 57, 59, 60, 61, 62, 63, 65, 66, 67, 69, 70, 71, 73, 74, 75, 77, 78,
79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 94, 95, 97, 99, 100, 102, 103, 105, 106, 108, 110, 111, 113, 115, 116, 118, 120, 121, 123, 125, 127, 128,
130, 132, 134, 135, 137, 139, 140, 142, 144, 145, 147, 149, 150, 152, 153, 155, 156, 158, 160, 161, 163, 164, 166, 167, 169, 170, 171, 173, 174, 176, 177, 178,
180, 181, 182, 184, 185, 186, 188, 189, 190, 192, 193, 194, 195, 196, 198, 199, 200, 201, 202, 203, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216,
217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 226, 227, 228, 229, 230, 231, 231, 232, 233, 234, 234, 235, 236, 237, 237, 238, 239, 239, 240, 241, 241, 242,
242, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 252, 253, 253, 253, 254, 254, 254, 254, 255, 255 };
int c_low_bound[256] = { 0, 1, 3, 4, 5, 7, 8, 10, 11, 12, 14, 15, 16, 17, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 31, 33, 34, 35, 36, 38, 39, 40, 41,
42, 43, 45, 46, 47, 48, 49, 50, 52, 53, 54, 55, 56, 57, 58, 59, 60, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 98, 99, 100, 101, 102, 103, 103, 104, 105,
106, 107, 107, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 116, 117, 118, 119, 119, 120, 121, 121, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128,
128, 129, 130, 130, 131, 132, 132, 133, 134, 134, 135, 136, 136, 137, 138, 139, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 147, 148, 148, 149, 150, 151,
152, 152, 153, 154, 155, 156, 157, 157, 158, 159, 160, 161, 162, 163, 164, 165, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 195, 196, 197, 198, 199, 200, 201, 202, 203, 205, 206, 207, 208, 209, 210, 212, 213, 214, 215,
216, 217, 219, 220, 221, 222, 224, 225, 226, 227, 229, 230, 231, 232, 234, 235, 236, 238, 239, 240, 241, 243, 244, 245, 247, 248, 250, 251, 252, 254, 255 };
int upbound[65536];
int lowbound[65536];

//打表法模拟PS对比度 按照PS里面的调整范围-50<=contrast<=100
void contrast(Mat_<unsigned short> & input, Mat_<unsigned short> & output, int contrast)
{
    input.copyTo(output);
    if (contrast == 0) return;
    contrast = MAX(-100, MIN(100, contrast));

    double ratio = abs(contrast) / 100.0;
    //调整程度关于contrast不是线性的，用二次曲线来拟合，得到变化比率ratio
    ratio = 0.4 * ratio + 0.6 * (1 - (1 - ratio) * (1 - ratio));

    //线性调整对比度
    unsigned short vmap[65536];
    for (int i = 0; i < 65536; i++){
        if (contrast > 0) {
            vmap[i] = round(i + (upbound[i] - i) * ratio);
        }
        else{
            vmap[i] = round(i + (lowbound[i] - i) * ratio);
        }
    }

    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
        {
            output(i, j) =vmap[input(i,j)] ;
        }
}



ui_contrast::ui_contrast(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ui_contrast)
{
    ui->setupUi(this);
}

ui_contrast::~ui_contrast()
{
    delete ui;
}
//初始
void ui_contrast::r_imageshort(cv::Mat_<unsigned short> a)
{
    for (int i=0;i<65536;i++)
    {
        upbound[i]=int(double(c_up_bound[int(double(i)/65535*255)])/255*65535);
        lowbound[i]=int(double(c_low_bound[int(double(i)/65535*255)])/255*65535);
    }
    srcimg=a;
    contrast(srcimg,outputimg,ui->level->value());
    emit s_imageshort(outputimg);
}

void ui_contrast::on_level_valueChanged(int value)
{
    ui->lineEdit->setText(QString::number(value));
    contrast(srcimg,outputimg,value);
    emit s_imageshort(outputimg);
}
void ui_contrast::on_minus_clicked()
{
    int position=ui->level->value()-1;
    position=position<ui->level->maximum()?position:ui->level->maximum();
    position=position>ui->level->minimum()?position:ui->level->minimum();
    ui->level->setValue(position);

}
void ui_contrast::on_plus_clicked()
{
    int position=ui->level->value()+1;
    position=position<ui->level->maximum()?position:ui->level->maximum();
    position=position>ui->level->minimum()?position:ui->level->minimum();
    ui->level->setValue(position);
}



void ui_contrast::on_lineEdit_editingFinished()
{
    int position=ui->lineEdit->text().toInt();
    position=position<ui->level->maximum()?position:ui->level->maximum();
    position=position>ui->level->minimum()?position:ui->level->minimum();
    ui->level->setValue(position);

}

void ui_contrast::on_ok_button_clicked()
{
    emit s_ok(outputimg);
    this->close();
}


void ui_contrast::on_cancel_button_clicked()
{
    emit s_cancel();
    this->close();
}


