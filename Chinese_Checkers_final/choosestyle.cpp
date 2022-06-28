#include "choosestyle.h"
#include "ui_choosestyle.h"
#include "mainwindow.h"
#include "aigame.h"

int player;
//设置标题，背景
choosestyle::choosestyle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::choosestyle)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/pic2.png"));
    this->setWindowTitle("模式选择");
}
 //析构
choosestyle::~choosestyle()
{
    delete ui;
}

//选择先后手
void choosestyle::getGameState()
{
    QString str;
    str=ui->comboBox->currentText();
    if(str=="先手")
    {
        player=0;
    }
    else
    {
        player=1;
    }

}

//进入人机模式
void choosestyle::on_pushButton_2_clicked()
{
    getGameState();
    aigame* g = new  aigame();
    g->show();
    this->hide();
}

//返回按钮
void choosestyle::on_pushButton_clicked()
{
    this->hide();
    MainWindow* w = new MainWindow();
    w->show();
}



