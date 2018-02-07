#include "hystorypair.h"

HystoryDeals::HystoryDeals()
{

}

void HystoryDeals::addDeal(qint64 time, double price, double amount)
{
    Deal deal;
    deal.price = price;
    deal.amount = amount;
    hystory.insert(time,deal);
}

double HystoryDeals::getVolume(qint64 time1, qint64 time2)
{
    double volume;
    Deal *deal;
    QHash<qint64, Deal>::iterator startIt = hystory.find(time1);
    QHash<qint64, Deal>::iterator stopIt;
    if (time2 != 0)
        QHash<qint64, Deal>::iterator stopIt = hystory.find(time2);
    else
        stopIt = hystory.end();
    while(startIt != stopIt){
        deal = &startIt.value();
        qDebug() << deal->price << " " << deal->amount;
    }

}
