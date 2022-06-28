#ifndef JOINROOMCLIENT_H
#define JOINROOMCLIENT_H

#include <QMainWindow>
#include "gameplay.h"

namespace Ui {
class joinRoomClient;
}

class joinRoomClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit joinRoomClient(QWidget *parent = nullptr);
    ~joinRoomClient();
//    static joinRoomClient *myjoinRoomClient;
    Ui::joinRoomClient *ui;
    QString getId();
    QString getRoomNum();
//    QString getPort();
    void joinRoom();//加入房间过程的函数

private:
    // Ui::joinRoomClient *ui;
    gameplay* gameInJoin;
signals:
    void sendData(gameplay*_game);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
public slots:
    void getData(gameplay*_game);
signals:
    void sendData1(gameplay* _game);
public slots:
    void getData1(gameplay*_game);
};

#endif // JOINROOMCLIENT_H
