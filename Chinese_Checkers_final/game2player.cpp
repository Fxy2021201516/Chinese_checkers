#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <vector>
#include <QDebug>
#include "game2player.h"
#include "ui_game2player.h"
#include "board.h"
#include "mainwindow.h"
#include"gameover.h"

using namespace std;

game2player::game2player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::game2player)
{
    ui->setupUi(this);
    this->control = 0;
    this->select = NULL;
    // ui->label_7->setText("player1");
    this->producePiece2Player();
    this->produceRectF();
    cb = new Board();
    cb->initial();
    lightColor=0;
    numOfWinner=0;
    sameRank=0;
    i_rank=0;
    i_bye=0;
    for(int i=0;i<2;i++)
    {
        isWin[i]=0;
    }
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
    this->setPiece1();
    //为每一个棋子（按钮）的点击设置槽函数
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-10;iter++)
        //iterator 行为类似于指针，用于遍历整个容器
    {
        //Piece2* ptr = *iter;
        connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick0(int,int)));
        //connet(指向棋子的指针，棋子里证明被点到的函数，本类：game2player,game2player里的处理点击的函数)
        //begin是vector的开始，有60个棋子
    }
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;iter !=this->vectorPieces.end();iter++)
    {
        //Piece2* ptr = *iter;
        connect(*iter,SIGNAL(pieceInfo(int,int)),this,SLOT(processClick1(int,int)));
    }
}
game2player::~game2player()
{
    delete ui;
}
void game2player::on_pushButton_clicked()//返回键
{
    this->hide();
    MainWindow* w = new MainWindow();
    w->show();
}
void game2player::mousePressEvent(QMouseEvent* event)//QMouseEvent传入用于描述鼠标事件的参数
{
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
                            //set *s=new set();
                            changeInfo(this->select->x(),this->select->y(),ptr->x(),ptr->y());
                            //differences
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess.png"));
                            this->select=NULL;
                            judgewin0();
                            for(int add=1;add<3;add++)
                            {
                                if(control+add>1)
                                    sameRank=0;
                                if(!isWin[(control+add)%2])
                                {
                                    control = (control+add)%2;
                                    break;

                                }
                            }
                        }
                        else{
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess.png"));
                            this->select=NULL;
                        }
                    }
                    else if(this->control==1){
                        if(isLegalMove(this->select->x(),this->select->y(),ptr->x(),ptr->y()))
                        {
                            changeInfo(this->select->x(),this->select->y(),ptr->x(),ptr->y());
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess2.png"));
                            this->select=NULL;
                            judgewin1();
                            for(int add=1;add<3;add++)
                            {
                                if(control+add>1)
                                    sameRank=0;
                                if(!isWin[(control+add)%2])
                                {
                                    control = (control+add)%2;
                                    break;

                                }
                            }
                        }

                        else{
                            this->lightColor=0;
                            this->select->setIcon(QIcon(":/res/chess2.png"));
                            this->select=NULL;
                        }
                    }

                }

            }

        }
    }
}

void game2player::produceRectF()//创建棋盘可点击区域链表//right
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
void  game2player::producePiece2Player()//right
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
        p->setStatus(1);                //玩家为1
        p->setIcon(QIcon(":/res/chess2.png"));
        p->setIconSize(QSize(23,23));   //将图片大小设置成button的大小
        p->setFlat(true);               //去掉原来pushbutton的背景

        p->setParent(this);             //加入界面
    }
}
void game2player::setPiece0()
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-10;iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }
}
void game2player::setPiece1()
{
    Piece2* ptr;
    vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;
    int begin=12;//小三角形中轴线
    for(int i=0;i<4;i++)
    {
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
    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;iter !=this->vectorPieces.end();iter++)
    {
        ptr=*iter;
        ptr->setParent(this);
    }

}


