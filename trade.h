#ifndef TRADE_H
#define TRADE_H

#include <QObject>
#include <QDateTime>
#include <QDebug>

class Trade : public QObject
{
    Q_OBJECT
public:
    explicit Trade(QObject *parent = nullptr, QString name = "noname", int trend = 0);
//    Trade(QString name, double lvl, int id, int proc);
//    Trade(QString name);
    enum StateCreateOrder {StateLaunch = 0, StateCompleted = 1};
    enum StateTrend {StateTrendFlat = 0, StateTrendUp = 1, StateTrendDown = 2};

    struct StackOrder {
        int idOrder;
        int pairId;
        int type;
        double rate;
        double amount;
        double tradeSum;
        StateCreateOrder state;
    } lastSell, lastBay;

    StateTrend stateTrend;
    QVector<double> getHystoryBay();
    QVector<double> getHystorySell();
    double getPriceUp();
    double getPriceDown();
    double getPriceLevel();
    int getIdTrade();
    int getTimeWait();
    int getProcent();
    double getMinTrade();
    double getMaxTrade();
    int getOffset();
    const QString getNameLevel();
    void setData(double up, double down, int id);
    void setLevel(double lvl);
    void setName(QString name);
    void setProcent(int proc);
    void setIdTrade(int id);
    void setTimeWait(int sec);
    void setMinTrade(double min);
    void setMaxTrade(double max);
    void setOffset(int diff);
    void initTrade();
    void check(QVector<int> vec);
    void reset();

private:
    double getApproximate();
    double getPriceSell();
    double getPriceBay();
    void createOrderBay();
    void createOrderSell();
    double calcForTotalBay(double price);
    double calcForTotalSell(double price);
    void resetStateBay();
    void resetStateSell();
    void calcLevel();
    double getSumTradeSell();
    double getSumTradeBay();
    void sendStatTrade();

//    double totalMoney;        //
//    double totalCoin;
    QVector <double> hystoryBay;
    QVector <double> hystorySell;
    QString nameLevel;
    double priceLevel;
    int tradeId;
    int procent;
    double priceUp;
    double priceDown;

    int countTrade = 0;                             // Количество сделок
    int countSell = 0;
    int countBay = 0;
    qint64 timeWait = 1;
    double minTrade = 1.1;
    double maxTrade = 1.5;
    int offset = 0;

    qint64 timeMark;
//    qint64 curTime;



signals:

    void bayCoin(double price, double volume, int id);
    void sellCoin(double price, double volume, int id);
    void sendStatistics(QString statistics);
    void removeIdOrder(int id);
public slots:
    void setIdOrder(int id, int type);
    void getNotifi(double price, double amount, int pairId, int ordId);
};

#endif // TRADE_H
