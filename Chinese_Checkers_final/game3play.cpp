#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <vector>
#include <QDebug>
#include "game3play.h"
#include "ui_game3play.h"
#include "board.h"
#include "mainwindow.h"

using namespace std;

game3play::game3play(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::game3play)
{
    ui->setupUi(this);
    this->control = 0;
    this->select = NULL;
    this->producePiece3Player();
    this->produceRectF();
    cb = new Board();
    cb->initial();
    lightColor=0;
    numOfWinner=0;
    sameRank=0;
    //为每一个坐标添加正方形点击区域
    QRectF* qr;
    int k = 0;
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<13;j++)
        {
            qr=this->vectorRectF.at(k);
            k++;
            qr->setRect(this->cb->chessBoard[i][j].x,this->cb->chessBoard[i][j].y,23,23);
            //setRect是设置一个矩形区域
        }
    }
    //设置玩家棋子
    this->setPiece0();
    this->setPiece2();
    this->setPiece3();
    //为每一个棋子（按钮）的点击设置槽函数
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-20;iter++)
        //iterator 行为类似于指针，用于遍历整个容器
    {
        //Piece2* ptr = *iter;
        connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
        //connet(指向棋子的指针，棋子里证明被点到的函数，本类：game3play,game3play里的处理点击的函数)
        //begin是vector的开始，有60个棋子
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;iter !=this->vectorPieces.end()-10;iter++)
    {
        //Piece2* ptr = *iter;
        connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick2(int,int)));
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+20;iter !=this->vectorPieces.end();iter++)
    {
        //Piece2* ptr = *iter;
        connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick3(int,int)));
    }



}
game3play::~game3play()
{
    delete ui;
}
void game3play::on_pushButton_clicked()//返回键
{
    this->hide();
    MainWindow* w = new MainWindow();
    w->show();
}
void game3play::mousePressEvent(QMouseEvent* event)//QMouseEvent传入用于描述鼠标事件的参数
{
    qDebug()<<"enter mousePressEvent";
    if(event->button()==Qt::LeftButton)
    {
        for(vector<QRectF*>::iterator iter = this->vectorRectF.begin();iter != this->vectorRectF.end();iter++)
        {
            QRectF* ptr =*iter;
            if(ptr->contains( mapFromGlobal(QCursor::pos()))==true)
            {
                if(this->select!=NULL)
                {
                    if(this->control==0){
                        if(isLegalMove(this->select->x(),this->select->y(),ptr->x(),ptr->y()))
                        {
                            qDebug()<<"enter control==0";
                            //set *s=new set();
                            changeInfo(this->select->x(),this->select->y(),ptr->x(),ptr->y());
                            //differences
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess.png"));
                            this->select=NULL;
                            //judgewin0();
                            control = 2;
                        }
                        else{
                            qDebug()<<"enter control==0****";
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess.png"));
                            this->select=NULL;
                        }
                    }

                    else if(this->control==2){

                        if(isLegalMove(this->select->x(),this->select->y(),ptr->x(),ptr->y()))
                        {
                            qDebug()<<"enter control==2";
                            //set *s=new set();
                            changeInfo(this->select->x(),this->select->y(),ptr->x(),ptr->y());
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess3.png"));
                            this->select=NULL;
                            //judgewin2();
                            control = 3;
                        }
                        else{
                            qDebug()<<"enter control==2****";
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess3.png"));
                            this->select=NULL;
                        }
                    }
                    else if(this->control==3){

                        if(isLegalMove(this->select->x(),this->select->y(),ptr->x(),ptr->y()))
                        {
                            qDebug()<<"enter control==3";
                            //set *s=new set();
                            changeInfo(this->select->x(),this->select->y(),ptr->x(),ptr->y());
                            //differences
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess4.png"));
                            this->select=NULL;
                            //judgewin3();
                            control = 0;
                        }
                        else{
                            qDebug()<<"enter control==3****";
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess4.png"));
                            this->select=NULL;
                        }
                    }


                }

            }
        }
    }
}
void game3play::produceRectF()//创建棋盘可点击区域链表//right
{

    for(int i=0;i<17;i++)
    {
        for(int j=0;j<13;j++)
        {
            QRectF* rect=new QRectF();//QRectF：一个矩形区域
            this->vectorRectF.push_back(rect);//pushback是在容器最后加一个
        }
    }
}
void  game3play::producePiece3Player()//right
{
    //生成玩家方的棋子
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
    for(vector<Piece2*>::iterator iter = vectorPieces.begin()+10;iter != vectorPieces.end();iter++)
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
    for(vector<Piece2*>::iterator iter = vectorPieces.begin()+20;iter != vectorPieces.end();iter++)
    {
        Piece2* p = *iter;
        p->setStatus(3);                //玩家为3
        p->setIcon(QIcon(":/res/chess4.png"));
        p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
        p->setFlat(true);               //去掉原来pushbutton的背景

        p->setParent(this);             //加入界面
    }
}

