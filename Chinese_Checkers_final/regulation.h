#ifndef REGULATION_H
#define REGULATION_H

#include <QWidget>

namespace Ui {
class regulation;
}

class regulation : public QWidget
{
    Q_OBJECT

public:
    explicit regulation(QWidget *parent = nullptr);
    ~regulation();

private:
    Ui::regulation *ui;

private slots://regulation界面上返回按钮
    void on_pushButton_clicked();
};

#endif // REGULATION_H
