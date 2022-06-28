#include "gameserver.h"
#include "ui_gameserver.h"
#include "networkdata.h"
#include "gameover.h"
#include "networkserver.h"
#include <QTimer>
#include"QDebug"
#include "board.h"
#include <QDateTime>

gameServer::gameServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gameServer)
{
    this->setWindowTitle("Server");//窗口标题
    ui->setupUi(this);
    this->server = new NetworkServer(this);
    roomName="?";


    connect(this->server, &NetworkServer::receive, this, &gameServer::receiveData);
    for(int i=0;i<6;i++)
    {thePlayers[i].name="?";}
    control=0;
    sameRank=0;
    for(int i=0;i<6;i++)
    {
        thePlayers[i].num=i;
        thePlayers[i].isReady=0;
    }
    i_rank=0;
    i_bye=0;
    cb = new Board();
    cb->initial();
    this->counttime = 300;
    this->myTimer=new QTimer();
    myTimer->start(1000);
    connect(myTimer,SIGNAL(timeout()),this,SLOT(TimeSlot()));
    isRunning=false;
    waitTime=300;
    numOfPlayersBegin=-1;
}
int gameServer::changePlayerId(QChar ch)//接收时向玩家转换
{
    char tmpCh;
    tmpCh = ch.toLatin1();//Qchar转为char
    qDebug()<<"tmpCh ="<<tmpCh ;
    qDebug()<<"beginNum="<<numOfPlayersBegin;
    switch(numOfPlayersBegin)
    {
    case 2:
    {
        switch (tmpCh)
        {
        case 'A':
            return 0;
        case 'D':
            return 1;
        default:
            qDebug()<<"err in beginNum";

        }
        break;
    }
    case 3:
    {
        switch (tmpCh)
        {
        case 'A':
            return 0;
        case 'C':
            return 1;
        case 'E':
            return 2;
        default:
            qDebug()<<"err in beginNum";

        }
        break;
    }
    case 6:
    {
        return tmpCh-'A';
        break;
    }
    default:
    {
        qDebug()<<"err in beginnum";
        return -1;
    }
    }
    return -1;
}
char gameServer:: changePlayerId2(int c)//把01转成ABC
{
    switch(numOfPlayersBegin)
    {
    case 2:
    {
        switch(c)
        {
        case 0:
            return 'A';
        case 1:
            return 'D';
        default:
            qDebug()<<"err in beginNum";
        }
    }
    case 3:
    {
        switch(c)
        {
        case 0:
            return 'A';
        case 1:
            return 'C';
        case 2:
            return 'E';
        default:
            qDebug()<<"err in beginNum";
        }
    }
    case 6:
    {
        switch(c)
        {
        case 0:
            return 'A';
        case 1:
            return 'B';
        case 2:
            return 'C';
        case 3:
            return 'D';
        case 4:
            return 'E';
        case 5:
            return 'F';
        default:
            qDebug()<<"err in beginNum";
        }
    }
    default:
    {
        qDebug()<<"err in beginnum";
        return -1;
    }
        return -1;
    }
}
void  gameServer::producePiece()//right
{
    //生成玩家方的棋子
    int numOfPlayers=getTotalNum();
    switch (numOfPlayers)
    {
    case 2:
    {
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin();iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(0);                //玩家为0
            p->setIcon(QIcon(":/res/chess.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景
            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(1)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(1);                //玩家为1
            p->setIcon(QIcon(":/res/chess2.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        setPiece0(0);
        setPiece3(1);

        break;
    }
    case 3:
    {
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin();iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(0);                //玩家为0
            p->setIcon(QIcon(":/res/chess.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(1)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(1);                //玩家为1
            p->setIcon(QIcon(":/res/chess2.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(2)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(2);                //玩家为2
            p->setIcon(QIcon(":/res/chess3.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        setPiece0(0);
        setPiece2(1);
        setPiece4(2);

        break;
    }
    case 4:
    {
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin();iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(0);                //玩家为0
            p->setIcon(QIcon(":/res/chess.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(1)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(1);                //玩家为1
            p->setIcon(QIcon(":/res/chess2.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(2)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(2);                //玩家为2
            p->setIcon(QIcon(":/res/chess3.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(3)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(3);                //玩家为3
            p->setIcon(QIcon(":/res/chess4.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        setPiece0(0);
        setPiece1(1);
        setPiece4(2);
        setPiece5(3);

        break;
    }
    case 5:
    {
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin();iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(0);                //玩家为0
            p->setIcon(QIcon(":/res/chess.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(1)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(1);                //玩家为1
            p->setIcon(QIcon(":/res/chess2.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(2)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(2);                //玩家为2
            p->setIcon(QIcon(":/res/chess3.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(3)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(3);                //玩家为3
            p->setIcon(QIcon(":/res/chess4.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(4)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(4);                //玩家为4
            p->setIcon(QIcon(":/res/chess5.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        setPiece0(0);
        setPiece1(1);
        setPiece3(2);
        setPiece4(3);
        setPiece2(4);

        break;
    }
    case 6:
    {
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin();iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(0);                //玩家为0
            p->setIcon(QIcon(":/res/chess.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(1)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(1);                //玩家为1
            p->setIcon(QIcon(":/res/chess2.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(2)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(2);                //玩家为2
            p->setIcon(QIcon(":/res/chess3.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(3)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(3);                //玩家为3
            p->setIcon(QIcon(":/res/chess4.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(4)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(4);                //玩家为4
            p->setIcon(QIcon(":/res/chess5.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        for(int i = 0;i<10;i++){
            Piece2* p = new Piece2();
            vectorPieces.push_back(p);
        }
        for(vector<Piece2*>::iterator iter = vectorPieces.begin()+(getNumBefore(5)*10);iter != vectorPieces.end();iter++)
        {
            Piece2* p = *iter;
            p->setStatus(5);                //玩家为5
            p->setIcon(QIcon(":/res/chess6.png"));
            p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
            p->setFlat(true);               //去掉原来pushbutton的背景

            p->setParent(this);             //加入界面
        }
        setPiece0(0);
        setPiece1(1);
        setPiece3(2);
        setPiece4(3);
        setPiece2(4);
        setPiece5(5);

        break;
    }
    }

}
void gameServer::setPiece0(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=4;//小三角形中轴线
    for(int i=16;i>12;i--)
    {
        for(int count=0;count<=(16-i);count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin+count].x,this->cb->chessBoard[i][begin+count].y,23,23);
            ptr->setPlace(i,begin+count);
            this->cb->chessBoard[i][begin+count].flag=1;
            this->cb->chessBoard[i][begin+count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void gameServer::setPiece1(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=12;//小三角形中轴线
    for(int i=9;i<13;i++)
    {
        //        if(i%2)
        //            begin--;
        for(int count=0;count<=i-9;count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin-count].x,this->cb->chessBoard[i][begin-count].y,23,23);
            ptr->setPlace(i,begin-count);
            this->cb->chessBoard[i][begin-count].flag=1;
            this->cb->chessBoard[i][begin-count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
    ptr=NULL;
}
void gameServer::setPiece2(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=13;//小三角形中轴线
    for(int i=7;i>3;i--)
    {
        //        if(i%2)
        //            begin--;
        for(int count=0;count<=7-i;count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin+count].x,this->cb->chessBoard[i][begin+count].y,23,23);
            ptr->setPlace(i,begin+count);
            this->cb->chessBoard[i][begin+count].flag=1;
            this->cb->chessBoard[i][begin+count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void gameServer::setPiece3(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=12;//小三角形中轴线
    for(int i=0;i<4;i++)
    {
        //        if(i%2)
        //            begin--;
        for(int count=0;count<=i;count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin-count].x,this->cb->chessBoard[i][begin-count].y,23,23);
            ptr->setPlace(i,begin-count);
            this->cb->chessBoard[i][begin-count].flag=1;
            this->cb->chessBoard[i][begin-count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void gameServer::setPiece4(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=4;//小三角形中轴线
    for(int i=7;i>3;i--)
    {
        //        if(i%2)
        //            begin--;
        for(int count=0;count<=7-i;count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin+count].x,this->cb->chessBoard[i][begin+count].y,23,23);
            ptr->setPlace(i,begin+count);
            this->cb->chessBoard[i][begin+count].flag=1;
            this->cb->chessBoard[i][begin+count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void gameServer::setPiece5(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=3;//小三角形中轴线
    for(int i=9;i<13;i++)
    {
        //        if(i%2)
        //            begin--;
        for(int count=0;count<=i-9;count++)
        {
            ptr=*iter;
            ptr->setGeometry(this->cb->chessBoard[i][begin-count].x,this->cb->chessBoard[i][begin-count].y,23,23);
            ptr->setPlace(i,begin-count);
            this->cb->chessBoard[i][begin-count].flag=1;
            this->cb->chessBoard[i][begin-count].piecePlace=ptr;
            iter++;
        }
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
int gameServer::getNumBefore(int i)
{
    int num=0;
    for(int j=0;j<i;j++)
    {
        if(thePlayers[j].isReady)
            num+=1;
    }
    return num;
}
int gameServer::getNumAfter(int i)
{
    int num=0;
    for(int j=i+1;j<6;j++)
    {
        if(thePlayers[j].isReady)
            num+=1;
    }
    return num;
}
int gameServer::getTotalNum()
{
    int num=0;
    for(int j=0;j<6;j++)
    {
        if(thePlayers[j].isReady)
            num+=1;
    }
    return num;
}
gameServer::~gameServer()
{
    delete ui;
}

void gameServer::receiveData(QTcpSocket* client,NetworkData networkData)
{
    QList<QTcpSocket*>::iterator it1 = recClients.begin();
    bool sameClient=false;
    for(; it1!= recClients.end(); ++it1)
    {
        if(*it1==client)
        {
            sameClient=true;
        }
    }
    if(!sameClient)
    {
        recClients.append(client);

    }
    switch(networkData.op)
    {
    case OPCODE::JOIN_ROOM_OP:
    {

        if(isRunning)
        {
            OPCODE op=OPCODE::ERROR_OP;
            //String str = String.valueOf(int);
            QString data1= QString::number(400001, 10);
            QString data2="The game has begun";
            NetworkData errData(op,data1,data2);
            server->send(client,errData);
            return;
        }
        QString tmpName;
        tmpName=networkData.data1;
        bool isLegal=true;
        if(tmpName.length()==0)
        {
            OPCODE op=OPCODE::ERROR_OP;
            //String str = String.valueOf(int);
            QString data1= QString::number(400003, 10);
            QString data2="Room name can't be empty";
            NetworkData errData(op,data1,data2);
            server->send(client,errData);
            return;
        }//判断房间名是否为空

        for(int i=0;i<20&&i<tmpName.length();i++)
        {
            char tmp;
            tmp=(char)(networkData.data1.at(i).unicode());
            if(!(('A'<=tmp&&tmp<='Z')||('a'<=tmp&&tmp<='z')||(tmp=='_')||('0'<=tmp&&tmp<='9')))
            {
                isLegal=false;
            }
        }
        if(!isLegal)
        {
            OPCODE op=OPCODE::ERROR_OP;
            //String str = String.valueOf(int);
            QString data1= QString::number(400003, 10);
            QString data2="Wrong room name format";
            NetworkData errData(op,data1,data2);
            server->send(client,errData);
            return;
        }//判断房间名格式并报错

        if(networkData.data2.length()==0)
        {
            OPCODE op=OPCODE::ERROR_OP;
            //String str = String.valueOf(int);
            QString data1= QString::number(400003, 10);
            QString data2="Player id can't be empty";
            NetworkData errData(op,data1,data2);
            server->send(client,errData);
            return;
        }//判断用户名是否为空
        isLegal=true;
        for(int i=0;thePlayers[i].name!="?";i++)
        {
            if(thePlayers[i].name==networkData.data2)
            {
                isLegal=false;
            }
        }
        if(!isLegal)
        {
            OPCODE op=OPCODE::ERROR_OP;
            //String str = String.valueOf(int);
            QString data1= QString::number(400003, 10);
            QString data2="This name has been used";
            NetworkData errData(op,data1,data2);
            server->send(client,errData);
            return;
        }//判断用户名是否被用过
        if(roomName!=tmpName)
        {
            roomName=tmpName;
        }
        OPCODE op=OPCODE::JOIN_ROOM_REPLY_OP;
        QString data1="";
        for(int i=0;thePlayers[i].name!="?"&&i<6;i++)
        {
            data1=data1+thePlayers[i].name+" ";
        }
        data1=data1.simplified();
        QString data2="";
        for(int i=0;thePlayers[i].name!="?"&&i<6;i++)
        {
            data2=data2+QString("%1 ").arg(thePlayers[i].isReady);
        }
        data2=data2.simplified();
        NetworkData newData(op,data1,data2);
        server->send(client,newData);//发送joinroomreply给原来玩家
        data1=networkData.data2;
        op=OPCODE::JOIN_ROOM_OP;
        NetworkData newData2(op,data1,data2);
        int i;
        for( i=0;thePlayers[i].name!="?";i++){}
        thePlayers[i].name=networkData.data2;
        QList<QTcpSocket*>::iterator it1 = recClients.begin();
        for(; it1!= recClients.end(); ++it1)
        {
            if(!(*it1==client))
            {
                server->send(*it1,newData2);//发送joinroom给其他玩家
            }
        }


        break;
    }
    case OPCODE::PLAYER_READY_OP:
    {
        for(int i=0;thePlayers[i].name!="?"&&i<6;i++)
        {
            if(thePlayers[i].name==networkData.data1)
            {
                thePlayers[i].isReady=1;
                break;
            }
        }
        QList<QTcpSocket*>::iterator it1 = recClients.begin();
        for(; it1!= recClients.end(); ++it1)
        {
            if(!(*it1==client))
            {
                server->send(*it1,networkData);//发送joinroom给其他玩家
            }
        }
        //发送networkdata给所有除了发送者以外的人
        if (thePlayers[0].isReady&&thePlayers[1].isReady&&(thePlayers[2].isReady||thePlayers[2].name==QString::fromStdString("?"))&&(thePlayers[3].isReady||thePlayers[3].name==QString::fromStdString("?"))&&(thePlayers[4].isReady||thePlayers[4].name==QString::fromStdString("?"))&&(thePlayers[5].isReady||thePlayers[5].name==QString::fromStdString("?")))
        {
            producePiece();
            isRunning=true;
            OPCODE op=OPCODE::START_GAME_OP;

            totalNum=getTotalNum();
            numOfPlayersBegin=getTotalNum();
            waitTime=3;
            counttime=15;
            QString data1="";
            for(int i=0;i<6&&thePlayers[i].isReady!=0;i++)
            {
                data1=data1+thePlayers[i].name+" ";
            }
            data1=data1.simplified();
            QString data2="";
            for(int i=0;i<6&&thePlayers[i].isReady!=0;i++)
            {
                char  tmpPlaces;
                tmpPlaces=changePlayerId2(thePlayers[i].num);
                data2=data2+QString(tmpPlaces)+" ";
            }
            data2=data2.simplified();
            NetworkData newData(op,data1,data2);
            //发给所有玩家
            QList<QTcpSocket*>::iterator it1 = recClients.begin();
            for(; it1!= recClients.end(); ++it1)
            {
                server->send(*it1,newData);//发送joinroom给其他玩家
            }



        }
        break;
    }
    case OPCODE::MOVE_OP:
    {
        int m1,n1,m2,n2;
        control=changePlayerId(networkData.data1.at(0).unicode());//Control设为现在在行棋的玩家
        int totalNum=1;//假设data2最后没有空格
        for(int i=0;i<networkData.data2.length();i++)
        {
            if((char)(networkData.data2.at(i).unicode())==' ')
            {
                totalNum+=1;
            }
        }
        for(numOfPlaces=0;numOfPlaces<totalNum;numOfPlaces++)
        {
            int tmpNum=0;
            QString qstringNum;
            qstringNum=networkData.data2.section(" ",numOfPlaces,numOfPlaces);
            if (qstringNum.at(0)!='-')
            {
                if(qstringNum.length()==1)
                {
                    tmpNum=qstringNum.at(0).unicode()-'0';
                }
                else if(qstringNum.length()==2)
                {
                    tmpNum+=(qstringNum.at(0).unicode()-'0')*10;
                    tmpNum+=qstringNum.at(1).unicode()-'0';
                }
                else
                {
                    qDebug()<<"error in MOVE_OP (wrong data2)";
                }
            }
            else
            {
                if(qstringNum.length()==2)
                {
                    tmpNum=qstringNum.at(1).unicode()-'0';
                }
                else if(qstringNum.length()==3)
                {
                    tmpNum+=(qstringNum.at(1).unicode()-'0')*10;
                    tmpNum+=qstringNum.at(2).unicode()-'0';
                }
                else
                {
                    qDebug()<<"error in MOVE_OP (wrong data2)";

                }
                tmpNum*=(-1);
            }
            places[numOfPlaces]= tmpNum+8;
        }
        for(int i=0;i<(numOfPlaces-2)/2;i+=1)
        {
            if(!isLegalMove(places[i*2+1],places[i*2],places[i*2+3],places[i*2+2]))
            {
                OPCODE op=OPCODE::ERROR_OP;
                QString data1= QString::number(400005, 10);
                QString data2="Illegal move";
                NetworkData errData(op,data1,data2);
                server->send(client,errData);
                return;
                //判断是否是合法的移动
            }
            if(i==0)
            {
                m1=places[i+1];
                n1=places[i];
            }
            if(i==((numOfPlaces-2)/2-1))
            {
                m2=places[i*2+3];
                n2=places[i*2+2];
            }
        }
        processClick(m1,n1);
        changeInfo(m1,n1,m2,n2);
        //向所有玩家发送同样的move——op,包括移动者自身

        QList<QTcpSocket*>::iterator it1 = recClients.begin();//【逻辑问题】：如果服务器不给行棋者发送MOVEOOP那行棋者棋子会不会不走
        for(; it1!= recClients.end(); ++it1)
        {
            if(*it1!=client)
            {
                server->send(*it1,networkData);
            }
        }
        //转控制权
        moveControl();

        numOfPlaces=0;
        break;
    }
    case OPCODE::LEAVE_ROOM_OP:
    {
        int leave_num;
        for(int i=0;i<6;i++)
        {
            if(thePlayers[i].name==networkData.data1)
            {
                leave_num=thePlayers[i].num;
                break;
            }
        }
        QList<QTcpSocket*>::iterator it1 = recClients.begin();
        for(int i=0; it1!= recClients.end(); ++it1,i++)
        {
            if(i!=leave_num)
                server->send(*it1,networkData);//发送joinroom给其他玩家
        }
        break;

    }
    default:
    {
        OPCODE op=OPCODE::ERROR_OP;
        QString data1= QString::number(400006, 10);
        QString data2="invaled request";
        NetworkData errData(op,data1,data2);
        server->send(client,errData);
        return;
    }

    }
}
void gameServer::moveControl()
{
    if(this->control==0)
    {
        qDebug()<<"control="<<control;
        judgewin0(0);
        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {

                control = (control+add)%6;
                break;

            }
        }
    }
    else if(this->control==1)
    {
        qDebug()<<"control="<<control;
        switch (numOfPlayersBegin)
        {
        case 2:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin3(1);
            break;
        }
        case 3:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin2(1);
            break;
        }
        case 4:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin1(1);
            break;
        }
        case 5:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin1(1);
            break;
        }
        case 6:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin1(1);
        }
        }
        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {
                control = (control+add)%6;
                break;

            }
        }
    }
    else if(this->control==2)
    {
        qDebug()<<"control="<<control;
        switch (numOfPlayersBegin)
        {
        case 3:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin4(2);
            break;
        }
        case 4:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin3(2);
            break;
        }
        case 5:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin2(2);
            break;
        }
        case 6:
        {
            qDebug()<<"enter case"<<numOfPlayersBegin;
            judgewin2(2);
        }
        }

        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {
                control = (control+add)%6;
                break;

            }
        }
    }
    else if(this->control==3)
    {
        switch (numOfPlayersBegin)
        {
        case 4:
        {
            judgewin4(3);
            break;
        }
        case 5:
        {
            judgewin3(3);
            break;
        }
        case 6:
        {
            judgewin3(3);
        }
        }
        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {
                control = (control+add)%6;
                break;

            }
        }
    }
    else if(this->control==4)
    {
        judgewin4(4);

        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {
                control = (control+add)%6;
                break;

            }
        }
    }
    else if(this->control==5)
    {
        judgewin5(5);
        for(int add=1;add<7;add++)
        {
            if(control+add>6)
                sameRank=0;
            if(thePlayers[(control+add)%6].isReady)
            {
                control = (control+add)%6;
                break;

            }
        }
    }
    else
        qDebug()<<"err in move control";
    if (isGameOver())
    {
        this->hide();
        gameover *over = new gameover();
        over->show();
        over->setWindowTitle("游戏结束");
        QString rec_Rank = "1";
        QString tmp_people, tmp_rank;

        tmp_people = rank[0][0];
        if (rank[0][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "1";
            rec_Rank = "1";
        }
        over->set1(tmp_people, tmp_rank);

        tmp_people = rank[1][0];
        if (rank[1][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "2";
            rec_Rank = "2";
        }
        over->set2(tmp_people, tmp_rank);

        tmp_people = rank[2][0];
        if (rank[2][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "3";
            rec_Rank = "3";
        }
        over->set3(tmp_people, tmp_rank);

        tmp_people = rank[3][0];
        if (rank[3][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "4";
            rec_Rank = "4";
        }
        over->set4(tmp_people, tmp_rank);

        tmp_people = rank[4][0];
        if (rank[4][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "5";
            rec_Rank = "5";
        }
        over->set5(tmp_people, tmp_rank);

        tmp_people = rank[5][0];
        if (rank[5][1])
        {
            tmp_rank = rec_Rank;
        }
        else
        {
            tmp_rank = "6";
            rec_Rank = "6";
        }
        over->set6(tmp_people, tmp_rank);
    }
    OPCODE op=OPCODE::START_TURN_OP;
    QString data1,data2;
    for(int i=0; i<6; i++)
    {
        if(control==thePlayers[i].num)
        {
            data1=changePlayerId2(thePlayers[i].num);
        }
    }
    //data2=时间戳
    qint64 epoch=QDateTime::currentMSecsSinceEpoch();
    data2=QString("%1").arg(epoch);
    NetworkData newData(op,data1,data2);
    QList<QTcpSocket*>::iterator it1 = recClients.begin();
    for(; it1!= recClients.end(); ++it1)
    {
        server->send(*it1,newData);
    }
    counttime=15;

    //    inTime();
}
void gameServer::TimeSlot()
{
    if(waitTime<0)
    {
        this->counttime--;
        if(counttime<0){
            counttime=15;
            for(int i=0;i<6;i++)
            {
                if(thePlayers[i].num==control)
                {
                    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(i))*10;iter !=this->vectorPieces.end()-(getNumAfter(i))*10;)
                    {
                        Piece2* ptr=NULL;
                        ptr=*iter;
                        int row=-1,col=-1;
                        row=ptr->getRow();
                        col=ptr->getColumn();
                        this->cb->chessBoard[row][col].flag=0;
                        this->cb->chessBoard[row][col].piecePlace=NULL;
                        vectorPieces.erase(iter);
                        rank[5-i_bye][0]=control;//在输出界面后i——bye人数排名直接输出轮空
                        thePlayers[i].isReady=0;
                    }
                }
            }

            OPCODE op2=OPCODE::MOVE_OP;
            char tmpData1;
            qDebug()<<"【MOVE_OP】";
            tmpData1=changePlayerId2(control);
            QString data1=QString("%1").arg(tmpData1);
            QString data2="-1";
            NetworkData newData2(op2,data1,data2);

            QList<QTcpSocket*>::iterator it1 = recClients.begin();
            for(; it1!= recClients.end(); ++it1)
            {
                server->send(*it1,newData2);
            }
            i_bye++;

            moveControl();

        }

    }
    else if(waitTime==0)
    {

        OPCODE op2=OPCODE::START_TURN_OP;
        QString data1="";
        QString data2="";
        NetworkData newData2(op2,data1,data2);
        //发给player0
        QList<QTcpSocket*>::iterator it2 = recClients.begin();
        server->send(*it2,newData2);//给第一个人发startturnop
        qDebug ()<<"send START_TURN_OP";
        waitTime--;
    }
    else
    {
        this->counttime--;
        waitTime--;
    }
}


bool gameServer::isLegalMove(int m1, int n1, int m2, int n2)//n是横坐标，m是纵坐标
{

    isLegal.clear();
    if (n1 < 16 && cb->chessBoard[m1][n1 + 1].x != 0 && cb->chessBoard[m1][n1 + 1].flag == 0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1 + 1);
    } //向右挪
    if (n1 > 0 && cb->chessBoard[m1][n1 - 1].x != 0 && cb->chessBoard[m1][n1 - 1].flag == 0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1 - 1);
    } //向左挪

    if (m1 > 0 && cb->chessBoard[m1 - 1][n1].x != 0 && cb->chessBoard[m1 - 1][n1].flag == 0) // y-1,左下
    {
        isLegal.push_back(m1 - 1);
        isLegal.push_back(n1);
    }
    if (m1 > 0 && n1 < 16 && cb->chessBoard[m1 - 1][n1 + 1].x != 0 && cb->chessBoard[m1 - 1][n1 + 1].flag == 0) // y-1,x+1,右下
    {
        isLegal.push_back(m1 - 1);
        isLegal.push_back(n1 + 1);
    }

    if (m1 < 16 && cb->chessBoard[m1 + 1][n1].x != 0 && cb->chessBoard[m1 + 1][n1].flag == 0) // y+1,右上
    {
        isLegal.push_back(m1 + 1);
        isLegal.push_back(n1);
    }
    if (m1 < 16 && n1 > 0 && cb->chessBoard[m1 + 1][n1 - 1].x != 0 && cb->chessBoard[m1 + 1][n1 - 1].flag == 0) // y+1,x-1,左上
    {
        isLegal.push_back(m1 + 1);
        isLegal.push_back(n1 - 1);
    }
    //    }
    for (int i = 0; i < 17; i++)
        for (int j = 0; j < 17; j++)
            cb->chessBoard[i][j].jumpFlag = 0;
    isLegalJump(m1, n1, m2, n2);
    for (vector<int>::iterator it = isLegal.begin(); it != isLegal.end(); it = it + 2)
    {

        if (m2 == *it && n2 == *(it + 1))
        {
            return true;
        }
    }

    return false;
}
void gameServer::isLegalJump(int m1, int n1, int m2, int n2) //判断单步各子跳
{
    if (n1 < 12 && cb->chessBoard[m1][n1 + 1].x != 0 && cb->chessBoard[m1][n1 + 1].flag == 1) //向右挪，如果右面有子且在棋盘内
    {
        if (n1 < 11 && cb->chessBoard[m1][n1 + 2].x != 0 && cb->chessBoard[m1][n1 + 2].flag == 0) //如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {
            if (m1 == m2 && n1 + 2 == n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1 + 2);
            }
        }
    }
    if (n1 > 0 && cb->chessBoard[m1][n1 - 1].x != 0 && cb->chessBoard[m1][n1 - 1].flag == 1)
    {
        if (n1 > 1 && cb->chessBoard[m1][n1 - 2].x != 0 && cb->chessBoard[m1][n1 - 2].flag == 0) //如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            if (m1 == m2 && n1 - 2 == n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1 - 2);
            }
        }
    } //向左挪

    if (m1 < 16 && cb->chessBoard[m1 + 1][n1].x != 0 && cb->chessBoard[m1 + 1][n1].flag == 1) //右上
    {
        if (m1 < 15 && cb->chessBoard[m1 + 2][n1].x != 0 && cb->chessBoard[m1 + 2][n1].flag == 0)
        {
            if (m1 + 2 == m2 && n1 == n2)
            {
                isLegal.push_back(m1 + 2);
                isLegal.push_back(n1);
            }
        }
    }
    if (m1 > 0 && n1 < 16 && cb->chessBoard[m1 - 1][n1 + 1].x != 0 && cb->chessBoard[m1 - 1][n1 + 1].flag == 1) //右下
    {
        if (m1 > 1 && n1 < 15 && cb->chessBoard[m1 - 2][n1 + 2].x != 0 && cb->chessBoard[m1 - 2][n1 + 2].flag == 0)
        {
            if (m1 - 2 == m2 && n1 + 2 == n2)
            {
                isLegal.push_back(m1 - 2);
                isLegal.push_back(n1 + 2);
            }
        }
    }

    if (m1 < 16 && n1 > 0 && cb->chessBoard[m1 + 1][n1 - 1].x != 0 && cb->chessBoard[m1 + 1][n1 - 1].flag == 1) //左上
    {
        if (m1 < 15 && n1 > 1 && cb->chessBoard[m1 + 2][n1 - 2].x != 0 && cb->chessBoard[m1 + 2][n1 - 2].flag == 0)
        {
            if (m1 + 2 == m2 && n1 - 2 == n2)
            {
                isLegal.push_back(m1 + 2);
                isLegal.push_back(n1 - 2);
            }
        }
    }
    if (m1 > 0 && cb->chessBoard[m1 - 1][n1].x != 0 && cb->chessBoard[m1 - 1][n1].flag == 1) //左下
    {
        if (m1 > 1 && cb->chessBoard[m1 - 2][n1].x != 0 && cb->chessBoard[m1 - 2][n1].flag == 0) //如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            if (m1 - 2 == m2 && n1 == n2)
            {
                isLegal.push_back(m1 - 2);
                isLegal.push_back(n1);
            }
        }
    }
}

void gameServer::changeInfo(int m1,int n1,int m2,int n2)
{
    this->cb->chessBoard[m1][n1].flag=0;
    this->cb->chessBoard[m2][n2].piecePlace=this->cb->chessBoard[m1][n1].piecePlace;
    this->cb->chessBoard[m1][n1].piecePlace=NULL;
    this->cb->chessBoard[m2][n2].flag=1;
    this->cb->chessBoard[m2][n2].piecePlace->setPlace(m2,n2);
    this->select->move(cb->chessBoard[m2][n2].x,cb->chessBoard[m2][n2].y);
}
void gameServer::processClick(int m,int n)
{
    this->select=this->cb->chessBoard[m][n].piecePlace;
}
//judgewin0里面的012345是位置
void gameServer::judgewin0(int _i)
{
    int count = 0;
    int line = 0;
    int col = 0;
    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10; iter != this->vectorPieces.end() -(getNumAfter(_i))*10; iter++)
    {
        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 3 && col == 9) || (line == 3 && col == 10) || (line == 3 && col == 11) || (line == 3 && col == 12) ||
                (line == 2 && col == 10) || (line == 2 && col == 11) || (line == 2 && col == 12) || (line == 1 && col == 11) || (line == 1 && col == 12) || (line == 0 && col == 12))
            count++;
    }
    if (count == 10)
    {
        thePlayers[_i].isReady = 0;
        rank[i_rank][0] =(char) (_i+'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        OPCODE op=OPCODE::END_TURN_OP;
        QString data1="",data2="";
        NetworkData newData(op,data1,data2);
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家


    }

}
void gameServer::judgewin1(int _i)
{
    int count = 0;
    int line = 0;
    int col = 0;
    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin() +(getNumBefore(_i))*10; iter != this->vectorPieces.end()- (getNumAfter(_i))*10; iter++)
    {
        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 7 && col == 4) || (line == 6 && col == 4) || (line == 6 && col == 5) || (line == 5 && col == 4) ||
                (line == 5 && col == 5) || (line == 5 && col == 6) || (line == 4 && col == 4) || (line == 4 && col == 5) || (line == 4 && col == 6) || (line == 4 && col == 7))
            count++;
    }
    if (count == 10)
    {
        thePlayers[_i].isReady = 0;
        rank[i_rank][0] = (char)(_i+'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        OPCODE op=OPCODE::END_TURN_OP;
        QString data1="",data2="";
        NetworkData newData(op,data1,data2);
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家

    }

}
void gameServer::judgewin2(int _i)
{
    qDebug()<<"enter judgewin2【【【【【【【【";
    int count = 0;
    int line = 0;
    int col = 0;
    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin() +(getNumBefore(_i))*10; iter != this->vectorPieces.end() - (getNumAfter(_i))*10; iter++)
    {
        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 12 && col == 0) || (line == 12 && col == 1) || (line == 12 && col == 2) || (line == 12 && col == 3) ||
                (line == 11 && col == 1) || (line == 11 && col == 2) || (line == 11 && col == 3) || (line == 10 && col == 2) || (line == 10 && col == 3) || (line == 9 && col == 3))
            count++;
    }
    qDebug()<<"judgewin 2 "<<count;
    if (count == 10)
    {
        thePlayers[_i].isReady = 0;
        rank[i_rank][0] = (char)(_i+'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        qDebug()<<"1";
        OPCODE op=OPCODE::END_TURN_OP;
        qDebug()<<"2";
        QString data1="",data2="";
        qDebug()<<"3";
        NetworkData newData(op,data1,data2);
        qDebug()<<"4";
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        qDebug()<<"5";
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家
        qDebug()<<"6";

    }

}
void gameServer::judgewin3(int _i)
{
    int count = 0;
    int line = 0;
    int col = 0;

    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin() +(getNumBefore(_i))*10; iter != this->vectorPieces.end() - (getNumAfter(_i))*10; iter++)
    {
        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 16 && col == 4) || (line == 15 && col == 4) || (line == 15 && col == 5) || (line == 14 && col == 4) ||
                (line == 14 && col == 5) || (line == 14 && col == 6) || (line == 13 && col == 4) || (line == 13 && col == 5) || (line == 13 && col == 6) || (line == 13 && col == 7))
            count++;
    }
    if (count == 10)
    {
        thePlayers[_i] .isReady= 0;
        rank[i_rank][0] = (char)(_i+'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        OPCODE op=OPCODE::END_TURN_OP;
        QString data1="",data2="";
        NetworkData newData(op,data1,data2);
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家

    }

}
void gameServer::judgewin4(int _i)
{
    int count = 0;
    int line = 0;
    int col = 0;
    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;iter != this->vectorPieces.end() - (getNumAfter(_i))*10; iter++)
    {
        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 12 && col == 9) || (line == 12 && col == 10) || (line == 12 && col == 11) || (line == 12 && col == 12) ||
                (line == 11 && col == 10) || (line == 11 && col == 11) || (line == 11 && col == 12) || (line == 10 && col == 11) || (line == 10 && col == 12) || (line == 9 && col == 12))
            count++;
    }
    if (count == 10)
    {
        thePlayers[_i].isReady = 0;
        rank[i_rank][0] = (char)(_i+'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        OPCODE op=OPCODE::END_TURN_OP;
        QString data1="",data2="";
        NetworkData newData(op,data1,data2);
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家
    }

}
void gameServer::judgewin5(int _i)
{
    int count = 0;
    int line = 0;
    int col = 0;

    for (vector<Piece2 *>::iterator iter = this->vectorPieces.begin() +(getNumBefore(_i))*10; iter != this->vectorPieces.end()-(getNumAfter(_i))*10; iter++)
    {

        Piece2 *ptr = new Piece2();
        ptr = *iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if ((line == 7 && col == 13) || (line == 6 && col == 13) || (line == 6 && col == 14) || (line == 5 && col == 13) ||
                (line == 5 && col == 14) || (line == 5 && col == 15) || (line == 4 && col == 13) || (line == 4 && col == 14) || (line == 4 && col == 15) || (line == 4 && col == 16))
            count++;
    }
    if (count == 10)
    {
        thePlayers[_i].isReady = 0;
        rank[i_rank][0] = (char)(_i +'0');
        if (sameRank)
        {
            rank[i_rank][1] = 1;
        }
        else
        {
            rank[i_rank][1] = 0;
            sameRank = 1;
        }
        i_rank++;
        sameRank = 0;
        OPCODE op=OPCODE::END_TURN_OP;
        QString data1="",data2="";
        NetworkData newData(op,data1,data2);
        QList<QTcpSocket*>::iterator it2 = recClients.begin()+_i;
        server->send(*it2,newData);//END_TURN_OP发送给对应玩家
    }

}
bool gameServer::isGameOver()
{
    if (i_bye + i_rank == totalNum)
    {
        OPCODE op=OPCODE::END_GAME_OP;
        QString data1="",data2="";
        for(int i=0;i<totalNum-i_bye;i++)
        {
            char tmp=rank[i][0]-'0'+'A';
            data1=data1+QString("%1 ").arg(tmp);
        }
        NetworkData networkData(op,data1,data2);
        QList<QTcpSocket*>::iterator it1 = recClients.begin();
        myTimer->stop();
        for(int i=0; it1< recClients.end(); i++)
        {

            server->send(*it1,networkData);
            it1++;

        }
        isRunning=false;
        return true;
    }
    else
        return false;
}
