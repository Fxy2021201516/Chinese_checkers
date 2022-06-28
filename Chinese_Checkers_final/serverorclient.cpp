#include "serverorclient.h"
#include "ui_serverorclient.h"
#include "gameplay.h"
#include "gameserver.h"
#include <Qstring.h>
#include "networksocket.h"
#include "networkserver.h"
#include <QAbstractSocket>
#include "joinroomclient.h"
#include "issuccessful.h"
#include "iserror.h"
#include<QDebug>

serverOrClient::serverOrClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::serverOrClient)
{
    ui->setupUi(this);
}

serverOrClient::~serverOrClient()
{
    delete ui;
}

//客户端
void serverOrClient::on_pushButton_2_clicked()
{
    //如果用户选择客户端应该调用hello信息向服务端发送连接请求
    //查清校园网的内网地址
    //实现显示联网结果的函数
    gameplay* game = new gameplay();

    connect(game->socket->base(),&QAbstractSocket::connected,this,&serverOrClient::connected);
    connect(game->socket->base(),&QAbstractSocket::errorOccurred,this,&serverOrClient::errorOccurred);
    const QString& host= ui->lineEdit->text();
    QString tmpStr=ui->lineEdit_2->text();
    quint16 port=tmpStr.toInt();
    game->socket->hello(host,port);
    this->hide();
    joinRoomClient* jrc=new joinRoomClient(game);
    connect(this,SIGNAL(sendData(gameplay*)),jrc,SLOT(getData(gameplay*)));
    connect(jrc,SIGNAL(sendData(gameplay*)),this,SLOT(getData(gameplay*)));
    sendData(game);
    jrc->show();
}

//服务端
void serverOrClient::on_pushButton_clicked()
{
    gameServer* gs =new gameServer();
    gs->show();
}
void serverOrClient::errorOccurred()
{
    isError *err=new isError();
     qDebug()<<"There is error!!";
    err->show();
}
void serverOrClient::connected()
{
    isSuccessful *suc=new isSuccessful();
    qDebug()<<"There is successful!";
    suc->show();
}
void serverOrClient::getData(gameplay*)
{}
