#ifndef PIECE2_H
#define PIECE2_H
#include <QPushButton>
#include<QMouseEvent>
#include<string>
using  std::string;
#include<QDebug>
#include<QToolTip>


class Piece2 : public QPushButton
{
    Q_OBJECT
public:
    Piece2(QWidget *parent = 0);
    ~Piece2(){}
    void setPlace(int row,int column)
    {
        this->setRow(row);
        this->setColumn(column);
    }

    int getRow(){return this->row;}
    int getColumn(){return this->column;}
    void setRow(int row){this->row=row;}
    void setColumn(int column){ this->column=column;}
    void setClick(int click)
    {
        this->click=click;
    }
    int getClick()
    {
        return this->click;
    }
    void setStatus(int s)
    {
        this->status=s;
    }

    int getStatus()
    {
        return this->status;
    }
    void mousePressEvent(QMouseEvent* event)//differerces
    {
        if(event->button()==Qt::LeftButton)
        {
            this->setClick(1);
            emit pieceInfo(this->getRow(),this->getColumn());
        }
        QPushButton::mousePressEvent(event);
    }



signals:
    void pieceInfo(int,int);
private:
    int row;//行
    int column;//列
    int click;//是否被选中
    int status;//棋子属于哪一方（看图，012345）

};

#endif // PIECE2_H
