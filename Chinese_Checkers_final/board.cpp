#include "board.h"
#include<iostream>
using namespace std;

void Board::initial()
{
    int i = 0;
    int j = 0;
    int count = 0;
    for(i=0;i<17;i++)//  i：行数
    {
        for(j=0;j<17;j++)//  j:列数
        {

            //各项数据初始化
            AIPower[i][j]=-10;
            humanPower[0][i][j]=-10;
            humanPower[1][i][j]=-10;
            chessBoard[i][j].x=0;
            chessBoard[i][j].y=0;
            chessBoard[i][j].flag=0;
            chessBoard[i][j].jumpFlag=0;
            chessBoard[i][j].piecePlace = NULL;
            chessBoard[i][j].paintFlag=-1;
        }
    }
    int row[17];
    int column[17];
    //横纵坐标
    //生成有效的棋盘，每个点对应各个坐标
    for(double d_i=0;d_i<17;d_i+=1)
    {
        count = 0;
        for(double d_j=0;d_j<17;d_j+=1)
        {
            this->chessBoard[(int)d_i][(int)d_j].y=48 + 27.3 * (16-d_i);
            this->chessBoard[(int)d_i][(int)d_j].x= 14 + 31.5 * (d_j+(d_i-8)/2);
        }
    }

}//初始化棋盘
