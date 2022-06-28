#ifndef PLAYERS_H
#define PLAYERS_H

#include <QObject>
#include <QString>

class players : public QObject
{
    Q_OBJECT
public:
    explicit players(QObject *parent = nullptr);
    int num;
    QString name;
    int isReady;

signals:

};

#endif // PLAYERS_H
