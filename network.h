#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QDebug>
#include <windows.h>

class Network : public QObject
{
    Q_OBJECT
public:
    Network(QString name);

private:
    QString name;

signals:
    void send(int);
public slots:
    void doWork();
    void update (int i);
};

#endif // NETWORK_H
