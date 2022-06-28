#ifndef AIGAME_H
#define AIGAME_H

#include <QWidget>

namespace Ui {
class aigame;
}

class aigame : public QWidget
{
    Q_OBJECT

public:
    explicit aigame(QWidget *parent = nullptr);
    ~aigame();
    void paintEvent(QPaintEvent* event);//画背景

private:
    Ui::aigame *ui;


//按钮函数
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // AIGAME_H
