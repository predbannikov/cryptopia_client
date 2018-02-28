#include <QtMath>
#include "trade.h"

Trade::Trade(QObject *parent, QString name) : QObject(parent)
  //Trade::Trade(QString name = "Name", double lvl = 0, int id = 0, int proc = 0)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    nameLevel = name;
    lastBay.state = StateCompleted;
    lastSell.state = StateCompleted;
    lastBay.rate = 0;
    lastSell.rate = 0;
    timeMark = QDateTime::currentSecsSinceEpoch();
    for(int i=0; i<5; i++){
        hystoryBay.append((qrand()%100+1)/1000.);
        hystorySell.append((qrand()%120+1)/1000.);
    }
}

QVector<double> Trade::getHystoryBay()
{
    return hystoryBay;
}

QVector<double> Trade::getHystorySell()
{
    return hystorySell;
}

double Trade::getPriceUp()
{
    return priceUp;
}

double Trade::getPriceDown()
{
    return priceDown;
}

double Trade::getPriceLevel()
{
    return priceLevel;
}

int Trade::getIdTrade()
{
    return tradeId;
}

int Trade::getTimeWait()
{
    return timeWait;
}

int Trade::getProcent()
{
    return procent;
}

double Trade::getMaxTrade()
{
    return maxTrade;
}

const QString Trade::getNameLevel()
{
    return nameLevel;
}

void Trade::setData(double up, double down, int id)
{
    tradeId = id;
    priceUp = up;
    priceDown = down;
    lastBay.idOrder = 0;
    lastSell.idOrder = 0;
    lastBay.state = StateCompleted;
    lastSell.state = StateCompleted;

}

void Trade::setLevel(double lvl)
{
    priceLevel = lvl;
}

void Trade::setName(QString name)
{
    nameLevel = name;
}

void Trade::setProcent(int proc)
{
    procent = proc;
}

void Trade::setIdTrade(int id)
{
    tradeId = id;
}

void Trade::check(QVector<int> vec)
{
    int bay = vec.indexOf(lastBay.idOrder);         // Проверка существования ордера, если ордера уже или и небыло то получаем -1
    int sell = vec.indexOf(lastSell.idOrder);       // Проверка существования ордера, если ордера уже или и небыло то получаем -1


//    if(vec != tradeId)           // Если id не совпадает с айди этого класса значит выходим
//        return;
    curTime = QDateTime::currentSecsSinceEpoch();

    if((curTime - timeMark) >= timeWait)
    {
        timeMark = curTime;

        if(sell == -1 && lastSell.state == StateCompleted)      // Если ордер отсутствует и мы находимся не в состоянии создания ордера
        {                                                       // то выполняем блок кода
            if(!qFuzzyIsNull(lastSell.rate)){
                hystorySell.append(getSumTradeSell());
            }
//            qDebug() << "sell=" << sell;
            createOrderSell();
            sendStatTrade();
//            qDebug()  << "getSumTradeSell()=" << QString::number(getSumTradeSell(), 'f', 8);
//            qDebug() << "createOrderSell()=" ;
//            return;
        }
        if(bay == -1 && lastBay.state == StateCompleted)     //
        {
            if(!qFuzzyIsNull(lastBay.rate)){
                hystoryBay.append(getSumTradeBay());
            }
//            qDebug() << "bay=" << bay;
            createOrderBay();
            sendStatTrade();
//              qDebug()  << "getSumTradeBay()=" << QString::number(getSumTradeBay(), 'f', 8);
//            qDebug() << "createOrderBay()=" ;
        }
    }
}

double Trade::getApproximate()
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    int rand = qrand()%1000 + 1;
    double randD = rand/100000000.;
    double round = qFloor(randD*100000000. + 0.5)/100000000.;
    return round;
}

void Trade::setTimeWait(int sec)
{
    timeWait = sec;
}

void Trade::setMaxTrade(double max)
{
    maxTrade = max;
}

void Trade::initTrade()
{
    calcLevel();
}

double Trade::getPriceSell()
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    double discret = priceUp/1000000;
    int rand = qrand()%100 + 1;
    countSell++;
    double sell = priceUp + discret * rand;
    double roundSell = qFloor(sell*100000000. + 0.5)/100000000.;
    return roundSell;
}

double Trade::getPriceBay()
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    double discret = priceDown/1000000;
    int rand = qrand()%100 + 1;
    countBay++;
    double bay = priceDown - discret * rand;
    double roundBay = qFloor(bay*100000000. + 0.5)/100000000.;
    return roundBay;
}

void Trade::createOrderBay()
{
    if(lastBay.state == StateCompleted)
    {
        lastBay.pairId = tradeId;
        lastBay.rate = getPriceBay();       // Генерируем подходящий прайс
        lastBay.amount = calcForTotalBay(lastBay.rate);
        lastBay.type = 0;                   // bay = 0;
        qDebug() << "rate=" << QString::number(lastBay.rate,'f', 8) << " amount=" << QString::number(lastBay.amount, 'f', 8) << " pairId=" << lastBay.pairId;
        lastBay.state = StateLaunch;
        emit bayCoin(lastBay.rate, lastBay.amount, lastBay.pairId);
    } else {
        return;
    }
}