void game3play::setPiece0()
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin();
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-20;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }

}
void game3play::setPiece2()
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;
    int begin=13;//小三角形中轴线
    for(int i=7;i>3;i--)
    {
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;iter !=this->vectorPieces.end()-10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void game3play::setPiece3()
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+20;
    int begin=4;//小三角形中轴线
    for(int i=7;i>3;i--)
    {
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+20;iter !=this->vectorPieces.end();iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}

void game3play::processClick0(int m,int n)
{
    qDebug()<<"enter processClick0***********************************************";
    qDebug()<<"control="<<control;
    //处理鼠标点击事件
    if(control==0){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chesss.png"));
        }
        else{

            this->lightColor=0;
            this->select->setIcon(QIcon(":/res/chess.png"));
            this->select=NULL;
        }
    }

}
void game3play::processClick2(int m,int n)
{
    qDebug()<<"enter processClick2***********************************";
    qDebug()<<"control="<<control;
    //处理鼠标点击事件
    if(control==2){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess3s.png"));
            qDebug()<<"select=cb->chessBoard[m][n].piecePlace="<<cb->chessBoard[m][n].piecePlace;
        }
        else{

            this->lightColor=0;
            this->select->setIcon(QIcon(":/res/chess3.png"));
            this->select=NULL;
        }
    }

}
void game3play::processClick3(int m,int n)
{
    qDebug()<<"enter processClick3****************************";
    qDebug()<<"control="<<control;
    //处理鼠标点击事件
    if(control==3){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess4s.png"));
        }
        else{

            this->lightColor=0;
            this->select->setIcon(QIcon(":/res/chess4.png"));
            this->select=NULL;
        }
    }

}


