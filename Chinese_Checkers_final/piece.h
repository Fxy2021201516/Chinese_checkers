#ifndef PIECE_H
#define PIECE_H

#include <QObject>

class piece : public QObject
{
    Q_OBJECT
public:
    explicit piece(QObject *parent = nullptr);

signals:

};

#endif // PIECE_H
