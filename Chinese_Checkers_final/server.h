#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include "networkserver.h"

namespace Ui {
class server;
}

class server : public QDialog
{
    Q_OBJECT

public:
    explicit server(QWidget *parent = nullptr);
    ~server();
    NetworkServer* server;

private:
    Ui::server *ui;
};

#endif // SERVER_H
