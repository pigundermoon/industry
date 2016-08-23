#ifndef MYSLIDER_H
#define MYSLIDER_H


#include <QWidget>
#include <QPushButton>

class myslider : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(int gstart READ getgstart WRITE setgstart)
    Q_PROPERTY(int gend READ getgend WRITE setgend)
    Q_PROPERTY(int lstart READ getlstart WRITE setlstart)
    Q_PROPERTY(int lend READ getlend WRITE setlend)
    Q_PROPERTY(int lmax READ getlmax WRITE setlmax)
    Q_PROPERTY(int lmin READ getlmin WRITE setlmin)
    Q_PROPERTY(int pos READ getpos WRITE setpos NOTIFY poschanged)


public:
    myslider(QWidget *parent = 0);

    void setgstart(int num);
    int getgstart();

    void setlstart(int num);
    int getlstart();

    void setgend(int num);
    int getgend();

    void setlend(int num);
    int getlend();

    void setlmax(int num);
    int getlmax();

    void setlmin(int num);
    int getlmin();

    void setpos(int num);
    int getpos();

Q_SIGNALS:
    void poschanged(int num);
private:
    bool moveflag;
    int gstart;
    int gend;
    int lstart;
    int lend;
    int lmin;
    int lmax;
    int pos;
    int cnt;

protected:
//    void mousePressEvent(QMouseEvent *ev);
//    void mouseReleaseEvent();
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

};


#endif
