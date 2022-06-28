#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include "networkdata.h"
#include <QtNetwork>
#include <QTcpSocket>
//#include "gameserver.h"

class NetworkServer : public QTcpServer {
    Q_OBJECT
public:
    explicit NetworkServer(QObject* parent = nullptr);

    void send(QTcpSocket* client, NetworkData data);//给客户端发
//    friend void gameServer::receiveData(QTcpSocket* client,NetworkData networkData);
signals:
    void receive(QTcpSocket* client, NetworkData data);//当服务端收到客户端发来的消息并解析成功时发送 receive 信号
    //client: 表示发送消息的客户端。
    //data: 表示接收到的数据。
    void parseError(const InvalidMessage&);
    void leave(QTcpSocket*);

private slots:
    void receiveData(QObject*);
    void disconnect(QObject*);
    void newconnection();

private:
    QList<QTcpSocket*> clients;
    QSignalMapper* disconnMapper;
    QSignalMapper* recvMapper;
};

#endif // NETWORKSERVER_H
