#ifndef CHOOSESTYLE_H
#define CHOOSESTYLE_H

#include <QWidget>

namespace Ui {
class choosestyle;
}

class choosestyle : public QWidget
{
    Q_OBJECT

public:
    explicit choosestyle(QWidget *parent = nullptr);
    void getGameState();
    ~choosestyle();

private:
    Ui::choosestyle *ui;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // CHOOSESTYLE_H