void game2player::processClick0(int m,int n)
{
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
void game2player::processClick1(int m,int n)
{
    //处理鼠标点击事件
    if(control==1){
        if(this->lightColor==0)
        {
            this->lightColor=1;
            this->select=this->cb->chessBoard[m][n].piecePlace;
            this->select->setIcon(QIcon(":/res/chess2s.png"));
        }
        else{

            this->lightColor=0;
            this->select->setIcon(QIcon(":/res/chess2.png"));
            this->select=NULL;
        }
    }

}

void game2player::changeInfo(int x1,int y1,int x2,int y2)
{

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
    this->cb->chessBoard[m1][n1].flag=0;
    this->cb->chessBoard[m2][n2].piecePlace=this->cb->chessBoard[m1][n1].piecePlace;//把指针从原来的换到现在的
    this->cb->chessBoard[m1][n1].piecePlace=NULL;
    this->cb->chessBoard[m2][n2].flag=1;
    this->cb->chessBoard[m2][n2].piecePlace->setPlace(m2,n2);
    this->select->move(cb->chessBoard[m2][n2].x,cb->chessBoard[m2][n2].y);
}
bool game2player::isLegalMove(int x1,int y1,int x2,int y2){


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
    isLegalJump(m1,n1,m2,n2);
    for(vector<int>::iterator it = isLegal.begin();it!=isLegal.end();it=it+2){

        if(m2 == *it && n2 == *(it+1)){
            return true;
        }
    }

    return false;
}
void game2player::isLegalJump(int m1,int n1,int m2,int n2)
{
    if(n1<12&&cb->chessBoard[m1][n1+1].x!=0&&cb->chessBoard[m1][n1+1].flag==1)//向右挪，如果右面有子且在棋盘内
    {
        //cb->chessBoard[m1][n1+1].jumpFlag=1;//记右边有子一格jumpFlag为1，避免重复路径
        if(n1<11&&cb->chessBoard[m1][n1+2].x!=0&&cb->chessBoard[m1][n1+2].flag==0&&cb->chessBoard[m1][n1+2].jumpFlag!=1)//如果右面隔一个的位置是空的，那就是一次合法的跳跃
        {
            if(m1==m2&&n1+2==n2)
            {
                isLegal.push_back(m1);
                isLegal.push_back(n1+2);
                //cb->chessBoard[m1][n1+1].jumpFlag=1;
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

}
void game2player::judgewin0(){
    int count=0;
    int line =0;
    int col=0;

    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin();iter !=this->vectorPieces.end()-10;iter++)
    {
        Piece2* ptr=new Piece2();
        ptr=*iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if((line==3&&col==9)||(line==3&&col==10)||(line==3&&col==11)||(line==3&&col==12)||
                (line==2&&col==10)||(line==2&&col==11)||(line==2&&col==12)||(line==1&&col==11)||(line==1&&col==12)||(line==0&&col==12))
            count++;
    }
    if(count==10)
    {
        isWin[0]=true;
        rank[i_rank][0]='1';
        if(sameRank)
        {
            rank[i_rank][1]=1;
        }
        else
        {
            rank[i_rank][1]=0;
            sameRank=1;
        }
        i_rank++;
    }
    if(isGameOver())
    {
        this->hide();
        gameover*over=new gameover();
        over->show();
        over->setWindowTitle("游戏结束");
        QString rec_Rank="1";
        QString tmp_people,tmp_rank;

        tmp_people=rank[0][0];
        if(rank[0][1])
        {
            tmp_rank=rec_Rank;
        }
        else
        {
            tmp_rank="1";
            rec_Rank="1";
        }
        over->set1(tmp_people,tmp_rank);

        tmp_people=rank[1][0];
        if(rank[1][1])
        {
            tmp_rank=rec_Rank;
        }
        else
        {
            tmp_rank="2";
            rec_Rank="2";
        }
        over->set2(tmp_people,tmp_rank);
    }
}


void game2player::judgewin1(){
    int count=0;
    int line =0;
    int col=0;

    for(vector<Piece2*>::iterator iter = this->vectorPieces.begin()+10;iter !=this->vectorPieces.end();iter++)
    {
        Piece2* ptr=new Piece2();
        ptr=*iter;
        line = ptr->getRow();
        col = ptr->getColumn();
        if((line==16&&col==4)||(line==15&&col==4)||(line==15&&col==5)||(line==14&&col==4)||
                (line==14&&col==5)||(line==14&&col==6)||(line==13&&col==4)||(line==13&&col==5)||(line==13&&col==6)||(line==13&&col==7))
            count++;
    }
    if(count==10)
    {
        isWin[1]=true;
        rank[i_rank][0]='2';
        if(sameRank)
        {
            rank[i_rank][1]=1;
        }
        else
        {
            rank[i_rank][1]=0;
            sameRank=1;
        }
        i_rank++;
    }
    if(isGameOver())
    {
        this->hide();
        gameover*over=new gameover();
        over->show();
        over->setWindowTitle("游戏结束");
        QString rec_Rank="1";
        QString tmp_people,tmp_rank;

        tmp_people=rank[0][0];
        if(rank[0][1])
        {
            tmp_rank=rec_Rank;
        }
        else
        {
            tmp_rank="1";
            rec_Rank="1";
        }
        over->set1(tmp_people,tmp_rank);

        tmp_people=rank[1][0];
        if(rank[1][1])
        {
            tmp_rank=rec_Rank;
        }
        else
        {
            tmp_rank="2";
            rec_Rank="2";
        }
        over->set2(tmp_people,tmp_rank);
    }
}
bool game2player::isGameOver()
{
    if(i_bye+i_rank==2)
        return true;
    else
        return false;
}
