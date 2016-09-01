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

using namespace std;

class imageitem{
public:
    QString name;
    QString id;
    QString path;
    QString date;
    QString operation;
    bool exist;
};

class database
{
public:
    database();
    bool initialize();
    bool insert_imageitem(imageitem item);
    bool update_imageitem(imageitem item);
    imageitem query_imageitem(QString path);

};

#endif // DATABASE_H
