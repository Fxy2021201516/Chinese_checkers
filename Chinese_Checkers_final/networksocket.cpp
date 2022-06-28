#include "networksocket.h"
#include <QtCore>

NetworkSocket::NetworkSocket(QTcpSocket* socket, QObject* parent)
    : QObject(parent)
    , socket(socket) {
    this->socket->setParent(this);
    connect(this->base(), &QAbstractSocket::readyRead, this, &NetworkSocket::receiveData);
}

QTcpSocket* NetworkSocket::base() const {
    return this->socket;
}

void NetworkSocket::hello(const QString& host, quint16 port)
//host: 远程机器的 IPv4 地址,让所有客户端与服务端接入同一个局域网（如校园网），并在此传入内网地址。
//port: 远程机器的端口号
{
    this->socket->abort();
    qDebug()<<"begin connectToHost";
    this->socket->connectToHost(host, port);
    qDebug()<<"end connectToHost";

}

void NetworkSocket::bye() {
    this->socket->disconnectFromHost();
}


void NetworkSocket::receiveData() {
    auto whole_block = this->socket->readAll();
    auto blocks = whole_block.split('\n');
    for (const auto& block : blocks) {
        try {
            NetworkData recv(block);
            qDebug() << "Client: "
                     << static_cast<int>(recv.op) 
                     << ' ' << recv.data1 << ' ' << recv.data2 << Qt::endl;
            emit receive(recv);
        }
        catch (const InvalidMessage& e) {
            if (!e.messageReceived().isEmpty()) {
                emit parseError(e);
            }
        }
    }
}

void NetworkSocket::send(NetworkData data) {
    this->socket->write(data.encode());
    this->socket->flush();
}
