#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <vector>
#include <QDebug>
#include "gameplay.h"
#include "ui_gameplay.h"
#include "board.h"
#include "mainwindow.h"
#include"gameover.h"
#include"networksocket.h"
#include<QMessageBox>
#include "networkdata.h"
#include "ui_joinroomclient.h"
#include "joinroomclient.h"
#include "gameserver.h"
#include <QTime>
#include"networkserver.h"

using namespace std;

gameplay::gameplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameplay)
{
    ui->setupUi(this);
    for(int i=0;i<5;i++)
    {
        thePlayers[i].isReady=0;
        thePlayers[i].num=-1;
        thePlayers[i]. name="";
    }
    this->control = 0;
    this->select = NULL;//select是我选中想移动的棋子（？）
    this->produceRectF();
    cb = new Board();
    cb->initial();
    lightColor=0;
    numOfWinner=0;

    numOfPlaces=0;
    myTurn=-1;
    this->socket = new NetworkSocket(new QTcpSocket(),this);

    connect(socket,&NetworkSocket::receive,this,&gameplay::receive);
    connect(socket->base(),&QAbstractSocket::disconnected,[=]() {
        QMessageBox::critical(this,tr("Connection lost"),tr("Connection to server has closed"));
    });
    //为每一个坐标添加正方形点击区域
    QRectF* qr;
    int k = 0;
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            qr=this->vectorRectF.at(k);
            k++;
            qr->setRect(this->cb->chessBoard[i][j].x,this->cb->chessBoard[i][j].y,23,23);
            //setRect是设置一个矩形区域
        }
    }
    for(int i=0;i<50;i++)
    {
        places[i]=0;
    }
    myTimer = new QTimer(this);//计时器
    this->counttime = 16;
    connect(myTimer,&QTimer::timeout,[=](){
        counttime--;
        if(counttime>=0)
        {
            ui->lcdNumber->display(counttime);
        }
        else
        {
            ui->lcdNumber->display(0);
        }

    });
    updateTime = this->startTimer(100);
    PTime=3;
    range=0;
    attck = 3;
    defence = 1;
    fornest = 1;
    AIOrNot=false;
    isRunning=true;
}
gameplay::~gameplay()
{
    delete ui;
}
int gameplay::changePlayerId(QChar ch)//接收时向玩家转换
{
    char tmpCh;
    tmpCh = ch.toLatin1();//Qchar转为char
   qDebug()<<"tmpCh ="<<tmpCh ;
   qDebug()<<"beginNum="<<beginNum;
    switch(beginNum)
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
char gameplay:: changePlayerId2(int c)//把01转成ABC
{
    switch(beginNum)
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
void gameplay::timerEvent(QTimerEvent*e){
    if(updateTime==e->timerId())
    {
        //qDebug()<<"HERE!!!PTime="<<PTime;
        if(PTime<0)
        {
            for(int i=0;i<17;i++)
                for(int j=0;j<17;j++)
                    this->cb->chessBoard[i][j].paintFlag=-1;
            PTime=3;
        }

        else
        {
            bool tmpflag=false;
            for(int i=0;i<17;i++)
                for(int j=0;j<17;j++)
                    if(this->cb->chessBoard[i][j].paintFlag>=0)
                        tmpflag=true;
            if(tmpflag)
            {
                PTime--;
            }
        }
        update();
    }
}

void gameplay::receive(NetworkData networkData)
{
    int num=0;
    switch(networkData.op)
    {
    case OPCODE::JOIN_ROOM_REPLY_OP:
    {
        //1.解析房间内[的所有玩家的用户名data1（字符串）
        qDebug()<<"networkData.data2.length()="<<networkData.data2.length();
        while(num<6&&num<networkData.data2.length())
        {
            thePlayers[num].name=networkData.data1.section(" ",num,num);
            thePlayers[num].isReady=(char)(networkData.data2.at(num).unicode())-'0';
            thePlayers[num].num=num;
            qDebug()<<"【thePlayers[num].isReady】="<<thePlayers[num].isReady;
            num++;
        }
        break;
    }
    case OPCODE::JOIN_ROOM_OP:
    {
        while(thePlayers[num].num!=-1)
        {
            num++;
        }

        thePlayers[num].name=networkData.data1;
        thePlayers[num].isReady=0;
        thePlayers[num].num=num;
        break;
    }
    case OPCODE::PLAYER_READY_OP:
    {
        //解析新准备好的玩家的用户名data1（字符串）
        int i=0;
        bool isMe=true;
        while(i<6)
        {
            if( thePlayers[i].name==networkData.data1)
            {
                qDebug()<<thePlayers[i].name;
                thePlayers[i].isReady=1;
                num++;
                isMe=false;
                break;
            }
            i++;
        }
        if(isMe)
        {
            me.isReady=1;
        }
        qDebug()<<"【me.isReady】="<<me.isReady;
        break;
    }
    case OPCODE::START_GAME_OP:
    {

        myTimer->start(1000);
        int j=0;
        qDebug()<<"【START_GAME_OP】";
        beginNum=getTotalNum();
        qDebug()<<" beginNum="<< beginNum;
        for(j=0;j<(networkData.data2.length()+1)/2;j++)
        {
            bool isMe=true;
            int i=-1;
            while(i<4)
            {
                i++;
                if( thePlayers[i].name==networkData.data1.section(" ",j,j))
                {
                    thePlayers[i].num=changePlayerId(networkData.data2.at(2*j).unicode());
                    num++;
                    isMe=false;
                    //ui->textBrowser_9->setText(thePlayers[i].name);
                    break;
                }

            }
            if(isMe)
            {
                me.num=changePlayerId(networkData.data2.at(2*j).unicode());
                myTurn=me.num;
                qDebug()<<"-------------------------";
                qDebug()<<"myTurn="<<myTurn;
                ui->textBrowser_9->setText(me.name);
                setPower();
            }
        }
        producePiece();
         this->show();
        break;
    }
    case OPCODE::START_TURN_OP:
    {
        if(networkData.data1==(QChar)(changePlayerId2(myTurn)))//networkData.data1==(QChar)(myTurn+'A')
        {
            control=myTurn;
            if(AIOrNot)
            {
                buildPath(AIJudge(myTurn),myTurn);
            }
        }
        break;
    }
    case OPCODE::MOVE_OP://自己不会收到自己的move
    {
        counttime=16;
        control=changePlayerId(networkData.data1.at(0).unicode());//Control设为现在在行棋的玩家
        if(networkData.data2.at(0).unicode()=='-'&&networkData.data2.at(1).unicode()=='1'&&networkData.data2.length()==2)
        {
            int tmpId;
            tmpId=changePlayerId(networkData.data1.at(0).unicode());
            qDebug()<<"tmpId="<<tmpId;
            for(int i=0;i<5;i++)
            {
                if(thePlayers[i].num==tmpId)
                {
                    qDebug()<<"here!!!"<<getNumBefore(thePlayers[i].num)<<getNumAfter(thePlayers[i].num)*10;
                    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(thePlayers[i].num)*10;iter !=this->vectorPieces.end()-getNumAfter(thePlayers[i].num)*10;)
                    {
                        thePlayers[i].isReady=0;
                        Piece2* ptr=NULL;
                        ptr=*iter;
                        qDebug()<<"before hide";
                        ptr->hide();
                        qDebug()<<"after hide";
                        int row=-1,col=-1;
                        row=ptr->getRow();
                        col=ptr->getColumn();
                        this->cb->chessBoard[row][col].flag=0;
                        this->cb->chessBoard[row][col].piecePlace=NULL;
                        vectorPieces.erase(iter);
                    }
                }
            }
            if(me.num==tmpId)
            {
                qDebug()<<"here!!!"<<getNumBefore(me.num)<<getNumAfter(me.num)*10;
                for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(me.num)*10;iter !=this->vectorPieces.end()-getNumAfter(me.num)*10;)
                {
                    me.isReady=0;
                    Piece2* ptr=NULL;
                    ptr=*iter;
                    qDebug()<<"before hide";
                    ptr->hide();
                    qDebug()<<"after hide";
                    int row=-1,col=-1;
                    row=ptr->getRow();
                    col=ptr->getColumn();
                    this->cb->chessBoard[row][col].flag=0;
                    this->cb->chessBoard[row][col].piecePlace=NULL;

                    vectorPieces.erase(iter);
                }
            }
        }
        else
        {
            int totalNum=1;//假设data2最后没有空格
            for(int i=0;i<networkData.data2.length();i++)
            {
                if((char)(networkData.data2.at(i).unicode())==' ')
                {
                    totalNum+=1;
                }
            }
            cpyNumOfPlaces[control]=totalNum;
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
                tmpNum+=8;
                places[numOfPlaces]= tmpNum;
                cpyPlaces[control][numOfPlaces]=tmpNum;
            }
            int m1=-1,n1=-1,m2=-1,n2=-1;
            for(int i=0;i<(numOfPlaces-2)/2;i+=1)
            {

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
            changeInfo(m1,n1,m2,n2);


            numOfPlaces=0;

        }
        showPlaces(control);

        break;
    }
    case OPCODE::LEAVE_ROOM_OP:
    {
        QString tmpName;
        tmpName=networkData.data1;
        for(int i=0;i<5;i++)
        {
            if(thePlayers[i].name==tmpName)
            {
                for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+thePlayers[i].num*10;iter !=this->vectorPieces.end()-(50-thePlayers[i].num*10);)
                {
                    thePlayers[i].isReady=0;
                    Piece2* ptr=NULL;
                    ptr=*iter;
                    int row=-1,col=-1;
                    row=ptr->getRow();
                    col=ptr->getColumn();
                    vectorPieces.erase(iter);
                    this-> repaint(row,col,23,23);
                }
            }
        }
        break;
    }
    case OPCODE::END_TURN_OP:
    {
        me.isReady=0;
        myTurn=-2;
        ui->textBrowser->setText("你已经取得胜利，请等待其他玩家完成游戏");
        break;
    }
    case OPCODE::END_GAME_OP:
    {
        isRunning=false;
        ui->textBrowser_8->setText("rank");
        int tmpNum=getTotalNum();
        for(int i=0;networkData.data1.section(" ",i,i)!="";i++)
        {

            switch (i)
            {
            case 0:
            {
                ui->textBrowser_2->setText(networkData.data1.section(" ",i,i));
                break;
            }
            case 1:
            {
                ui->textBrowser_3->setText(networkData.data1.section(" ",i,i));
                break;
            }
            case 2:
            {
                ui->textBrowser_4->setText(networkData.data1.section(" ",i,i));
                break;
            }
            case 3:
            {
                ui->textBrowser_5->setText(networkData.data1.section(" ",i,i));
                break;
            }
            case 4:
            {
                ui->textBrowser_6->setText(networkData.data1.section(" ",i,i));
                break;
            }
            case 5:
            {
                ui->textBrowser_7->setText(networkData.data1.section(" ",i,i));
                break;
            }
            }
        }
        break;
    }
    case OPCODE::ERROR_OP:
    {
       qDebug()<<networkData.data1;
        if(networkData.data1=="INVALID_MOVE")
        {
            qDebug()<<"【！！！】";
            int m1,n1,m2,n2;
            m1=cpyPlaces[control][1];
            n1=cpyPlaces[control][0];
            int i=(numOfPlaces-2)/2-1;
            m2=places[i*2+3];
            n2=places[i*2+2];
            changeInfo(m2,n2,m1,n1);
            numOfPlaces=0;
            //行棋错误的时候棋子回到原位
        }
        else if(networkData.data1=="OUTTURN_MOVE")
        {
            int m1,n1,m2,n2;
            m1=cpyPlaces[control][1];
            n1=cpyPlaces[control][0];
            int i=(numOfPlaces-2)/2-1;
            m2=places[i*2+3];
            n2=places[i*2+2];
            changeInfo(m2,n2,m1,n1);
            numOfPlaces=0;
        }
        else
        {
            qDebug()<<"err  in ERROR_CODE";
        }
        break;
    }
    default:
        qDebug()<<"err  in receive";

    }
}

