#ifndef ISSUCCESSFUL_H
#define ISSUCCESSFUL_H

#include <QMainWindow>

namespace Ui {
class isSuccessful;
}

class isSuccessful : public QMainWindow
{
    Q_OBJECT

public:
    explicit isSuccessful(QWidget *parent = nullptr);
    ~isSuccessful();

private:
    Ui::isSuccessful *ui;
};

#endif // ISSUCCESSFUL_H
