#include "server.h"
#include "ui_server.h"

server::server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::server)
{
    ui->setupUi(this);
    this->server = new NetworkServer(this);

    connect(this->server, &NetworkServer::receive, this, &server::receiveData);
}

server::~server()
{
    delete ui;
}
