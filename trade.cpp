#include <QtMath>
#include "trade.h"

Trade::Trade(QObject *parent, QString name, int trend) : QObject(parent)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    nameLevel = name;
    switch(trend)
    {
    case 0:
        stateTrend = StateTrendFlat;
        break;
    case 1:
        stateTrend = StateTrendUp;
        break;
    case 2:
        stateTrend = StateTrendDown;
        break;
    }

    lastBay.state = StateCompleted;
    lastSell.state = StateCompleted;
    lastBay.rate = 0;
    lastSell.rate = 0;
    timeMark = QDateTime::currentSecsSinceEpoch();
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

double Trade::getMinTrade()
{
    return minTrade;
}

double Trade::getMaxTrade()
{
    return maxTrade;
}

int Trade::getOffset()
{
    return offset;
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
    qint64 curTime = QDateTime::currentSecsSinceEpoch();
    if((curTime - timeMark) >= timeWait)
    {
//        qDebug() << "timeWait has passed: " << curTime - timeMark;
        timeMark = curTime;

        int openBay = vec.indexOf(lastBay.idOrder);         // Проверка существования ордера, если ордера уже или и небыло то получаем -1
        int openSell = vec.indexOf(lastSell.idOrder);       // Проверка существования ордера, если ордера уже или и небыло то получаем -1
        if(stateTrend == StateTrendUp)
        {
            if(openSell == -1 && lastSell.state == StateCompleted)      // Если ордер отсутствует и мы находимся не в состоянии создания ордера
            {                                                       // то выполняем блок кода
                if(!qFuzzyIsNull(lastSell.rate)){
                    hystorySell.append(lastSell.tradeSum);
                }
                createOrderSell();
                sendStatTrade();
            }
        } else if(stateTrend == StateTrendDown)
        {

        } else if(stateTrend == StateTrendFlat)
        {
            if(openSell == -1 && lastSell.state == StateCompleted)      // Если ордер отсутствует и мы находимся не в состоянии создания ордера
            {                                                       // то выполняем блок кода
                if(!qFuzzyIsNull(lastSell.rate)){
                    hystorySell.append(lastSell.tradeSum);
                    double nextLvl = priceLevel + (priceLevel/1000. * double(offset));
                    qDebug() << "Block to sell. Next level = " << QString::number(nextLvl, 'f', 8);
                    setLevel(nextLvl);
                    int idOrdForRemove = lastBay.idOrder;
                    initTrade();
                    createOrderBay();
                    emit removeIdOrder(idOrdForRemove);
                }
                createOrderSell();
                sendStatTrade();
            }
            if(openBay == -1 && lastBay.state == StateCompleted)     //
            {
                if(!qFuzzyIsNull(lastBay.rate)){
                    hystoryBay.append(lastBay.tradeSum);
                    double nextLvl = priceLevel - (priceLevel/1000. * double(offset));
                    qDebug() << "Block to bay. Next level = " << QString::number(nextLvl, 'f', 8);
                    setLevel(nextLvl);
                    int idOrdForRemove = lastSell.idOrder;
                    initTrade();
                    createOrderSell();
                    emit removeIdOrder(idOrdForRemove);
                }
                createOrderBay();
                sendStatTrade();
            }
        }
    }
}

void Trade::reset()
{
    int idOrdForRemove = lastBay.idOrder;
    lastBay.rate = 0;
    emit removeIdOrder(idOrdForRemove);
    idOrdForRemove = lastSell.idOrder;
    lastSell.rate = 0;
    emit removeIdOrder(idOrdForRemove);
}

double Trade::getApproximate()
{
//    qsrand(QDateTime::currentMSecsSinceEpoch());
    int rand = qrand()%1000 + 1;
    double randD = rand/100000000.;
    double round = qFloor(randD*100000000. + 0.5)/100000000.;
    return round;
}

void Trade::setTimeWait(int sec)
{
    timeWait = sec;
}

void Trade::setMinTrade(double min)
{
    minTrade = min;
}

void Trade::setMaxTrade(double max)
{
    maxTrade = max;
}

void Trade::setOffset(int diff)
{
    offset = diff;
}

void Trade::initTrade()
{
    calcLevel();
}

double Trade::getPriceSell()
{
//    qsrand(QDateTime::currentMSecsSinceEpoch());
    double discret = priceUp/1000000;
    int rand = qrand()%100 + 1;
    countSell++;
    double sell = priceUp + discret * rand;
    double roundSell = qFloor(sell*100000000. + 0.5)/100000000.;
    return roundSell;
}

double Trade::getPriceBay()
{
//    qsrand(QDateTime::currentMSecsSinceEpoch());
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
        lastBay.tradeSum = lastBay.rate * lastBay.amount;
        lastBay.tradeSum += lastBay.tradeSum/500;
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
        lastSell.tradeSum = lastSell.rate * lastSell.amount;
        lastSell.tradeSum += lastSell.tradeSum/500;
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
    double fee, sum;
//    qsrand(QDateTime::currentMSecsSinceEpoch());
    double total = (double)(rand())/RAND_MAX*(maxTrade - minTrade) + minTrade;
    sum = total/(1+1./500.);
    fee = sum/500.;
    double bayAmount = sum/price;
    double roundAmount = qFloor(bayAmount*100000000. + 0.5)/100000000.;
    qDebug() << "Bay for amount = " << QString::number(fee + sum, 'f', 8);
    return roundAmount;
}

double Trade::calcForTotalSell(double price)
{
    if(qFuzzyIsNull(price))
        return 0;
    double fee, sum;
//    qsrand(QDateTime::currentSecsSinceEpoch());
    double total = (double)(rand())/RAND_MAX*(maxTrade - minTrade) + minTrade;
    sum = total/(1-1./500.);
    fee = sum/500.;
    double sellAmount = sum/price;
    double roundAmount = qFloor(sellAmount*100000000. + 0.5)/100000000.;
    qDebug() << "Sell for amount = " << QString::number(fee + sum, 'f', 8);
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

//double Trade::getSumTradeSell()
//{
//    double fee, sum;
//    sum = lastSell.amount * lastSell.rate;
////    fee = sum/500.;
//    lastSell.tradeSum = sum - fee;
//    return lastSell.tradeSum;
//}

//double Trade::getSumTradeBay()
//{
//    double fee, sum;
//    sum = lastBay.amount * lastBay.rate;
//    fee = sum/500.;
//    lastBay.tradeSum = sum + fee;
//    return lastBay.tradeSum;
//}

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
    statistic.append(QString::number(hystorySell.size()));
    statistic.append("\n");
    statistic.append("Total bay = ");
    statistic.append(QString::number(bay, 'f', 8));
    statistic.append(" count trade = ");
    statistic.append(QString::number(hystoryBay.size()));
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
                if(ordId == -1)
                    lastBay.rate = 0;
                lastBay.idOrder = ordId;
                resetStateBay();
            }
        }
    }
    if(qFuzzyCompare(lastSell.rate,price))
    {
        if(qFuzzyCompare(lastSell.amount, amount))
        {
            if(lastSell.pairId == pairId)
            {
                if(ordId == -1)
                    lastSell.rate = 0;
                lastSell.idOrder = ordId;
                resetStateSell();
            }
        }
    }
}

