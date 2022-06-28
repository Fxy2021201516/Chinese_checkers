#ifndef GAMEPLAY_H//把头文件都放在#ifndef，#define和结尾的#endif中，防止命名冲突
#define GAMEPLAY_H//一般把字母变成大写，“.”变成“_”，其实一般来说应该前后也加“_”

#include <QWidget>
#include "board.h"
#include <vector>
#include <QRectF>
#include <QTimer>
//#include"networkserver.h"
#include "networksocket.h"
#include"networkdata.h"
#include "players.h"

using namespace std;

namespace Ui {
class gameplay;

}//把gameplay这个类定义在了命名空间中

class gameplay : public QWidget//gameplay是qwiget的一个子类，QMainWindow中在setUi时自动创建了一个菜单栏、工具栏、中心窗口和状态栏。而QWidget没有，继承自QWidget类的用户类中无法创建菜单栏等几种行为。
{
    Q_OBJECT

public:
    explicit gameplay(QWidget *parent = nullptr);//用explicit关键字标示一个构造函数的话，就说明这个类的对象不允许在函数的参数传递过程中用调用这类构造函数。（因为编译器默认的是会调用这样的构造函数的）
    ~gameplay();
    void produceRectF();//确定棋盘中的可点击区域
    int lightColor;//0是无点击
    int numOfWinner;


    void setPiece0(int _i);
    void setPiece1(int _i);
    void setPiece2(int _i);
    void setPiece3(int _i);
    void setPiece4(int _i);
    void setPiece5(int _i);
    void producePiece();//创建棋子
    void mousePressEvent(QMouseEvent* event);
    void changeInfo(int m1,int n1,int m2,int n2);//走棋后更正相关信息
    NetworkData JoinRoom(NetworkData networkData);
    NetworkSocket *socket;
    players thePlayers[5];
    players me;
    //  void gameBegin();
    int myTurn;//设myturn和control，myturn记录玩家能操控的棋子，control记录此时要改变显示的棋子
    //玩家能操控的棋子两个判断条件，此时要改变显示的棋子一个判断条件
    int places[50];
    int numOfPlaces;
    int cpyPlaces[6][50];
    int cpyNumOfPlaces[6];
    QString roomIdInGameplay;
    QTimer *myTimer;// 定义定时器对象
    int getNumBefore(int i);
    int getNumAfter(int i);
    int getTotalNum();
    int counttime;
    void setPower();
    void buildPath(int pieceNumber,int _myturn);//找到棋子移动的路径并更新信息
    int AIJudge(int _myturn);//判断哪个棋子要被移动
    vector<int> longjump;       //记录AI当前每个点可以到达的最大权值
    vector<int> opjump;         //记录玩家当前每个点可以到达的最大权值
    vector<int> Olongjump;      //记录AI上一步每个点可以到达的最大权值
    vector<int> Oopjump;        //记录玩家上一步每个点可以到达的最大权值
    void longJump();                                //生成AI下一步每个棋子能走的最大权值的位置
    void humanJump(int _i,int _num);                               //生成玩家下一步每个棋子能走的最大权值的位置
    void OhumanJump(int _i,int _num);                              //生成玩家每个棋子当前能走的最大权值的位置
    vector<int> legal;          //记录可以达到的点
    void isLegalMove(int m1, int n1);
    void isLegalJump(int m1, int n1);
    int range;                  //棋局控制权值
    void getPath(int m1,int n1,int m2,int n2,int _num);
    bool getJumpPath(int m1,int n1,int m2,int n2,int _num);
    bool AIOrNot;
    bool isRunning;
    void showPlaces(int i);//展示棋子的移动轨迹并停留三秒
    void paintEvent(QPaintEvent *event);
    virtual void timerEvent(QTimerEvent*e);
    QTimer Gtime;
    int MoveTime,updateTime,PTime;
private:

    Ui::gameplay *ui;
    QPainter *paint;
    Board *cb;
    Piece2 *select;
    int flag[183];
    vector<QRectF*> vectorRectF;     //可点击的有效区域
    vector<Piece2*> vectorPieces;     //所有棋子
    int control;//谁在走
    int judge0;
    int attck;                  //进攻比例
    int defence;                //防守比例
    int fornest;                //搭桥比例
    int beginNum;
    //    NetworkServer *server;
    int changePlayerId(QChar ch);//改变表示坐标的a,b,c,d,e
    char changePlayerId2(int c);


public slots:
    void processClick0(int,int);
    void processClick1(int,int);
    void processClick2(int,int);
    void processClick3(int,int);
    void processClick4(int,int);
    void processClick5(int,int);
    void receive(NetworkData networkData);
signals:
    //void sendQstringFromGameplay(QString str);
private slots:

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    //    void receiveData();
    //void on_beginpushButton_clicked();
    //void getQstringFromGameplay(QString str);
};

#endif // GAMEPLAY_H
