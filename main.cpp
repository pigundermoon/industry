#include "mainwindow.h"
#include <QApplication>
#include "QString"
#include "QTextCodec"
#include "QTextStream"
#include "qfile.h"
#include "QDebug"
#include "QDesktopWidget"





using namespace std;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    w.setWindowTitle(QString::fromLocal8Bit("工业探伤"));
    w.initialize();

    QDesktopWidget *desktop = QApplication::desktop();
    QRect cur = desktop->availableGeometry(desktop->primaryScreen());
    w.setFixedSize(cur.width(), cur.height());
    w.setWindowFlags(w.windowFlags() ^ Qt::WindowMaximizeButtonHint);

    w.show();
    QFile file(":/style-back.qss");
    file.open(QFile::ReadOnly);
    QString ss=file.readAll();
    file.close();
    a.setStyleSheet(ss);

    return a.exec();
}