void gameplay::on_pushButton_2_clicked()//点击走完按钮发送MOVE_OP信息
{
    if(isRunning)
    {
        QString data1,data2;
        char tmpData1;
        tmpData1=changePlayerId2(me.num);
        data1=QString("%1").arg(tmpData1);
        qDebug()<<"---------------------------------";
        qDebug()<<data1;
        for(int i=0;i<numOfPlaces;i++)
        {
            QString tmpStr;
            tmpStr=QString("%1 ").arg(places[i]-8);
            data2=data2+tmpStr;
        }
        qDebug()<<"     "<<data1<<" "<<data2;
        data2=data2.trimmed();
        OPCODE tmpOp=OPCODE::MOVE_OP;
        NetworkData newData(tmpOp,data1,data2);
        socket->send(newData);
    }

}

void gameplay::on_pushButton_clicked()//返回
{
    QString data1,data2;
    data1=roomIdInGameplay;
    data2=me.name;
    OPCODE tmpOp=OPCODE::LEAVE_ROOM_OP;
    NetworkData newData(tmpOp,data1,data2);
    socket->send(newData);
    socket->bye();
    this->hide();
    joinRoomClient* jrc = new joinRoomClient(this);
    jrc->show();
}
void gameplay::on_pushButton_3_clicked()//人机切换
{
    if(AIOrNot==false)
    {
        AIOrNot=true;
        if(myTurn==control)
        {
            buildPath(AIJudge(myTurn),myTurn);
        }
    }
    else if(AIOrNot==true)
    {
        AIOrNot=false;
    }
}
void gameplay::on_pushButton_4_clicked()//复现
{
    int _control= ui->lineEdit->text().toInt();
    showPlaces(_control);
}
void gameplay::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton&&control==myTurn)
    {
        int m1=-1,n1=-1;
        int m=-1,n=-1;
        for(int i=0;i<17;i++)
        {
            for(int j=0;j<17;j++)
            {
                if(cb->chessBoard[i][j].x<=e->pos().x() &&cb->chessBoard[i][j].x+23>=e->pos().x()&& cb->chessBoard[i][j].y<=e->pos().y()&&cb->chessBoard[i][j].y+23>=e->pos().y()){
                    m1=i;//纵坐标
                    n1=j;//横坐标
                }
            }
        }
        if(m1>=0&&n1>=0)
        {
            places[numOfPlaces]=n1;
            cpyPlaces[myTurn][numOfPlaces]=n1;
            numOfPlaces++;
            places[numOfPlaces]=m1;
            cpyPlaces[myTurn][numOfPlaces]=m1;
            numOfPlaces++;
            cpyNumOfPlaces[myTurn]=numOfPlaces;
            m=cpyPlaces[myTurn][numOfPlaces-3];
            n=cpyPlaces[myTurn][numOfPlaces-4];
            qDebug()<<"changeinfo begin";
            qDebug()<<"numOfPlaces="<<numOfPlaces;
            qDebug()<<"numOfPlaces="<<numOfPlaces<<" m="<<m<<" n="<<n<<" m1="<<m1<<" n1="<<n1;
            changeInfo(m,n,m1,n1);
            qDebug()<<"changeinfo begin";
        }


    }
}