void Trade::createOrderSell()
{
    if(lastSell.state == StateCompleted)
    {
        lastSell.pairId = tradeId;
        lastSell.rate = getPriceSell();       // Генерируем подходящий прайс
        lastSell.amount = calcForTotalSell(lastSell.rate);
        lastSell.type = 1;                   // sell = 1;
        lastSell.state = StateLaunch;
        qDebug() << "rate=" << QString::number(lastSell.rate,'f', 8) << " amount=" << QString::number(lastSell.amount, 'f', 8) << " pairId=" << lastSell.pairId;
            emit sellCoin(lastSell.rate, lastSell.amount, lastSell.pairId);
    } else {
        return;
    }
}

double Trade::calcForTotalBay(double price)
{
    if(qFuzzyIsNull(price))
        return 0;
    if(qFuzzyIsNull(maxTrade))
        return 0;
    double fee, sum, total = maxTrade;
    double approximate = getApproximate();
//    qDebug() << "approximate=" << QString::number(approximate, 'f', 8);
    total += approximate;
    sum = total/(1+1./500.);
    fee = sum/500.;
    double bayAmount = sum/price;
    double roundAmount = qFloor(bayAmount*100000000. + 0.5)/100000000.;
    return roundAmount;
}

double Trade::calcForTotalSell(double price)
{
    if(qFuzzyIsNull(price))
        return 0;
    if(qFuzzyIsNull(maxTrade))
        return 0;
    double fee, sum, total = maxTrade;
    total += getApproximate();
    sum = total/(1-1./500.);
    fee = sum/500.;
    double sellAmount = sum/price;
    double roundAmount = qFloor(sellAmount*100000000. + 0.5)/100000000.;
    return roundAmount;
}

void Trade::resetStateBay()
{
    lastBay.state = StateCompleted;
    qDebug() << "Reset state bay";
}

void Trade::resetStateSell()
{
    lastSell.state = StateCompleted;
    qDebug() << "Reset state sell";
}

void Trade::calcLevel()
{
    if(qFuzzyIsNull(priceLevel))
        return;
    double fee, sum, discretInProcent;
    discretInProcent = (priceLevel/100.)/2. * procent;
//    qDebug() << discretInProcent;
    sum = discretInProcent + priceLevel;
//    qDebug() << "sum up=" << sum;
    fee = sum/500.;
//    qDebug() << "fee up =" << fee;
    priceUp = sum + fee;
    sum = priceLevel - discretInProcent;
//    qDebug() << "sum down=" << sum;
    fee = sum/500.;
//    qDebug() << "fee down =" << fee;
    priceDown = sum - fee;
}

double Trade::getSumTradeSell()
{
    double fee, sum;
    sum = lastSell.amount * lastSell.rate;
//    fee = sum/500.;
    lastSell.tradeSum = sum - fee;
    return lastSell.tradeSum;
}

double Trade::getSumTradeBay()
{
    double fee, sum;
    sum = lastBay.amount * lastBay.rate;
    fee = sum/500.;
    lastBay.tradeSum = sum + fee;
    return lastBay.tradeSum;
}

void Trade::sendStatTrade()
{
    double sell = 0, bay = 0;
    int i, j;
    for(i = 0; i < hystorySell.size(); i++)
        sell += hystorySell[i];
    for(j = 0; j < hystoryBay.size(); j++)
        bay += hystoryBay[j];
    QString statistic;
    statistic.append("Total sell = ");
    statistic.append(QString::number(sell, 'f', 8));
    statistic.append(" count trade = ");
    statistic.append(QString::number(i));
    statistic.append("\n");
    statistic.append("Total bay = ");
    statistic.append(QString::number(bay, 'f', 8));
    statistic.append(" count trade = ");
    statistic.append(QString::number(j));
    emit sendStatistics(statistic);
}

void Trade::setIdOrder(int id, int type)
{
    if(type == 1)           // 1 == sell
    {
        lastSell.idOrder = id;
    } else if(type == 0)    // 0 == bay
    {
        lastBay.idOrder = id;
    }
}

void Trade::getNotifi(double price, double amount, int pairId, int ordId)
{
    if(qFuzzyCompare(lastBay.rate,price))
    {
        if(qFuzzyCompare(lastBay.amount, amount))
        {
            if(lastBay.pairId == pairId)
            {
                resetStateBay();
                lastBay.idOrder = ordId;
            }
        }
    }
    if(qFuzzyCompare(lastSell.rate,price))
    {
        if(qFuzzyCompare(lastSell.amount, amount))
        {
            if(lastSell.pairId == pairId)
            {
                resetStateSell();
                lastSell.idOrder = ordId;
            }
        }
    }
}

