#ifndef GAME2PLAYER_H//把头文件都放在#ifndef，#define和结尾的#endif中，防止命名冲突
#define GAME2PLAYER_H//一般把字母变成大写，“.”变成“_”，其实一般来说应该前后也加“_”

#include <QWidget>
#include "board.h"
#include <vector>
#include <QRectF>
#include <QTimer>
using namespace std;

namespace Ui {
class game2player;

}//把gameplay这个类定义在了命名空间中

class game2player : public QWidget//gameplay是qwiget的一个子类，QMainWindow中在setUi时自动创建了一个菜单栏、工具栏、中心窗口和状态栏。而QWidget没有，继承自QWidget类的用户类中无法创建菜单栏等几种行为。
{
    Q_OBJECT

public:
    explicit game2player(QWidget *parent = nullptr);//用explicit关键字标示一个构造函数的话，就说明这个类的对象不允许在函数的参数传递过程中用调用这类构造函数。（因为编译器默认的是会调用这样的构造函数的）
    ~game2player();
    // void paintEvent(QPaintEvent* event);//画背景
    void produceRectF();//确定棋盘中的可点击区域
    int lightColor;//0是无点击
    int numOfWinner;
    int sameRank;
    char rank[2][2];
    int i_rank;
    int i_bye;
    void setPiece0();
    void setPiece1();
    void producePiece2Player();
    void mousePressEvent(QMouseEvent* event);
    void changeInfo(int x1,int y1,int x2,int y2);//走棋后更正相关信息//differences
    bool isLegalMove(int x1,int y1,int x2,int y2);
    void isLegalJump(int x1,int y1,int x2,int y2);
    void judgewin0();
    void judgewin1();
    bool isGameOver();
    bool isWin[2];
private:
    Ui::game2player *ui;
    QPainter *paint;
    Board *cb;
    Piece2 *select;
    int flag[183];
    vector<QRectF*> vectorRectF;     //可点击的有效区域
    vector<Piece2*> vectorPieces;     //所有棋子
    vector<int> isLegal;          //所有可以走到的位置
    int control;//谁在走
    int judge0;

public slots:
    void processClick0(int,int);
    void processClick1(int,int);
private slots:

    void on_pushButton_clicked();

};

#endif // GAME2PLAY_H
