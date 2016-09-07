#include "database.h"

bool drawchart::onshape(QPoint po)
{
    if (po.x()>=p1.x()&&po.x()<=p2.x()&&po.y()>=p1.y()&&po.y()<=p2.y()) return true;
    return false;

}

bool drawchart::onrec(QPoint po)
{
    if (abs(po.x()-p2.x())<5&&abs(po.y()-p2.y())<5) return true;
    return false;
}
drawchart drawcharlist::findonshape(QPoint po)
{
    for (int i=0; i < chartlist.size(); i++)
    {
        if (drawchart(chartlist.at(i)).onshape(po)) return drawchart(chartlist.at(i));
    }
    drawchart tmp;
    return tmp;

}

bool drawcharlist::ifonshape(QPoint po)
{
    for (int i=0; i < chartlist.size(); i++)
    {
        if (drawchart(chartlist.at(i)).onshape(po)) return true;
    }
    return false;
}

drawchart drawcharlist::findonrec(QPoint po)
{
    for (int i=0; i < chartlist.size(); i++)
    {
        if (drawchart(chartlist.at(i)).onrec(po)) return drawchart(chartlist.at(i));
    }
    drawchart tmp;
    return tmp;

}
bool drawcharlist::ifonrec(QPoint po)
{
    for (int i=0; i < chartlist.size(); i++)
    {
        if (drawchart(chartlist.at(i)).onrec(po)) return true;
    }
    return false;
}

void drawcharlist::update(drawchart oldchart, drawchart newchart)
{
    for (int i=0; i < chartlist.size(); i++)
    {
        if ((drawchart(chartlist.at(i))).p1.x() == oldchart.p1.x() && (drawchart(chartlist.at(i))).p1.y() == oldchart.p1.y()
              && (drawchart(chartlist.at(i))).p2.x() == oldchart.p2.x() && (drawchart(chartlist.at(i))).p2.y() == oldchart.p2.y())
        {
            chartlist.at(i)=newchart;
        }
    }
}

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
    bool success = query.exec("create table if not exists imagelist(name varchar,id varchar,path varchar,date varchar,operation varchar,chart varchar)");
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

    query.prepare("select name, id, path, date, operation, chart from imagelist where path = :path");
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
    item.chart = query.value(5).toString();

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
    query.prepare("insert into imagelist (name, id, path, date, operation, chart) values (:name, :id, :path, :date, :operation, :chart)");

    query.bindValue(":name",QString::fromLocal8Bit(item.name.toLocal8Bit().data()));

    query.bindValue(":id",QString::fromLocal8Bit(item.id.toLocal8Bit().data()));

    query.bindValue(":path",QString::fromLocal8Bit(item.path.toLocal8Bit().data()));

    query.bindValue(":date",QString::fromLocal8Bit(item.date.toLocal8Bit().data()));

    query.bindValue(":operation",QString::fromLocal8Bit(item.operation.toLocal8Bit().data()));

    query.bindValue(":chart",QString::fromLocal8Bit(item.chart.toLocal8Bit().data()));

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
    query.prepare("update imagelist set name = ?, id = ?, date = ?, operation = ?, chart = ? where path = ?");
    query.bindValue(0,QString::fromLocal8Bit(item.name.toLocal8Bit().data()));
    query.bindValue(1,QString::fromLocal8Bit(item.id.toLocal8Bit().data()));
    query.bindValue(2,QString::fromLocal8Bit(item.date.toLocal8Bit().data()));
    query.bindValue(3,QString::fromLocal8Bit(item.operation.toLocal8Bit().data()));
    query.bindValue(4,QString::fromLocal8Bit(item.chart.toLocal8Bit().data()));
    query.bindValue(5,QString::fromLocal8Bit(item.path.toLocal8Bit().data()));

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

QStringList database::query_all()
{
    QStringList reclist;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","industry_sql");
    db.setDatabaseName("data.db");
    if (!db.open())
    {
        cerr<<"open failed!"<<endl;
        return reclist;
    }
    QSqlQuery query(db);
    query.prepare("select path from imagelist");

    bool success = query.exec();
    if (!success)
    {
        cerr<<"select all failed!"<<endl;
        qDebug()<<query.lastError()<<endl;
        return reclist;
    }

    while(query.next())
    {
        reclist.append(QString(query.value(0).toString()));
    }



    db.close();
    return reclist;
}
























