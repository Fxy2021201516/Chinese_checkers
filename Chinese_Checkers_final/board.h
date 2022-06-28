#ifndef BOARD_H
#define BOARD_H
#include "piece2.h"

typedef struct{
    int x;              //横坐标
    int y;              //纵坐标
    int flag;           //占用标志
    Piece2* piecePlace; //棋子指针
    int jumpFlag;
    int paintFlag;  //显示路径及复盘时的绘制标志
}info;

class Board
{
public:
    info chessBoard [17][17];
    void initial();                    //初始化函数
    int AIPower[17][17];
    int humanPower[2][17][17];
};

#endif // BOARD_H
