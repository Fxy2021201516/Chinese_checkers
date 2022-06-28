#ifndef ISERROR_H
#define ISERROR_H

#include <QMainWindow>

namespace Ui {
class isError;
}

class isError : public QMainWindow
{
    Q_OBJECT

public:
    explicit isError(QWidget *parent = nullptr);
    ~isError();

private:
    Ui::isError *ui;
};

#endif // ISERROR_H
