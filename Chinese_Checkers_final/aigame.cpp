#include "aigame.h"
#include "ui_aigame.h"
#include <QPainter>
#include <QPaintEvent>
#include "choosestyle.h"
#include "mainwindow.h"
#include"win.h"
//#include "gameover.h"

extern int player;
aigame::aigame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aigame)
{
    ui->setupUi(this);
    this->setWindowTitle("人机对战");
}

aigame::~aigame()
{
    delete ui;
}

//填棋盘图片
void aigame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //获取widget大小
    int x = ui->widget->geometry().x();
    int y = ui->widget->geometry().y();
    int width = ui->widget->geometry().width();
    int height =ui->widget->geometry().height();
    QRectF rect(x,y,width,height);
    //填充棋盘图片
    QImage image;
    image.load(":/res/chessboard.jpg");
    painter.drawImage(rect,image);

}

//返回
void aigame::on_pushButton_clicked()
{
    this->close();
    MainWindow* w = new MainWindow();
    w->show();
}

//结束游戏
void aigame::on_pushButton_2_clicked()
{
    this->close();
    win*w=new win();
    w->show();
}



