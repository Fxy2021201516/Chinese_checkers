#include "piece2.h"


/*Piece2::Piece2(QWidget *parent)
    : QPushButton{parent}
{

}*/
Piece2::Piece2(QWidget *parent) :QPushButton(parent)
{
    setClick(0);
    setStatus(0);
}
