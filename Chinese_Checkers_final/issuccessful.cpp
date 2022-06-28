#include "issuccessful.h"
#include "ui_issuccessful.h"

isSuccessful::isSuccessful(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::isSuccessful)
{
    ui->setupUi(this);
}

isSuccessful::~isSuccessful()
{
    delete ui;
}
