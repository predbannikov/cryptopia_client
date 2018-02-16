#include "hystorypair.h"

HystoryDeals::HystoryDeals()
{

}

void HystoryDeals::addDeal(qint64 time, double price, double amount, double total)
{
    Deal deal;
    deal.price = price;
    deal.amount = amount;
    deal.total = total;
    hystory.insert(time,deal);
}

double HystoryDeals::getVolume(qint64 time1, qint64 time2) const
{
    if(time1 > time2 && time2 !=0) return 0;
    double volume;
    Deal deal;
    QMultiMap<qint64, Deal>::const_iterator startIt;
    QMultiMap<qint64, Deal>::const_iterator stopIt;

    qint64  found = getTimeSec(time1);
    if (found == 0){
        return 0;
    }
    startIt = hystory.find(found);
    if (time2 == 0)
        stopIt = hystory.end();
    else{
        found = getTimeSec(time2);
        stopIt = hystory.find(found);
    }

    while(startIt != stopIt){
        deal = startIt.value();
//        qDebug() << "Deal found " << deal.price << " " << deal.amount << " " << deal.total;
        QDateTime timestamp;
        timestamp.setTime_t(startIt.key());
        qDebug() << "found " << timestamp.toString("yyyy.MM.dd hh:mm:ss");

        startIt++;
    }
}

qint64 HystoryDeals::getTimeSec(qint64 time) const
{
    if(hystory.empty())
        return 0;
    QMultiMap<qint64, Deal>::const_iterator startIt;
    QMultiMap<qint64, Deal>::const_iterator stopIt;

    startIt = hystory.begin();
    stopIt = hystory.end();
    while(startIt != stopIt) {
        if(startIt.key() >= time)
        {
//            QDateTime timestamp;
//            timestamp.setTime_t(startIt.key());
//            qDebug() << "found " << timestamp.toString("yyyy.MM.dd hh:mm:ss");
            qDebug() << "getTimeSec ";
            return startIt.key();
        }
        startIt++;
    };
    return 0;
//    qDebug() << "end cycle";
}
