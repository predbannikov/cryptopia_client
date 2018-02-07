#ifndef HYSTORYPAIR_H
#define HYSTORYPAIR_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QHash>


class HystoryDeals
{
public:
    HystoryDeals();
    void addDeal(qint64 time, double price, double amount);
    double getVolume(qint64 time1, qint64 time2 = 0);
private:
    struct Deal{        // Cделки
        double price;
        double amount;
    };
    QHash<qint64, Deal> hystory;
};

#endif // HYSTORYPAIR_H
