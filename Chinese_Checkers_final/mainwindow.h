#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>//包含窗口类

QT_BEGIN_NAMESPACE//定义命名空间
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//开始界面的窗口的类
class MainWindow : public QMainWindow
{
    Q_OBJECT//宏，允许类中使用信号和槽的机制，是默认存在的

public:
    MainWindow(QWidget *parent = nullptr);//构造和析构函数
    ~MainWindow();

private slots://开始界面上按钮发出的信号

    void on_pushButton_3_clicked();//游戏规则

    void on_pushButton_4_clicked();//退出游戏

    void on_beginpushButton_clicked();//六人游戏

    void on_doubleButton_clicked();//双人游戏

    void on_threeButton_clicked();//三人游戏

    void on_pushButton_clicked();//联机游戏



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
