#include "myslider.h"
#include "QMouseEvent"
#include "QPoint"

myslider::myslider(QWidget *parent) :
    QPushButton(parent)
{
    moveflag=false;
    cnt=0;
}

void myslider::setgstart(int num)
{
    gstart=num;
}
int myslider::getgstart()
{
    return gstart;
}
void myslider::setgend(int num)
{
    gend=num;
}
int myslider::getgend()
{
    return gend;
}
void myslider::setlstart(int num)
{
    lstart=num;
}
int myslider::getlstart()
{
    return lstart;
}
void myslider::setlend(int num)
{
    lend=num;
}
int myslider::getlend()
{
    return lend;
}
void myslider::setlmin(int num)
{
    lmin=num;
}
int myslider::getlmin()
{
    return lmin;
}
void myslider::setlmax(int num)
{
    lmax=num;
}
int myslider::getlmax()
{
    return lmax;
}
void myslider::setpos(int num)
{
    pos=num;
}
int myslider::getpos()
{
    return pos;
}

void myslider::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button()==Qt::LeftButton)
    {
        moveflag=true;
        setMouseTracking(true);

    }
}
void myslider::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button()==Qt::LeftButton)
    {
        moveflag=false;
        setMouseTracking(false);
    }
}


void myslider::mouseMoveEvent(QMouseEvent *ev)
{
    cnt++;
    int interval=1;
    if (ev->buttons()==Qt::LeftButton&&moveflag)
    {


        QPoint m_pos=ev->pos();
        int mx=m_pos.x();
//        this->setText(QString::number(mx)+','+QString::number(m_pos.y()));
        if (pos+mx<lstart||pos+mx>lend)
        {
            if (pos!=lstart&&pos+mx<lstart)
            {
                pos=lstart;
                if (cnt%interval==0)
                emit poschanged(pos);
                this->setGeometry(QRect(pos,this->y(),this->width(),this->height()));
            }
            if (pos!=lend&&pos+mx>lend)
            {
                pos=lend;
                if (cnt%interval==0)
                emit poschanged(pos);
                this->setGeometry(QRect(pos,this->y(),this->width(),this->height()));
            }
        }
        else
        {
            pos=pos+mx;
            if (cnt%interval==0)
            emit poschanged(pos);
            this->setGeometry(QRect(pos,this->y(),this->width(),this->height()));
        }
    }
}
