#include "joinroomclient.h"
#include "ui_joinroomclient.h"
#include "networkdata.h"
#include <QString>
#include "gameplay.h"
#include "players.h"
#include "networksocket.h"
#include "waitingroom.h"
#include <QDebug>

joinRoomClient::joinRoomClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::joinRoomClient)
{
    //qDebug()<<"begin dubug";
    ui->setupUi(this);
//    joinRoomClient::myjoinRoomClient=nullptr;
//    myjoinRoomClient=this;


}

joinRoomClient::~joinRoomClient()
{
    delete ui;
}
QString joinRoomClient::getId()
{
    QString tmpId;
    tmpId = ui->joinRoomClient::lineEdit->text();
    return tmpId;
}
QString joinRoomClient::getRoomNum()
{
    QString tmpRoomNum;
    tmpRoomNum = ui->joinRoomClient::lineEdit_2->text();
    return tmpRoomNum;
}
//QString joinRoomClient::getPort()
//{
//    QString tmpPort;
//    tmpPort = ui->joinRoomClient::lineEdit_3->text();
//    return tmpPort;
//}

void joinRoomClient::joinRoom()
//向外发送opration的，在这个函数里写并且顺次执行
//需要接受operation的，单独写成函数由receive判断执行
{
    // 写一个界面用来获取用户输入的名字和房间号信息，这个界面应该在用户点击客户端模式以后随即出现（通过信号和槽实现）
    //发送JOIN_ROOM_OP请求
    //1.将房间号和用户名分别包装成data1（字符串）和data2（字符串）

    QString Id=getId();
    QString RoomNum=getRoomNum();/*
    QString Port=getPort();*/
    qDebug()<<"joinroom begin";
    gameInJoin->roomIdInGameplay=RoomNum;
    gameInJoin->me.name=Id;
    qDebug()<<"joinroom end";
    OPCODE tmpOp=OPCODE::JOIN_ROOM_OP;
    NetworkData newData(tmpOp,RoomNum,Id);
    //2.利用NetworkSocket::send发送
    gameInJoin->socket->send(newData);
    //3.有可能会得到加入失败的信息，比如房间已满，游戏已经开始，重名等等
    //服务器反馈：也是receive的一部分

    //接受JOIN_ROOM_REPLY_OP请求

    //接受JOIN_ROOM_OP请求

    //发送PLAYER_READY_OP请求
    //将自己的用户名打包成data1（字符串）
    // 利用NetworkSocket::send发送
    //见下面函数
    //接受PLAYER_READY_OP请求

  //  gameInJoin->gameplay::gameBegin();

}
void joinRoomClient::on_pushButton_clicked()
{
    QString data1=gameInJoin->me.name;
    QString data2=0;
    OPCODE tmpOp=OPCODE::PLAYER_READY_OP;
    NetworkData newData(tmpOp,data1,data2);
    gameInJoin->socket->send(newData);
    gameInJoin->me.isReady=1;
    this->hide();
    waitingRoom* wr=new waitingRoom();
    connect(this,SIGNAL(sendData1(gameplay*)),wr,SLOT(getData1(gameplay*)));
    connect(wr,SIGNAL(sendData1(gameplay*)),this,SLOT(getData1(gameplay*)));
    sendData(gameInJoin);
    wr->show();
}
void joinRoomClient::on_pushButton_2_clicked()
{
    qDebug()<<"finish in";
    joinRoom();
}
void joinRoomClient::getData(gameplay*_game)
{
    gameInJoin=_game;

}
void joinRoomClient::getData1(gameplay*)
{}
