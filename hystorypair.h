#ifndef HYSTORYPAIR_H
#define HYSTORYPAIR_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QMultiHash>
#include <QDateTime>


class HystoryDeals
{
public:
    HystoryDeals();
    void addDeal(qint64 time, double price, double amount, double total);
    double getVolume(qint64 time1, qint64 time2 = 0) const;
    qint64 getTimeSec(qint64 time)const;
private:
    struct Deal{        // Cделки
        double price;
        double amount;
        double total;
    };
    QMultiMap<qint64, Deal> hystory;

};

#endif // HYSTORYPAIR_H
