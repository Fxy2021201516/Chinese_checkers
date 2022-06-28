#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameplay.h"
#include "game2player.h"
#include "game3play.h"
#include "regulation.h"
#include "serverorclient.h"
//.h文件用来声明函数/类、结构体
//mainwindow.cpp是主窗体的头文件和源文件，窗体的一些操作在这里写

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("中国跳棋");//窗口标题
}

MainWindow::~MainWindow()
{
    delete ui;
}
//二人游戏
void MainWindow::on_doubleButton_clicked()
{
    this->hide();//隐藏开始的界面（为了切到另一个界面）
    game2player* game = new game2player();//创建一个游戏界面（初始化）
    game->show();//展示这个新窗口
}

//三人游戏
void MainWindow::on_threeButton_clicked()
{
    this->hide();//隐藏开始的界面（为了切到另一个界面）
    game3play* game = new game3play();//创建一个游戏界面（初始化）
    game->show();//展示这个新窗口
}
//六人游戏
void MainWindow::on_beginpushButton_clicked()
{
    this->hide();//隐藏开始的界面（为了切到另一个界面）
    gameplay* game = new gameplay();//创建一个游戏界面（初始化）
    game->show();//展示这个新窗口
}




//规则窗口
void MainWindow::on_pushButton_3_clicked()
{
    regulation* re =new regulation(this);//初始化规则窗口的界面
    re->show();
}

//关闭窗口
void MainWindow::on_pushButton_4_clicked()
{
    this->close();
}

//联机游戏
void MainWindow::on_pushButton_clicked()
{
    this->hide();
    serverOrClient* soc = new serverOrClient();//初始化选择属性的窗口
    soc->show();
}

