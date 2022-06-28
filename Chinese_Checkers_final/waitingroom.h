#ifndef WAITINGROOM_H
#define WAITINGROOM_H

#include <QMainWindow>
#include "gameplay.h"

namespace Ui {
class waitingRoom;
}

class waitingRoom : public QMainWindow
{
    Q_OBJECT

public:
    explicit waitingRoom(QWidget *parent = nullptr);
    ~waitingRoom();
    gameplay* gameInWaitingroom;

private:
    Ui::waitingRoom *ui;
signals:
    void sendData1(gameplay* _game);
    //void sendQstringFromGameplay(QString str);
public slots:
    void getData1(gameplay*_game);
    //void getQstringFromGameplay(QString str);
};

#endif // WAITINGROOM_H