void gameplay::produceRectF()//创建棋盘可点击区域链表//right
{

    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            QRectF* rect=new QRectF();//QRectF：一个矩形区域
            this->vectorRectF.push_back(rect);//pushback是在容器最后加一个
        }
    }
}
void  gameplay::producePiece()
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
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-(getNumAfter(0))*10;iter++)
        {
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(1))*10;iter++)
        {
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
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
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-(getNumAfter(0))*10;iter++)
            //iterator 行为类似于指针，用于遍历整个容器
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
            //connet(指向棋子的指针，棋子里证明被点到的函数，本类：gameplay,gameplay里的处理点击的函数)
            //begin是vector的开始，有60个棋子
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(1))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(2))*10;iter !=this->vectorPieces.end()-(getNumAfter(2))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick2(int,int)));
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
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-(getNumAfter(0))*10;iter++)
            //iterator 行为类似于指针，用于遍历整个容器
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
            //connet(指向棋子的指针，棋子里证明被点到的函数，本类：gameplay,gameplay里的处理点击的函数)
            //begin是vector的开始，有60个棋子
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(1))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(2))*10;iter !=this->vectorPieces.end()-(getNumAfter(2))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick2(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(3))*10;iter !=this->vectorPieces.end()-(getNumAfter(3))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick3(int,int)));
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
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-(getNumAfter(0))*10;iter++)
            //iterator 行为类似于指针，用于遍历整个容器
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
            //connet(指向棋子的指针，棋子里证明被点到的函数，本类：gameplay,gameplay里的处理点击的函数)
            //begin是vector的开始，有60个棋子
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(1))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(2))*10;iter !=this->vectorPieces.end()-(getNumAfter(2))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick2(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(3))*10;iter !=this->vectorPieces.end()-(getNumAfter(3))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick3(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(4))*10;iter !=this->vectorPieces.end()-(getNumAfter(4))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick4(int,int)));
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
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-(getNumAfter(0))*10;iter++)
            //iterator 行为类似于指针，用于遍历整个容器
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
            //connet(指向棋子的指针，棋子里证明被点到的函数，本类：gameplay,gameplay里的处理点击的函数)
            //begin是vector的开始，有60个棋子
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(1))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(2))*10;iter !=this->vectorPieces.end()-(getNumAfter(2))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick2(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(3))*10;iter !=this->vectorPieces.end()-(getNumAfter(3))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick3(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(4))*10;iter !=this->vectorPieces.end()-(getNumAfter(4))*10;iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick4(int,int)));
        }
        for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(5))*10;iter !=this->vectorPieces.end();iter++)
        {
            //Piece2* ptr = *iter;
            connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick5(int,int)));
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
int gameplay::getNumBefore(int i)
{

    int num=0;
    for(int j=0;j<5;j++)
    {
        if(thePlayers[j].isReady&&thePlayers[j].num<i)
            num+=1;
    }
    if(me.num<i&&me.isReady)
        num++;
    return num;
}
int gameplay::getNumAfter(int i)
{
    int num=0;
    for(int j=0;j<5;j++)
    {
        if(thePlayers[j].isReady&&thePlayers[j].num>i)
            num+=1;
    }
    if(me.num>i&&me.isReady)
        num++;
    return num;
}
int gameplay::getTotalNum()
{
    int num=0;
    for(int j=0;j<5;j++)
    {
        qDebug()<<"thePlayers[j].isReady------------------------------------";
        qDebug()<<thePlayers[j].isReady;
        if(thePlayers[j].isReady)
        {
            num+=1;
        }
    }
    if(me.isReady)
        num++;
    return num;
}
//setPiece 中的012345 指的是位置而非实际的玩家
void gameplay::setPiece0(int _i)
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
void gameplay::setPiece1(int _i)
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(_i))*10;
    int begin=12;//小三角形中轴线
    for(int i=9;i<13;i++)
    {
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+(getNumBefore(1))*10;iter !=this->vectorPieces.end()-(getNumAfter(_i))*10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
    ptr=NULL;
}
void gameplay::setPiece2(int _i)
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
void gameplay::setPiece3(int _i)
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
void gameplay::setPiece4(int _i)
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
void gameplay::setPiece5(int _i)
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
//processClick中的012345对应的是玩家和棋子颜色
//n是横坐标，m是纵坐标
void gameplay::processClick0(int m,int n)
{
    //处理鼠标点击事件
    if(control==0&&myTurn==0){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chesss.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            this->select->setIcon(QIcon(":/res/chess.png"));
            this->select=NULL;
        }
    }

}
void gameplay::processClick1(int m,int n)
{
    //处理鼠标点击事件
    qDebug()<<"control="<<control<<" myTurn="<<myTurn;
    if(control==1&&myTurn==1){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess2s.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            this->select->setIcon(QIcon(":/res/chess2.png"));
            this->select=NULL;
        }
    }

}
void gameplay::processClick2(int m,int n)
{
    //处理鼠标点击事件
    if(control==2&&myTurn==2){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess3s.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            this->select->setIcon(QIcon(":/res/chess3.png"));
            this->select=NULL;
        }
    }

}
void gameplay::processClick3(int m,int n)
{
    //处理鼠标点击事件
    if(control==3&&myTurn==3){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess4s.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;

            this->select->setIcon(QIcon(":/res/chess4.png"));
            this->select=NULL;
        }
    }

}
void gameplay::processClick4(int m,int n)
{
    //处理鼠标点击事件
    if(control==4&&myTurn==4){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess5s.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            this->select->setIcon(QIcon(":/res/chess5.png"));
            this->select=NULL;
        }
    }

}
void gameplay::processClick5(int m,int n)
{
    //处理鼠标点击事件
    if(control==5&&myTurn==5){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            places[numOfPlaces]=n;
            cpyPlaces[control][numOfPlaces]=n;
            numOfPlaces++;
            places[numOfPlaces]=m;
            cpyPlaces[control][numOfPlaces]=m;
            numOfPlaces++;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess6s.png"));
        }
        else{
            this->lightColor=0;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            places[numOfPlaces]=0;
            cpyPlaces[control][numOfPlaces]=0;
            if(numOfPlaces>0)
                numOfPlaces--;
            this->select->setIcon(QIcon(":/res/chess6.png"));
            this->select=NULL;
        }
    }

}
void gameplay::showPlaces(int _i)//_i 是玩家的control
{
    qDebug()<<"_i="<<_i;
    for(int i=0;i<cpyNumOfPlaces[_i];i+=2)
    {
        this->cb->chessBoard[cpyPlaces[_i][i]][cpyPlaces[_i][i+1]].paintFlag=_i;
        qDebug()<<"chessBoard["<<cpyPlaces[_i][i]<<"]["<<cpyPlaces[_i][i+1]<<"].paintFlag="<<_i;
    }
}
void gameplay::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QImage image1;
    image1.load(":/res/win.png");
    QImage image2=image1.scaled(1151,600);
    QPalette palette;
    palette.setBrush(this->backgroundRole(),QBrush(image2));//设置背景
    setPalette(palette);
    QRectF recBack(0,0, 1151, 600);
    QImage backGround;
    backGround.load(":/res/win.png");
    p.drawImage(recBack,backGround);
    for(int i=0;i<17;i++)
        for(int j=0;j<17;j++)
        {
            if(this->cb->chessBoard[i][j].paintFlag>=0)
            {
                qDebug()<<"PAINT EVENT";
                switch(this->cb->chessBoard[i][j].paintFlag)
                {
                case 0:
                {
                    qDebug()<<"PAINT case 0*************************";
                    p.setBrush(QBrush(QColor(0,0,0,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }
                case 1:
                {
                     qDebug()<<"PAINT case 1*************************";
                    p.setBrush(QBrush(QColor(249,199,0,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }
                case 2:
                {
                     qDebug()<<"PAINT case 2*************************";
                    p.setBrush(QBrush(QColor(0,250,35,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }
                case 3:
                {
                     qDebug()<<"PAINT case 3*************************";
                    p.setBrush(QBrush(QColor(251,39,0,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }
                case 4:
                {
                     qDebug()<<"PAINT case 4*************************";
                    p.setBrush(QBrush(QColor(3,203,248,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }
                case 5:
                {
                     qDebug()<<"PAINT case 5*************************";
                    p.setBrush(QBrush(QColor(248,0,142,120),Qt::SolidPattern));
                    QRectF rectangle(this->cb->chessBoard[j][i].x-2,this->cb->chessBoard[j][i].y-2,27,27);
                    p.drawRect(rectangle);
                    break;
                }

                }
            }
        }
}
void gameplay::changeInfo(int m1,int n1,int m2,int n2)
{
    //qDebug()<<"changeinfo:"<<this->cb->chessBoard[m1][n1].piecePlace<<"("<<m1<<","<<n1<<")";
    this->select=this->cb->chessBoard[m1][n1].piecePlace;
    this->cb->chessBoard[m1][n1].flag=0;
    this->cb->chessBoard[m2][n2].piecePlace=this->cb->chessBoard[m1][n1].piecePlace;
    this->cb->chessBoard[m2][n2].piecePlace->setPlace(m2,n2);
    this->cb->chessBoard[m1][n1].piecePlace=NULL;
    this->cb->chessBoard[m2][n2].flag=1;
    this->select->move(cb->chessBoard[m2][n2].x,cb->chessBoard[m2][n2].y);
    lightColor=0;
    switch(myTurn)
    {
    case 0:
    {

        this->select->setIcon(QIcon(":/res/chess.png"));
        break;

    }
    case 1:
    {

        this->select->setIcon(QIcon(":/res/chess2.png"));
        break;
    }
    case 2:
    {

        this->select->setIcon(QIcon(":/res/chess3.png"));
        break;
    }
    case 3:
    {

        this->select->setIcon(QIcon(":/res/chess4.png"));
        break;
    }
    case 4:
    {

        this->select->setIcon(QIcon(":/res/chess5.png"));
        break;
    }
    case 5:
    {

        this->select->setIcon(QIcon(":/res/chess6.png"));
        break;
    }
    default:
    {
        qDebug()<<"err in myturn";
    }
    }

    switch (control)
    {
    case 0:
    {
        this->select->setIcon(QIcon(":/res/chess.png"));
        break;
    }
    case 1:
    {
        this->select->setIcon(QIcon(":/res/chess2.png"));
        break;
    }
    case 2:
    {
        this->select->setIcon(QIcon(":/res/chess3.png"));
        break;
    }
    case 3:
    {
        this->select->setIcon(QIcon(":/res/chess4.png"));
        break;
    }
    case 4:
    {
        this->select->setIcon(QIcon(":/res/chess5.png"));
        break;
    }
    case 5:
    {
        this->select->setIcon(QIcon(":/res/chess6.png"));
        break;
    }
    }

}
void gameplay::setPower()
{
    int powerdata0[17][17] = {
        {-1,-1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1, 3, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1, 5, 5, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1, 9,10,10, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { 0, 1, 2, 5,12,14,13,14,12, 5, 2, 1, 0,-1,-1,-1,-1},
        {-1, 1, 2, 5,10,15,18,18,15,10, 5, 2, 1,-1,-1,-1,-1},
        {-1,-1, 5,10,20,26,28,27,28,26,20,10, 5,-1,-1,-1,-1},
        {-1,-1,-1, 5,15,30,35,34,34,35,30,15, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,18,40,46,48,47,48,46,40,18,-1,-1,-1,-1},
        {-1,-1,-1,-1,10,34,50,58,60,60,58,50,34,10,-1,-1,-1},
        {-1,-1,-1,-1,15,30,40,67,70,71,70,67,40,30,15,-1,-1},
        {-1,-1,-1,-1, 1,15,30,70,82,84,84,82,70,30,15, 1,-1},
        {-1,-1,-1,-1, 0,15,30,45,88,92,91,92,88,45,30,15, 0},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,94,93,93,94,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,95,94,95,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,97,97,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,99,-1,-1,-1,-1},
    };
    int powerdata2[17][17] ={
        {-1,-1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,15, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,30,15,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,45,30,30,10,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {99,97,95,94,88,70,40,34,18, 5, 5, 1, 0,-1,-1,-1,-1},
        {-1,97,94,93,92,82,67,50,40,15,10, 2, 1,-1,-1,-1,-1},
        {-1,-1,95,93,91,84,70,58,46,30,20, 5, 2,-1,-1,-1,-1},
        {-1,-1,-1,94,92,84,71,60,48,35,26,10, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,88,82,70,60,47,34,28,15,12,-1,-1,-1,-1},
        {-1,-1,-1,-1,45,70,67,58,48,34,27,18,14, 9,-1,-1,-1},
        {-1,-1,-1,-1,30,30,40,50,46,35,28,18,13,10, 5,-1,-1},
        {-1,-1,-1,-1,15,15,30,34,40,30,26,15,14,10, 5, 3,-1},
        {-1,-1,-1,-1, 0, 1,15,10,18,15,20,10,12, 9, 5, 3, 1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1, 5,10, 5, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 2, 2,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1,-1,-1},
    };
    int powerdata3[17][17] ={
        {-1,-1,-1,-1,99,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,97,97,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,95,94,95,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,94,93,93,94,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { 0,15,30,45,88,92,91,92,88,45,30,15, 0,-1,-1,-1,-1},
        {-1, 1,15,30,70,82,84,84,82,70,30,15, 1,-1,-1,-1,-1},
        {-1,-1,15,30,40,67,70,71,70,67,40,30,15,-1,-1,-1,-1},
        {-1,-1,-1,10,34,50,58,60,60,58,50,34,10,-1,-1,-1,-1},
        {-1,-1,-1,-1,18,40,46,48,47,48,46,40,18,-1,-1,-1,-1},
        {-1,-1,-1,-1, 5,15,30,35,34,34,35,30,15, 5,-1,-1,-1},
        {-1,-1,-1,-1, 5,10,20,26,28,27,28,26,20,10, 5,-1,-1},
        {-1,-1,-1,-1, 1, 2, 5,10,15,18,18,15,10, 5, 2, 1,-1},
        {-1,-1,-1,-1, 0, 1, 2, 5,12,14,13,14,12, 5, 2, 1, 0},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1, 9,10,10, 9,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 5, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 3, 3,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1},
    };
    int powerdata4[17][17] ={
        {-1,-1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1, 1,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,15,15,30,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,20,30,30,45,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { 0, 1, 5, 5,18,34,40,70,88,94,95,97,99,-1,-1,-1,-1},
        {-1, 1, 2,10,15,40,50,67,82,92,93,94,97,-1,-1,-1,-1},
        {-1,-1, 2, 5,20,30,46,58,70,84,91,93,95,-1,-1,-1,-1},
        {-1,-1,-1, 5,10,26,35,48,60,71,84,92,94,-1,-1,-1,-1},
        {-1,-1,-1,-1,12,15,28,34,47,60,70,82,88,-1,-1,-1,-1},
        {-1,-1,-1,-1, 9,14,18,27,34,48,58,67,70,45,-1,-1,-1},
        {-1,-1,-1,-1, 5,10,13,18,28,35,46,50,40,30,30,-1,-1},
        {-1,-1,-1,-1, 3, 5,10,14,15,26,30,40,34,30,15,15,-1},
        {-1,-1,-1,-1, 1, 3, 5, 9,12,10,20,15,18,10,15, 1, 0},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 5,10, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 2, 2, 5,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1,-1,-1},
    };
    switch (myTurn)//初始化权值
    {
    case 0: {
        switch(getTotalNum())
        {
        case 2:
        {;
            for(int i=0;i<17;i++)
            {
                for(int j=0;j<17;j++)
                {
                    cb->AIPower[i][j]=powerdata0[16-i][j];
                    cb->humanPower[0][i][j]=powerdata3[16-i][j];
                }
            }
            break;
        }
        case 3:
        {
            for(int i=0;i<17;i++)
            {
                for(int j=0;j<17;j++)
                {
                    cb->AIPower[i][j]=powerdata0[16-i][j];
                    cb->humanPower[0][i][j]=powerdata2[16-i][j];
                    cb->humanPower[1][i][j]=powerdata4[16-i][j];
                }
            }
            break;
        }
        default:{
            qDebug()<<getTotalNum()<<" err in AIplay";
        }
        }

        break;
    }
    case 1:
    {
        switch(getTotalNum())
        {
        case 2:
        {
            for(int i=0;i<17;i++)
            {
                for(int j=0;j<17;j++)
                {
                    cb->AIPower[i][j]=powerdata3[16-i][j];
                    cb->humanPower[0][i][j]=powerdata0[16-i][j];
                }
            }
            break;
        }
        case 3:
        {
            for(int i=0;i<17;i++)
            {
                for(int j=0;j<17;j++)
                {
                    cb->AIPower[i][j]=powerdata2[16-i][j];
                    cb->humanPower[0][i][j]=powerdata0[16-i][j];
                    cb->humanPower[1][i][j]=powerdata4[16-i][j];
                }
            }
            break;
        }
        default:{
            qDebug()<<getTotalNum()<<" err in AIplay";
        }
        }
        break;
    }
    case 2:
    {

        for(int i=0;i<17;i++)
        {
            for(int j=0;j<17;j++)
            {
                cb->AIPower[i][j]=powerdata4[16-i][j];
                cb->humanPower[0][i][j]=powerdata0[16-i][j];
                cb->humanPower[1][i][j]=powerdata2[16-i][j];
            }
        }
        break;
    }

    default:
    {
        qDebug()<<"err in ai play";
    }
    }
    qDebug()<<"end setPower";
}
void gameplay::buildPath(int pnumber,int _myturn)//在已知移动哪个棋子时找出路径
{
    if(isRunning)
    {
        qDebug()<<"enter buildPath";
        //数据初始化
        int x=0;
        int y=0;
        int m1=0;
        int n1=0;
        int mmax=0;
        int nmax=0;
        int Opower = 0;
        int Npower = 0;
        int len = 0;
        Olongjump.clear();
        //选定指定棋子
        vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(_myturn)*10+pnumber;
        Piece2* ptr = new Piece2();
        ptr = *iter;
        this->select = ptr;
        //计算该棋子的走到的最大权值位置
        len = -100;
        x = ptr->getRow();
        y = ptr->getColumn();
        Opower = cb->AIPower[x][y];//第x行第y列
        isLegalMove(x,y);
        if(!legal.empty())
        {
            vector<int>::iterator it = legal.begin();
            mmax = *it;
            nmax = *(it+1);
            for(vector<int>::iterator it = legal.begin();it<legal.end();it=it+2){
                m1 = *it;
                n1 = *(it+1);
                qDebug()<<"坐标：m1 ="<<m1<<"n1 ="<<n1;
                Npower =cb->AIPower[m1][n1];
                if(Npower-Opower>len){
                    len = Npower-Opower;
                    mmax = m1;
                    nmax = n1;
                }

            }
            qDebug()<<" x="<<x<<" y="<<y<<" mmax="<<mmax<<" nmax="<<nmax;
            getPath(x,y,mmax,nmax,0);
            qDebug()<<cpyNumOfPlaces[_myturn];
            int m,n,m1=-1,n1=-1;
            m=x;
            n=y;
            for(int i=0;i<cpyNumOfPlaces[_myturn]-1;)
            {
                qDebug()<<"ai change info";
                if(m1>0&&n1>0)
                {
                    m=m1;
                    n=n1;
                }
                n1=cpyPlaces[_myturn][i];
                m1=cpyPlaces[_myturn][i+1];
                changeInfo(m,n,m1,n1);
                i+=2;
            }
            QString data1,data2;
            char tmpData1;
            tmpData1=changePlayerId2(me.num);
            data1=QString("%1").arg(tmpData1);
            data2=data2+QString("%1 ").arg(y-8);
            data2=data2+QString("%1 ").arg(x-8);
            for(int i=0;i<numOfPlaces;i++)
            {
                QString tmpStr;
                tmpStr=QString("%1 ").arg(places[i]-8);
                data2=data2+tmpStr;
            }
            showPlaces(control);
            data2=data2.trimmed();
            OPCODE tmpOp=OPCODE::MOVE_OP;
            NetworkData newData(tmpOp,data1,data2);
            socket->send(newData);
            qDebug()<<data2;
        }
        else
        {
            qDebug()<<"err in pnum";
        }
    }

}
int gameplay::AIJudge(int _myturn)//判断移动哪个棋子
{
    //初始化score作为判断标准
    int score[10];
    for(int i=0;i<10;i++){
        score[i]=0;
    }
    //初始化各个数值
    int x=0;
    int y=0;
    int m1=0;
    int n1=0;
    int mmax=0;
    int nmax=0;
    int number = 0;
    int Opower = 0;
    int Npower = 0;
    int len = 0;
    Olongjump.clear();

    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(_myturn)*10;iter !=this->vectorPieces.end()-getNumAfter(_myturn)*10;iter++)
    {
        //遍历全部棋子
        Piece2* ptr=new Piece2();
        ptr=*iter;
        len = -100;
        x = ptr->getRow();
        y = ptr->getColumn();
        //计算当前快要完成的棋子个数
        if(_myturn==0&&(beginNum==3||beginNum==2))
        {
            if (x<6)
                number++;
        }
        else if(_myturn==1&&beginNum==3)
        {
            if (y<6)
                number++;
        }
        else if(_myturn==1&&beginNum==2)
        {
            if (x>11)
                number++;
        }
        else if(_myturn==2&&beginNum==3)
        {
            if (x+y>19)
                number++;
        }
        else

        {            qDebug()<<"_myturn is not correct"<<_myturn;}

        Opower = cb->AIPower[x][y];
        isLegalMove(x,y);
        for(vector<int>::iterator it = legal.begin();it<legal.end();it=it+2){
            //遍历每一个可以到达的点
            m1 = *it;
            n1 = *(it+1);
            Npower =cb->AIPower[m1][n1];
            if(Npower-Opower>len){
                //记录最大权值的位置
                len = Npower-Opower;
                mmax = m1;
                nmax = n1;
            }
        }
        //如果快完成的棋子多了，就增加落后棋子的权重，保证整体性

        if(_myturn==0&&(beginNum==3||beginNum==2))
        {
            if(x>6)
            {
                if(x>10)
                {
                    if(x>13)
                    {
                        len = len+80*range;
                    }
                    else
                    {
                        len = len+50*range;
                    }
                }
                else
                {
                    len = len+5*range;
                }
            }
        }
        else if(_myturn==1&&beginNum==3)
        {

            if(y>6)
            {
                if(y>10)
                {
                    if(y>13)
                    {
                        len = len+80*range;
                    }
                    else
                    {
                        len = len+50*range;
                    }
                }
                else{
                    len = len+5*range;
                }
            }
        }
        else if(_myturn==1&&beginNum==2)
        {
            if(x<10)
            {
                if(x<6)
                {
                    if(x<3)
                    {
                        len = len+80*range;
                    }
                    else
                    {
                        len = len+50*range;
                    }
                }
                else
                {
                    len = len+5*range;
                }
            }
        }
        else if(_myturn==2&&beginNum==3)
        {
            if(x+y<18)
            {
                if(x+y<14)
                {
                    if(x+y<11)
                    {
                        len = len+80*range;
                    }
                    else
                    {
                        len = len+50*range;
                    }
                }
                else
                {
                    len = len+5*range;
                }
            }
        }
        else
        {
            qDebug()<<"_myturn is not correct";
        }
        switch (beginNum)//【两人和三人时区分humanjump】
        {
        qDebug()<<"enter switch beginNum";
        case 2:
        {

            if(len>=0)
            {
                Olongjump.push_back(len);
                //假设走到了最大权值的位置
                changeInfo(x,y,mmax,nmax);
                //计算假设的下一步玩家能走的最大权值
                //下一步AI能走的最大权值
                humanJump(0,1);
                longJump();
                //交换回来
                changeInfo(mmax,nmax,x,y);
            }
            else
            {
                Olongjump.push_back(-1);
                opjump.push_back(-1);
                Oopjump.push_back(-1);
            }
            break;
        }
        case 3:
        {
            if(len>=0)
            {
                Olongjump.push_back(len);
                //假设走到了最大权值的位置
                changeInfo(x,y,mmax,nmax);
                //计算假设的下一步玩家能走的最大权值
                //下一步AI能走的最大权值

                for(int i=0;i<2;i++)
                {
                    if(thePlayers[i].isReady)
                    {
                        humanJump(i,thePlayers[i].num);
                    }
                }
                longJump();
                //交换回来
                changeInfo(mmax,nmax,x,y);
            }
            else
            {
                Olongjump.push_back(-1);
                opjump.push_back(-1);
                Oopjump.push_back(-1);
            }
            break;
        }
        default:
        {
            qDebug()<<"_myturn is not correct";
        }
            qDebug()<<"finish switch (getTotalNum())";
        }

    }
    range = number;
    len = 0;
    int p = 0;

    if(range>5&&range<8){//棋子在中段
        for(vector<int>::iterator it = this->longjump.begin();it !=this->longjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p]+ (*it)*fornest;//搭桥
            }
            p++;
        }
        p=0;
        for(vector<int>::iterator it = this->Olongjump.begin();it !=this->Olongjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p] + (*it)*attck;//进攻
            }
            p++;
        }
        p=0;
    }
    if(range==0||range>=8){

        for(vector<int>::iterator it = this->Olongjump.begin();it !=this->Olongjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p] + (*it)*1;
            }
            p++;
        }
        p=0;
    }
    else{
        for(vector<int>::iterator it = this->longjump.begin();it !=this->longjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p]+(*it)*fornest;
            }
            p++;
        }
        p=0;
        for(vector<int>::iterator it = this->Olongjump.begin();it !=this->Olongjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p] + (*it)*attck;
            }
            p++;
        }
        p=0;
        for(vector<int>::iterator it = this->opjump.begin();it !=this->opjump.end();it++)
        {
            if(*it>=0)
            {
                score[p] = score[p] - (*it)*defence;
            }
            p++;
        }
        p=0;
        //        for(vector<int>::iterator it = this->Oopjump.begin();it !=this->Oopjump.end();it++)
        //        {
        //            if(*it>=0)
        //            {
        //                score[p] = score[p] + (*it)*defence;
        //            }
        //            p++;
        //        }
    }
    //计算出最合理的移动棋子

    int maxmum = score[0];
    int fg = 0;
    for(int i=0;i<10;i++){
        qDebug()<<"score["<<i<<"]="<<score[i];
        if(score[i]>maxmum){
            maxmum = score[i];
            fg = i;
        }
    }
    return fg;

}
void gameplay::isLegalMove(int m1, int n1)//n是横坐标，m是纵坐标
{
    legal.clear();//legal里按行，列顺序存
    if (n1 < 16 && cb->chessBoard[m1][n1 + 1].x != 0 && cb->chessBoard[m1][n1 + 1].flag == 0&&cb->AIPower[m1][n1 + 1]>= 0)
    {
        legal.push_back(m1);
        legal.push_back(n1 + 1);
    } //向右挪
    if (n1 > 0 && cb->chessBoard[m1][n1 - 1].x != 0 && cb->chessBoard[m1][n1 - 1].flag == 0&&cb->AIPower[m1][n1 - 1]>= 0)
    {
        legal.push_back(m1);
        legal.push_back(n1 - 1);
    } //向左挪

    if (m1 > 0 && cb->chessBoard[m1 - 1][n1].x != 0 && cb->chessBoard[m1 - 1][n1].flag == 0&&cb->AIPower[m1 - 1][n1]>= 0) // y-1,左下
    {
        legal.push_back(m1 - 1);
        legal.push_back(n1);
    }
    if (m1 > 0 && n1 < 16 && cb->chessBoard[m1 - 1][n1 + 1].x != 0 && cb->chessBoard[m1 - 1][n1 + 1].flag == 0&&cb->AIPower[m1 - 1][n1 + 1]>= 0) // y-1,x+1,右下
    {
        legal.push_back(m1 - 1);
        legal.push_back(n1 + 1);
    }

    if (m1 < 16 && cb->chessBoard[m1 + 1][n1].x != 0 && cb->chessBoard[m1 + 1][n1].flag == 0&&cb->AIPower[m1 + 1][n1]>= 0) // y+1,右上
    {
        legal.push_back(m1 + 1);
        legal.push_back(n1);
    }
    if (m1 < 16 && n1 > 0 && cb->chessBoard[m1 + 1][n1 - 1].x != 0 && cb->chessBoard[m1 + 1][n1 - 1].flag == 0&&cb->AIPower[m1 + 1][n1 - 1]>= 0) // y+1,x-1,左上
    {
        legal.push_back(m1 + 1);
        legal.push_back(n1 - 1);
    }
    for (int i = 0; i < 17; i++)
        for (int j = 0; j < 17; j++)
            cb->chessBoard[i][j].jumpFlag = 0;
    isLegalJump(m1, n1);
}
void gameplay::isLegalJump(int m1,int n1)
{
    if(n1<12&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==1&&cb->AIPower[m1][n1+1]>= 0)//向右挪，如果右面有子且在棋盘内
    {
        if(n1<11&&cb->chessBoard[m1][n1+2].x!=0&&cb->chessBoard[m1][n1+2].flag==0&&cb->chessBoard[m1][n1+2].jumpFlag!=1&&cb->AIPower[m1][n1+2]>= 0)//如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {
            legal.push_back(m1);
            legal.push_back(n1+2);
            cb->chessBoard[m1][n1+2].jumpFlag=1;
            isLegalJump(m1,n1+2);
        }
    }
    if(n1>0&&cb->chessBoard[m1][n1-1].x!=0&&cb->chessBoard[m1][n1-1].flag==1&&cb->AIPower[m1][n1-1]>= 0)
    {
        if(n1>1&&cb->chessBoard[m1][n1-2].x!=0&&cb->chessBoard[m1][n1-2].flag==0&&cb->chessBoard[m1][n1-2].jumpFlag!=1&&cb->AIPower[m1][n1-2]>= 0)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            legal.push_back(m1);
            legal.push_back(n1-2);
            cb->chessBoard[m1][n1-2].jumpFlag=1;
            isLegalJump(m1,n1-2);
        }
    }//向左挪

    if (m1 < 16 && cb->chessBoard[m1 + 1][n1].x != 0 && cb->chessBoard[m1 + 1][n1].flag == 1&&cb->AIPower[m1 + 1][n1]>= 0) //右上
    {
        if (m1 < 15 && cb->chessBoard[m1 + 2][n1].x != 0 && cb->chessBoard[m1 + 2][n1].flag == 0&&cb->chessBoard[m1+2][n1].jumpFlag!=1&&cb->AIPower[m1+2][n1]>= 0)
        {
            legal.push_back(m1 + 2);
            legal.push_back(n1);
            cb->chessBoard[m1 + 2][n1].jumpFlag=1;
            isLegalJump(m1 + 2,n1);
        }
    }
    if (m1 > 0 && n1 < 16 && cb->chessBoard[m1 - 1][n1 + 1].x != 0 && cb->chessBoard[m1 - 1][n1 + 1].flag == 1&&cb->AIPower[m1 - 1][n1 + 1]>= 0) //右下
    {
        if (m1 > 1 && n1 < 15 && cb->chessBoard[m1 - 2][n1 + 2].x != 0 && cb->chessBoard[m1 - 2][n1 + 2].flag == 0&&cb->chessBoard[m1-2][n1+2].jumpFlag!=1&&cb->AIPower[m1-2][n1+2]>= 0)
        {
            legal.push_back(m1 - 2);
            legal.push_back(n1 + 2);
            cb->chessBoard[m1 - 2][n1 + 2].jumpFlag=1;
            isLegalJump(m1 - 2,n1 + 2);
        }
    }

    if (m1 < 16 && n1 > 0 && cb->chessBoard[m1 + 1][n1 - 1].x != 0 && cb->chessBoard[m1 + 1][n1 - 1].flag == 1&&cb->AIPower[m1 + 1][n1 - 1]>= 0) //左上
    {
        if (m1 < 15 && n1 > 1 && cb->chessBoard[m1 + 2][n1 - 2].x != 0 && cb->chessBoard[m1 + 2][n1 - 2].flag == 0&&cb->chessBoard[m1+2][n1-2].jumpFlag!=1&&cb->AIPower[m1 + 2][n1 - 2]>= 0)
        {
            legal.push_back(m1 + 2);
            legal.push_back(n1 - 2);
            cb->chessBoard[m1 + 2][n1 - 2].jumpFlag=1;
            isLegalJump(m1 + 2,n1 - 2);
        }
    }
    if (m1 > 0 && cb->chessBoard[m1 - 1][n1].x != 0 && cb->chessBoard[m1 - 1][n1].flag == 1&&cb->AIPower[m1 - 1][n1]>= 0) //左下
    {
        if (m1 > 1 && cb->chessBoard[m1 - 2][n1].x != 0 && cb->chessBoard[m1 - 2][n1].flag == 0&&cb->chessBoard[m1-2][n1].jumpFlag!=1&&cb->AIPower[m1-2][n1]>= 0) //如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            legal.push_back(m1 - 2);
            legal.push_back(n1);
            cb->chessBoard[m1 - 2][n1].jumpFlag=1;
            isLegalJump(m1 - 2,n1);
        }
    }

}
void gameplay::getPath(int m1,int n1,int m2,int n2,int _num)
{
    bool tmpflag=true;
    if (n1 < 16 && cb->chessBoard[m1][n1 + 1].flag == 0&&cb->AIPower[m1][n1+1]>= 0&&tmpflag==true)
    {
        if(m1==m2&&n1 + 1==n2)
        {
            _num=0;
            places[_num]=n1 + 1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1;
            _num++;
            cb->chessBoard[m1][n1 + 1].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    } //向右挪
    if (n1 > 0  && cb->chessBoard[m1][n1 - 1].flag == 0&&cb->AIPower[m1][n1-1]>= 0&&tmpflag==true)
    {
        if(m1==m2&&n1 - 1==n2)
        {
            _num=0;
            places[_num]=n1 - 1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1;
            _num++;
            cb->chessBoard[m1][n1 - 1].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    } //向左挪
    if (m1 > 0  && cb->chessBoard[m1 - 1][n1].flag == 0&&cb->AIPower[m1-1][n1]>= 0&&tmpflag==true) // y-1,左下
    {
        if(m1-1==m2&&n1==n2)
        {
            _num=0;
            places[_num]=n1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1-1;
            _num++;
            cb->chessBoard[m1-1][n1 ].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    }
    if (m1 > 0 && n1 < 16&& cb->chessBoard[m1 - 1][n1 + 1].flag == 0&&cb->AIPower[m1-1][n1+1]>= 0&&tmpflag==true) // y-1,x+1,右下
    {
        if(m1-1==m2&&n1+1==n2)
        {
            _num=0;
            places[_num]=n1+1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1-1;
            _num++;
            cb->chessBoard[m1-1][n1 +1].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    }
    if (m1 < 16  && cb->chessBoard[m1 + 1][n1].flag == 0&&cb->AIPower[m1+1][n1]>= 0&&tmpflag==true) // y+1,右上
    {
        if(m1+1==m2&&n1==n2)
        {
            _num=0;
            places[_num]=n1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1+1;
            _num++;
            cb->chessBoard[m1+1][n1].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    }
    if (m1 < 16  && cb->chessBoard[m1 + 1][n1 - 1].flag == 0&&cb->AIPower[m1+1][n1-1]>= 0&&tmpflag==true) // y+1,x-1,左上
    {
        if(m1+1==m2&&n1 - 1==n2)
        {
            _num=0;
            places[_num]=n1-1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1+1;
            _num++;
            cb->chessBoard[m1+1][n1-1].jumpFlag=1;
            numOfPlaces=_num;
            tmpflag=false;
        }
    }
    if(tmpflag==true)
    {
        for (int i = 0; i < 17; i++)
            for (int j = 0; j < 17; j++)
                cb->chessBoard[i][j].jumpFlag = 0;
        if(!getJumpPath(m1,n1,m2,n2,0))
        {
            qDebug()<<"【wrong path!!!!!!!】";
        }
    }
    cpyNumOfPlaces[myTurn]=numOfPlaces;
    for(int i=0;i<numOfPlaces;i++)
    {
        cpyPlaces[myTurn][i]=places[i];
    }
}
bool gameplay::getJumpPath(int m1,int n1,int m2,int n2,int _num)
{
    qDebug()<<"getjumppath"<<m1<<" "<<n1<< " "<<m2<<" "<<n2;
    bool tmpFlag=true;
    int recNum=_num;
    if(n1<12&&cb->chessBoard[m1][n1+1].flag==1&&cb->AIPower[m1][n1+1]>= 0&&tmpFlag==true)//向右挪，如果右面有子且在棋盘内
    {
        if(n1<11&&cb->chessBoard[m1][n1+2].flag==0&&cb->chessBoard[m1][n1+2].jumpFlag!=1&&cb->AIPower[m1][n1+2]>= 0)//如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1+2;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1;
            _num++;
            cb->chessBoard[m1][n1+2].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1)==m2&&(n1+2)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1,n1+2,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }
    if(n1>0&&cb->chessBoard[m1][n1-1].flag==1&&cb->AIPower[m1][n1-1]>= 0&&tmpFlag==true)
    {
        if(n1>1&&cb->chessBoard[m1][n1-2].flag==0&&cb->chessBoard[m1][n1-2].jumpFlag!=1&&cb->AIPower[m1][n1-2]>= 0)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1-2;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1;
            _num++;
            cb->chessBoard[m1][n1-2].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1)==m2&&(n1-2)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1,n1-2,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }//向左挪

    if (m1 < 16 && cb->chessBoard[m1 + 1][n1].flag == 1&&cb->AIPower[m1+1][n1]>= 0&&tmpFlag==true) //右上
    {
        if (m1 < 15 && cb->chessBoard[m1 + 2][n1].flag == 0&&cb->chessBoard[m1+2][n1].jumpFlag!=1&&cb->AIPower[m1+2][n1]>= 0)
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1+2;
            _num++;
            cb->chessBoard[m1+2][n1].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1+2)==m2&&(n1)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1+2,n1,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }
    if (m1 > 0  && cb->chessBoard[m1 - 1][n1 + 1].flag == 1&&cb->AIPower[m1-1][n1+1]>= 0&&tmpFlag==true)//右下
    {
        if (m1 > 1&& cb->chessBoard[m1 - 2][n1 + 2].flag == 0&&cb->chessBoard[m1-2][n1+2].jumpFlag!=1&&cb->AIPower[m1-2][n1+2]>= 0)
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1+2;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1-2;
            _num++;
            cb->chessBoard[m1-2][n1+2].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1-2)==m2&&(n1+2)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1-2,n1+2,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    if (m1 < 16  && cb->chessBoard[m1 + 1][n1 - 1].flag == 1&&cb->AIPower[m1+1][n1-1]>= 0&&tmpFlag==true) //左上
    {
        if (m1 < 15 && cb->chessBoard[m1 + 2][n1 - 2].flag == 0&&cb->chessBoard[m1+2][n1-2].jumpFlag!=1&&cb->AIPower[m1+2][n1-2]>= 0)
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1-2;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1+2;
            _num++;
            cb->chessBoard[m1+2][n1-2].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1+2)==m2&&(n1-2)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1+2,n1-2,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }
    if (m1 > 0&& cb->chessBoard[m1 - 1][n1].flag == 1&&cb->AIPower[m1-1][n1]>= 0&&tmpFlag==true) //左下
    {
        if (m1 > 1 && cb->chessBoard[m1 - 2][n1].flag == 0&&cb->chessBoard[m1-2][n1].jumpFlag!=1&&cb->AIPower[m1-2][n1]>= 0) //如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {
            _num=recNum;
            tmpFlag=false;
            places[_num]=n1;//n是横坐标，m是纵坐标
            _num++;
            places[_num]=m1-2;
            _num++;
            cb->chessBoard[m1-2][n1].jumpFlag=1;
            numOfPlaces=_num;
            if(!((m1-2)==m2&&(n1)==n2))
            {
                tmpFlag=true;
                if(getJumpPath(m1-2,n1,m2,n2,_num))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}
void gameplay::longJump(){
    //初始化各个数值
    int x=0;
    int y=0;
    int m1=0;
    int n1=0;
    int Opower = 0;
    int Npower = 0;
    longjump.clear();
    int len = 0;
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(myTurn)*10;iter !=this->vectorPieces.end()-getNumAfter(myTurn)*10;iter++)
    {
        //遍历全部棋子
        Piece2* ptr=new Piece2();
        ptr=*iter;
        len = 0;
        x = ptr->getRow();
        y = ptr->getColumn();
        Opower = cb->AIPower[x][y];
        isLegalMove(x,y);
        for(vector<int>::iterator it = legal.begin();it<legal.end();it=it+2){
            //遍历每一个可以到达的点
            m1 = *it;
            n1 = *(it+1);
            Npower =cb->AIPower[m1][n1];

            if(Npower-Opower>len){
                len = Npower-Opower;
            }
        }
        //将该棋子的最大权值进行记录
        longjump.push_back(len);

    }

}
//玩家下一步每一个棋子能到达的最大权值
void gameplay::humanJump(int _i, int _num){
    //初始化各个数值
    int x=0;
    int y=0;
    int m1=0;
    int n1=0;
    int Opower = 0;
    int Npower = 0;
    opjump.clear();
    int len = 0;
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(_num)*10;iter !=this->vectorPieces.end()-getNumAfter(_num)*10;iter++)
    {
        //遍历全部棋子
        Piece2* ptr=new Piece2();
        ptr=*iter;
        len = 0;
        x = ptr->getRow();
        y = ptr->getColumn();
        Opower = cb->humanPower[_i][x][y];
        isLegalMove(x,y);
        for(vector<int>::iterator it = legal.begin();it<legal.end();it=it+2){
            //遍历每一个可以到达的点
            m1 = *it;
            n1 = *(it+1);
            Npower =cb->humanPower[_i][m1][n1];

            if(Npower-Opower>len){
                len = Npower-Opower;
            }
        }
        //将该棋子的最大权值进行记录
        opjump.push_back(len);

    }

}
//玩家当前每一个棋子能到达的最大权值
void gameplay::OhumanJump(int _i, int _num){
    //初始化各个数值
    int x=0;
    int y=0;
    int m1=0;
    int n1=0;
    int Opower = 0;
    int Npower = 0;
    Oopjump.clear();
    int len = 0;
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+getNumBefore(_num)*10;iter !=this->vectorPieces.end()-getNumAfter(_num)*10;iter++)
    {
        //遍历全部棋子
        Piece2* ptr=new Piece2();
        ptr=*iter;
        len = 0;
        x = ptr->getRow();
        y = ptr->getColumn();
        Opower = cb->humanPower[_i][x][y];
        isLegalMove(x,y);
        for(vector<int>::iterator it = legal.begin();it<legal.end();it=it+2){
            //遍历每一个可以到达的点
            m1 = *it;
            n1 = *(it+1);
            Npower =cb->humanPower[_i][m1][n1];

            if(Npower-Opower>len){
                len = Npower-Opower;
            }
        }
        //将该棋子的最大权值进行记录
        Oopjump.push_back(len);
    }
}
