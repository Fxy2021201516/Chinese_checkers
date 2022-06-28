#include "iserror.h"
#include "ui_iserror.h"

isError::isError(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::isError)
{
    ui->setupUi(this);
}

isError::~isError()
{
    delete ui;
}
