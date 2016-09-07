#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include "QStringList"
#include "QPoint"
#include "vector"

using namespace std;

class imageitem{
public:
    QString name;
    QString id;
    QString path;
    QString date;
    QString operation;
    QString chart;
    bool exist;
};

class drawchart{
public:
    int type;
    QPoint p1;
    QPoint p2;
    bool onshape(QPoint po);
    bool onrec(QPoint po);
};

class drawcharlist{
public:
    vector<drawchart> chartlist;
    void clear(){chartlist.clear();}
    void insert(drawchart temp){chartlist.push_back(temp);}
    void update(drawchart oldchart, drawchart newchart);
    bool ifonshape(QPoint po);
    drawchart findonshape(QPoint po);
    bool ifonrec(QPoint po);
    drawchart findonrec(QPoint po);
};

class database
{
public:
    database();
    bool initialize();
    bool insert_imageitem(imageitem item);
    bool update_imageitem(imageitem item);
    imageitem query_imageitem(QString path);
    QStringList query_all();

};

#endif // DATABASE_H
