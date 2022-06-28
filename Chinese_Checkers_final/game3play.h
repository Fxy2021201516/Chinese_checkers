#ifndef game3play_H//把头文件都放在#ifndef，#define和结尾的#endif中，防止命名冲突
#define game3play_H//一般把字母变成大写，“.”变成“_”，其实一般来说应该前后也加“_”

#include <QWidget>
#include "board.h"
#include <vector>
#include <QRectF>
#include <QTimer>
using namespace std;

namespace Ui {
class game3play;

}//把game3play这个类定义在了命名空间中

class game3play : public QWidget//game3play是qwiget的一个子类，QMainWindow中在setUi时自动创建了一个菜单栏、工具栏、中心窗口和状态栏。而QWidget没有，继承自QWidget类的用户类中无法创建菜单栏等几种行为。
{
    Q_OBJECT

public:
    explicit game3play(QWidget *parent = nullptr);//用explicit关键字标示一个构造函数的话，就说明这个类的对象不允许在函数的参数传递过程中用调用这类构造函数。（因为编译器默认的是会调用这样的构造函数的）
    ~game3play();
    // void paintEvent(QPaintEvent* event);//画背景
     void produceRectF();//确定棋盘中的可点击区域
     int lightColor;//0是无点击
     int numOfWinner;
     int sameRank;
     void setPiece0();
     void setPiece2();
     void setPiece3();
     void producePiece3Player();
     void mousePressEvent(QMouseEvent* event);
     void changeInfo(int x1,int y1,int x2,int y2);//走棋后更正相关信息//differences
     bool isLegalMove(int x1,int y1,int x2,int y2);
     void isLegalJump(int x1,int y1,int x2,int y2);
//     void judgewin0();
//     void judgewin1();
//     void judgewin2();
//     void judgewin3();
//     void judgewin4();
//     void judgewin5();

private:
    Ui::game3play *ui;
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
     void processClick2(int,int);
     void processClick3(int,int);
private slots:

     void on_pushButton_clicked();
     //void on_beginpushButton_clicked();

};

#endif // game3play_H
