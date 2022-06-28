#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <QWidget>
#include <QString>

namespace Ui {
class gameover;
}

class gameover : public QWidget
{
    Q_OBJECT

public:
    explicit gameover(QWidget *parent = nullptr);
    ~gameover();
    void set1(QString player,QString rank);
    void set2(QString player,QString rank);
    void set3(QString player,QString rank);
    void set4(QString player,QString rank);
    void set5(QString player,QString rank);
    void set6(QString player,QString rank);


private:
    Ui::gameover *ui;
};

#endif // GAMEOVER_H
