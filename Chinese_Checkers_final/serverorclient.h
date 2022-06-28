#ifndef SERVERORCLIENT_H
#define SERVERORCLIENT_H

#include <QMainWindow>
#include "gameplay.h"

namespace Ui {
class serverOrClient;
}

class serverOrClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit serverOrClient(QWidget *parent = nullptr);
    ~serverOrClient();

private slots://开始界面上按钮发出的信号

    void on_pushButton_clicked();//server

    void on_pushButton_2_clicked();//client
public slots:
    void errorOccurred();
    void connected();
signals:
    void sendData(gameplay* _game);
public slots:
    void getData(gameplay*_game);
private:
    Ui::serverOrClient *ui;
};

#endif // SERVERORCLIENT_H
