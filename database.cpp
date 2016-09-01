#include "database.h"


database::database()
{

}
bool database::initialize()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","industry_sql");
    db.setDatabaseName("data.db");
    if (!db.open())
    {
        cerr<<"databse create failed!"<<endl;
        return false;
    }
    QSqlQuery query(db);
    bool success = query.exec("create table if not exists imagelist(name varchar,id varchar,path varchar,date varchar,operation varchar)");
    if (!success)
    {
        cerr<<"create table failed!"<<endl;
        qDebug()<<query.lastError()<<endl;
        return false;
    }

    db.close();
    return true;
}

imageitem database::query_imageitem(QString path)
{
    imageitem item;
    item.exist=true;
    QSqlDatabase db = QSqlDatabase::database("industry_sql");
    if (!db.open())
    {
        cerr<<"open failed!"<<endl;
        item.exist=false;
        return item;
    }
    QSqlQuery query(db);

    query.prepare("select name, id, path, date, operation from imagelist where path = :path");
    query.bindValue(":path",QString::fromLocal8Bit(path.toLocal8Bit().data()));

    bool success = query.exec();
    if (!success || !query.first())
    {
        cerr<<"select failed!"<<endl;
        qDebug()<<query.lastError()<<endl;
        item.exist=false;
        return item;
    }

    item.name = query.value(0).toString();
    item.id = query.value(1).toString();
    item.path = query.value(2).toString();
    item.date = query.value(3).toString();
    item.operation = query.value(4).toString();

    db.close();
    return item;


}

bool database::insert_imageitem(imageitem item)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","industry_sql");
    db.setDatabaseName("data.db");
    if (!db.open())
    {
        cerr<<"open failed!"<<endl;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("insert into imagelist (name, id, path, date, operation) values (:name, :id, :path, :date, :operation)");

    query.bindValue(":name",QString::fromLocal8Bit(item.name.toLocal8Bit().data()));

    query.bindValue(":id",QString::fromLocal8Bit(item.id.toLocal8Bit().data()));

    query.bindValue(":path",QString::fromLocal8Bit(item.path.toLocal8Bit().data()));

    query.bindValue(":date",QString::fromLocal8Bit(item.date.toLocal8Bit().data()));

    query.bindValue(":operation",QString::fromLocal8Bit(item.operation.toLocal8Bit().data()));

    bool success = query.exec();
    if (!success)
    {
        cerr<<"insert failed!"<<endl;
        qDebug()<<query.lastError()<<endl;
    }


    db.close();
    return true;
}

bool database::update_imageitem(imageitem item)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","industry_sql");
    db.setDatabaseName("data.db");
    if (!db.open())
    {
        cerr<<"open failed!"<<endl;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("update imagelist set name = ?, id = ?, date = ?, operation = ? where path = ?");
    query.bindValue(0,item.name);
    query.bindValue(1,item.id);
    query.bindValue(2,item.date);
    query.bindValue(3,item.operation);
    query.bindValue(4,item.path);

    bool success = query.exec();
    if (!success)
    {
        cerr<<"update failed!"<<endl;
        qDebug()<<query.lastError()<<endl;
        return false;
    }


    db.close();
    return true;
}

























