#include "waitingroom.h"
#include "ui_waitingroom.h"
#include "gameplay.h"

waitingRoom::waitingRoom(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::waitingRoom)
{
    ui->setupUi(this);
}

waitingRoom::~waitingRoom()
{
    delete ui;
}
void waitingRoom::getData1(gameplay*_game)
{
    gameInWaitingroom=_game;

}

