#include "mainwindow.h"

#include <QApplication>//包含一个应用程序类的头文件

int main(int argc, char *argv[])
{
    //a是应用程序对象，有且只有一个
    QApplication a(argc, argv);
    //窗口对象
    MainWindow w;
    //窗口对象不show不显示
    w.show();
    //让应用程序对象进入消息循环，代码阻塞到当前行
    return a.exec();
}
