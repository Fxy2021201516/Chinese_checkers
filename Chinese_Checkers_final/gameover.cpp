#include "gameover.h"
#include "ui_gameover.h"

gameover::gameover(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameover)
{
    ui->setupUi(this);
}

gameover::~gameover()
{
    delete ui;
}
void gameover::set1(QString player,QString rank)
{
    ui->textBrowser->setText("player"+player+" is num "+rank);
}
void gameover::set2(QString player,QString rank)
{
    ui->textBrowser_3->setText("player"+player+" is num "+rank);
}
void gameover::set3(QString player,QString rank)
{
    ui->textBrowser_5->setText("player"+player+" is num "+rank);
}
void gameover::set4(QString player,QString rank)
{
    ui->textBrowser_10->setText("player"+player+" is num "+rank);
}
void gameover::set5(QString player,QString rank)
{
    ui->textBrowser_7->setText("player"+player+" is num "+rank);
}
void gameover::set6(QString player,QString rank)
{
    ui->textBrowser_11->setText("player"+player+" is num "+rank);
}
