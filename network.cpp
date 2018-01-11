#include "network.h"

Network::Network(QString s) : name(s)
{


}
void Network::doWork()
{
    for(int i = 0; i <= 100; i++)
    {
        emit send(i);
        Sleep(1000);
    }
}
void Network::update (int i)
{
    qDebug() << QString::number(i);
}
