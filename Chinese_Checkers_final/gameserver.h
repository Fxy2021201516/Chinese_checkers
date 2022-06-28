#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QMainWindow>
#include "networkserver.h"
#include "players.h"
#include "board.h"
#include <QTcpSocket>
using namespace std;
namespace Ui {
class gameServer;
}

class gameServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit gameServer(QWidget *parent = nullptr);
    ~gameServer();
    NetworkServer *server;
    QString roomName;
    players thePlayers[6];
    bool isLegalJoin(QString tmpName);
    int control;
    int places[50];
    int numOfPlaces;
    bool isLegalMove(int x1, int y1, int x2, int y2);
    void isLegalJump(int m1, int n1, int m2, int n2) ;
    void moveControl();
    void judgewin0(int _i);
    void judgewin1(int _i);
    void judgewin2(int _i);
    void judgewin3(int _i);
    void judgewin4(int _i);
    void judgewin5(int _i);
    bool isGameOver();
    int sameRank;
    //    bool isWin[6];
    char rank[6][2];
    int i_rank;
    int i_bye;
    QList<QTcpSocket*>recClients;
    QTimer *myTimer;// 定义定时器对象
    int counttime;              //倒计时时间
    int totaltime;              //总时间
    bool isRunning;
    void producePiece();//创建棋子
    void setPiece0(int _i);
    void setPiece1(int _i);
    void setPiece2(int _i);
    void setPiece3(int _i);
    void setPiece4(int _i);
    void setPiece5(int _i);
    int getNumBefore(int i);
    int getNumAfter(int i);
    int getTotalNum();
    void changeInfo(int m1,int n1,int m2,int n2);//走棋后更正相关信息
    int numOfPlayersBegin;
    void processClick(int,int);
    int waitTime;
    int totalNum;
    int changePlayerId(QChar ch);
    char changePlayerId2(int c);
private:
    Ui::gameServer *ui;
    Board *cb;
    Piece2 *select;
    int flag[183];
    vector<int> isLegal;
    vector<Piece2*> vectorPieces;     //所有棋子
public slots:
    void receiveData(QTcpSocket* client,NetworkData networkData);
    void TimeSlot();
};

#endif // GAMESERVER_H