void game3play::changeInfo(int x1,int y1,int x2,int y2)
{
    qDebug()<<"-------------------------------enter changeinfo-----------------------------------";
    qDebug()<<x1<<"  "<<y1<<"  "<<x2<<"  "<<y2;
    int m1=0;
    int n1=0;
    int n2=0;
    int m2=0;
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            if(cb->chessBoard[i][j].x==x1 && cb->chessBoard[i][j].y==y1){
                m1=i;
                n1=j;
            }
        }
    }//遍历棋盘找到原来的位置
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            if(cb->chessBoard[i][j].x==x2&&cb->chessBoard[i][j].y==y2){
                m2=i;
                n2=j;
            }
        }
    }//遍历棋盘找到现在的位置
    qDebug()<<m1<<"  "<<n1<<"  "<<m2<<"  "<<n2;
    qDebug()<<cb->chessBoard[m1][n1].flag;
    this->cb->chessBoard[m1][n1].flag=0;
    qDebug()<<"cb->chessBoard[m1][n1].piecePlace="<<cb->chessBoard[m1][n1].piecePlace;
    this->cb->chessBoard[m2][n2].piecePlace=this->cb->chessBoard[m1][n1].piecePlace;//把指针从原来的换到现在的
    qDebug()<<"cb->chessBoard[m2][n2].piecePlace="<<cb->chessBoard[m2][n2].piecePlace;
    this->cb->chessBoard[m1][n1].piecePlace=NULL;
    this->cb->chessBoard[m2][n2].flag=1;
    this->cb->chessBoard[m2][n2].piecePlace->setPlace(m2,n2);
    this->select->move(cb->chessBoard[m2][n2].x,cb->chessBoard[m2][n2].y);
}
bool game3play::isLegalMove(int x1,int y1,int x2,int y2)
{
    /*  isLegal.clear();
    int m1=0;
    int n1=0;
    int m2=0;
    int n2=0;
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<13;j++)
        {
            if(cb->chessBoard[i][j].x==x1 && cb->chessBoard[i][j].y==y1){
                m1=i;
                n1=j;
            }
        }
    }
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<13;j++)
        {
            if(cb->chessBoard[i][j].x==x2&&cb->chessBoard[i][j].y==y2){
                m2=i;
                n2=j;
            }
        }
    }//找到原来位置和现在位置
    if(n1<12&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1+1);
    }//向右挪
    if(n1>0&&cb->chessBoard[m1][n1-1].x!=0&&cb->chessBoard[m1][n1-1].flag==0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1-1);
    }//向左挪
    if(m1%2){

        if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==0){
            isLegal.push_back(m1-1);
            isLegal.push_back(n1);

        }
        if(m1>0&&n1<12&&cb->chessBoard[m1-1][n1+1].x!=0&&cb->chessBoard[m1-1][n1+1].flag==0){
            isLegal.push_back(m1-1);
            isLegal.push_back(n1+1);

        }
        if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==0){
            isLegal.push_back(m1+1);
            isLegal.push_back(n1);
        }
        if(m1<16&&n1<12&&cb->chessBoard[m1+1][n1+1].x!=0&&cb->chessBoard[m1+1][n1+1].flag==0){
            isLegal.push_back(m1+1);
            isLegal.push_back(n1+1);
        }
    }
    else{

        if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==0){
            isLegal.push_back(m1-1);
            isLegal.push_back(n1);
        }
        if(m1>0&&n1>0&&cb->chessBoard[m1-1][n1-1].x!=0&&cb->chessBoard[m1-1][n1-1].flag==0){
            isLegal.push_back(m1-1);
            isLegal.push_back(n1-1);

        }

        if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==0){
            isLegal.push_back(m1+1);
            isLegal.push_back(n1);
        }
        if(m1<16&&n1>0&&cb->chessBoard[m1+1][n1-1].x!=0&&cb->chessBoard[m1+1][n1-1].flag==0){
            isLegal.push_back(m1+1);
            isLegal.push_back(n1-1);
        }
    }*/
   qDebug()<<"enter isLegalMove";
    int m1=0;
    int n1=0;
    int m2=0;
    int n2=0;
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            if(cb->chessBoard[i][j].x==x1 && cb->chessBoard[i][j].y==y1){
                m1=i;
                n1=j;
            }
        }
    }
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<17;j++)
        {
            if(cb->chessBoard[i][j].x==x2&&cb->chessBoard[i][j].y==y2){
                m2=i;
                n2=j;
            }
        }
    }//找到原来位置和现在位置
    isLegal.clear();
    if(n1<16&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1+1);
    }//向右挪
    if(n1>0&&cb->chessBoard[m1][n1-1].x!=0&&cb->chessBoard[m1][n1-1].flag==0)
    {
        isLegal.push_back(m1);
        isLegal.push_back(n1-1);
    }//向左挪

    if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==0)//y-1,左下
    {
        isLegal.push_back(m1-1);
        isLegal.push_back(n1);
    }
    if(m1>0&&n1<16&&cb->chessBoard[m1-1][n1+1].x!=0&&cb->chessBoard[m1-1][n1+1].flag==0)//y-1,x+1,右下
    {
        isLegal.push_back(m1-1);
        isLegal.push_back(n1+1);

    }

    if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==0)//y+1,右上
    {
        isLegal.push_back(m1+1);
        isLegal.push_back(n1);
    }
    if(m1<16&&n1>0&&cb->chessBoard[m1+1][n1-1].x!=0&&cb->chessBoard[m1+1][n1-1].flag==0)//y+1,x-1,左上
    {
        isLegal.push_back(m1+1);
        isLegal.push_back(n1-1);
    }
    //    }
    for(int i=0;i<17;i++)
        for(int j=0;j<17;j++)
            cb->chessBoard[i][j].jumpFlag=0;
    qDebug()<<"enter isLegalJump";
    isLegalJump(m1,n1,m2,n2);
    for(vector<int>::iterator it = isLegal.begin();it!=isLegal.end();it=it+2){

        if(m2 == *it && n2 == *(it+1)){
            return true;
        }
    }

    return false;

}
void game3play::isLegalJump(int m1,int n1,int m2,int n2)
{
    if(n1<16&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==1)//向右挪，如果右面有子且在棋盘内
    {
        //cb->chessBoard[m1][n1+1].jumpFlag=1;//记右边有子一格jumpFlag为1，避免重复路径
        if(n1<11&&cb->chessBoard[m1][n1+2].x!=0&&cb->chessBoard[m1][n1+2].flag==0&&cb->chessBoard[m1][n1+2].jumpFlag!=1)//如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {
            if(m1==m2&&n1+2==n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1+2);
                cb->chessBoard[m1][n1+2].jumpFlag=1;
            }
            else{
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                isLegal.push_back(m1);
                isLegal.push_back(n1+2);
                cb->chessBoard[m1][n1+2].jumpFlag=1;
                isLegalJump(m1,n1+2,m2,n2);
            }
        }
    }
    if(n1>0&&cb->chessBoard[m1][n1-1].x!=0&&cb->chessBoard[m1][n1-1].flag==1)
    {
        if(n1>1&&cb->chessBoard[m1][n1-2].x!=0&&cb->chessBoard[m1][n1-2].flag==0&&cb->chessBoard[m1][n1-2].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {if(m1==m2&&n1-2==n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1-2);
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1-2].jumpFlag=1;
            }
            else{
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1-2].jumpFlag=1;
                isLegalJump(m1,n1-2,m2,n2);
            }

        }
    }//向左挪

    if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==1)//右上
    {
        if(m1<15&&cb->chessBoard[m1+2][n1].x!=0&&cb->chessBoard[m1+2][n1].flag==0&&cb->chessBoard[m1+2][n1].jumpFlag!=1)
        {if(m1+2==m2&&n1==n2)
            {
                isLegal.push_back(m1+2);
                isLegal.push_back(n1);
                cb->chessBoard[m1+2][n1].jumpFlag=1;
            }
            else{
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1+2][n1].jumpFlag=1;
                isLegalJump(m1+2,n1,m2,n2);
            }

        }
    }
    if(m1>0&&n1<16&&cb->chessBoard[m1-1][n1+1].x!=0&&cb->chessBoard[m1-1][n1+1].flag==1)//右下
    {
        if(m1>1&&n1<15&&cb->chessBoard[m1-2][n1+2].x!=0&&cb->chessBoard[m1-2][n1+2].flag==0&&cb->chessBoard[m1-2][n1+2].jumpFlag!=1)
        {if(m1-2==m2&&n1+2==n2)
            {
                isLegal.push_back(m1-2);
                isLegal.push_back(n1+2);
                cb->chessBoard[m1-2][n1+2].jumpFlag=1;
            }
            else{
                cb->chessBoard[m1-2][n1+2].jumpFlag=1;
                isLegalJump(m1-2,n1+2,m2,n2);
            }

        }

    }

    if(m1<16&&n1>0&&cb->chessBoard[m1+1][n1-1].x!=0&&cb->chessBoard[m1+1][n1-1].flag==1)//左上
    {
        if(m1<15&&n1>1&&cb->chessBoard[m1+2][n1-2].x!=0&&cb->chessBoard[m1+2][n1-2].flag==0&&cb->chessBoard[m1+2][n1-2].jumpFlag!=1)
        {if(m1+2==m2&&n1-2==n2)
            {
                isLegal.push_back(m1+2);
                isLegal.push_back(n1-2);
                cb->chessBoard[m1+2][n1-2].jumpFlag=1;
            }
            else{
                cb->chessBoard[m1+2][n1-2].jumpFlag=1;
                isLegalJump(m1+2,n1-2,m2,n2);
            }

        }
    }
    if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==1)//左下
    {
        if(m1>1&&cb->chessBoard[m1-2][n1].x!=0&&cb->chessBoard[m1-2][n1].flag==0&&cb->chessBoard[m1-2][n1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {if(m1-2==m2&&n1==n2)
            {
                isLegal.push_back(m1-2);
                isLegal.push_back(n1);
                cb->chessBoard[m1-2][n1].jumpFlag=1;
            }
            else{
                cb->chessBoard[m1-2][n1].jumpFlag=1;
                isLegalJump(m1-2,n1,m2,n2);
            }

        }
    }

    /*    if(n1<12&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==1)//向右挪，如果右面有子且在棋盘内
    {
        //cb->chessBoard[m1][n1+1].jumpFlag=1;//记右边有子一格jumpFlag为1，避免重复路径
        if(n1<11&&cb->chessBoard[m1][n1+2].x!=0&&cb->chessBoard[m1][n1+2].flag==0&&cb->chessBoard[m1][n1+2].jumpFlag!=1)//如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {if(m1==m2&&n1+2==n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1+2);
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1+2].jumpFlag=1;
            }
            else{
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1+2].jumpFlag=1;
                isLegalJump(m1,n1+2,m2,n2);
            }

        }
    }
    if(n1>0&&cb->chessBoard[m1][n1-1].x!=0&&cb->chessBoard[m1][n1-1].flag==1)
    {
        if(n1>1&&cb->chessBoard[m1][n1-2].x!=0&&cb->chessBoard[m1][n1-2].flag==0&&cb->chessBoard[m1][n1-2].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
        {if(m1==m2&&n1-2==n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1-2);
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1-2].jumpFlag=1;
            }
            else{
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
                cb->chessBoard[m1][n1-2].jumpFlag=1;
                isLegalJump(m1,n1-2,m2,n2);
            }

        }
    }//向左挪
    if(m1%2){

        if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==1)//左上
        {
            if(n1>0&&m1>1&&cb->chessBoard[m1-2][n1-1].x!=0&&cb->chessBoard[m1-2][n1-1].flag==0&&cb->chessBoard[m1-2][n1-1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1-2==m2&&n1-1==n2)
                {
                    isLegal.push_back(m1-2);
                    isLegal.push_back(n1-1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1-1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1-1].jumpFlag=1;
                    isLegalJump(m1-2,n1-1,m2,n2);
                }

            }
        }
        if(m1>0&&n1<12&&cb->chessBoard[m1-1][n1+1].x!=0&&cb->chessBoard[m1-1][n1+1].flag==1)//右上方
        {
            if(m1>1&&cb->chessBoard[m1-2][n1+1].x!=0&&cb->chessBoard[m1-2][n1+1].flag==0&&cb->chessBoard[m1-2][n1+1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1-2==m2&&n1+1==n2)
                {
                    isLegal.push_back(m1-2);
                    isLegal.push_back(n1+1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1+1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1+1].jumpFlag=1;
                    isLegalJump(m1-2,n1+1,m2,n2);
                }

            }
        }
        if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==1)//左下方
        {
            if(n1>0&&m1<15&&cb->chessBoard[m1+2][n1-1].x!=0&&cb->chessBoard[m1+2][n1-1].flag==0&&cb->chessBoard[m1+2][n1-1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1+2==m2&&n1-1==n2)
                {
                    isLegal.push_back(m1+2);
                    isLegal.push_back(n1-1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1-1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1-1].jumpFlag=1;
                    isLegalJump(m1+2,n1-1,m2,n2);
                }

            }
        }
        if(m1<16&&n1<12&&cb->chessBoard[m1+1][n1+1].x!=0&&cb->chessBoard[m1+1][n1+1].flag==1)//右下
        {
            if(n1<16&&m1<15&&cb->chessBoard[m1+2][n1+1].x!=0&&cb->chessBoard[m1+2][n1+1].flag==0&&cb->chessBoard[m1+2][n1+1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1+2==m2&&n1+1==n2)
                {
                    isLegal.push_back(m1+2);
                    isLegal.push_back(n1+1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1+1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1+1].jumpFlag=1;
                    isLegalJump(m1+2,n1+1,m2,n2);
                }

            }
        }
    }
    else{

        if(m1>0&&cb->chessBoard[m1-1][n1].x!=0&&cb->chessBoard[m1-1][n1].flag==1)//右上
        {
            if(m1>1&&cb->chessBoard[m1-2][n1+1].x!=0&&cb->chessBoard[m1-2][n1+1].flag==0&&cb->chessBoard[m1-2][n1+1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1-2==m2&&n1+1==n2)
                {
                    isLegal.push_back(m1-2);
                    isLegal.push_back(n1+1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1+1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1+1].jumpFlag=1;
                    isLegalJump(m1-2,n1+1,m2,n2);
                }

            }
        }
        if(m1>0&&n1>0&&cb->chessBoard[m1-1][n1-1].x!=0&&cb->chessBoard[m1-1][n1-1].flag==1){
            if(n1>0&&m1>1&&cb->chessBoard[m1-2][n1-1].x!=0&&cb->chessBoard[m1-2][n1-1].flag==0&&cb->chessBoard[m1-2][n1-1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1-2==m2&&n1-1==n2)
                {
                    isLegal.push_back(m1-2);
                    isLegal.push_back(n1-1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1-1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1-2][n1-1].jumpFlag=1;
                    isLegalJump(m1-2,n1-1,m2,n2);
                }

            }

        }

        if(m1<16&&cb->chessBoard[m1+1][n1].x!=0&&cb->chessBoard[m1+1][n1].flag==1){
            if(n1<16&&m1<15&&cb->chessBoard[m1+2][n1+1].x!=0&&cb->chessBoard[m1+2][n1+1].flag==0&&cb->chessBoard[m1+2][n1+1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1+2==m2&&n1+1==n2)
                {
                    isLegal.push_back(m1+2);
                    isLegal.push_back(n1+1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1+1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1+1].jumpFlag=1;
                    isLegalJump(m1+2,n1+1,m2,n2);
                }

            }
        }
        if(m1<16&&n1>0&&cb->chessBoard[m1+1][n1-1].x!=0&&cb->chessBoard[m1+1][n1-1].flag==1){
            if(n1>0&&m1<15&&cb->chessBoard[m1+2][n1-1].x!=0&&cb->chessBoard[m1+2][n1-1].flag==0&&cb->chessBoard[m1+2][n1-1].jumpFlag!=1)//如果左面隔一个的位置是空的，那就是一次合法的跳跃
            {if(m1+2==m2&&n1-1==n2)
                {
                    isLegal.push_back(m1+2);
                    isLegal.push_back(n1-1);
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1-1].jumpFlag=1;
                }
                else{
                    //cb->chessBoard[m1][n1+1].jumpFlag=1;
                    cb->chessBoard[m1+2][n1-1].jumpFlag=1;
                    isLegalJump(m1+2,n1-1,m2,n2);
                }

            }
        }
    }*/
}

