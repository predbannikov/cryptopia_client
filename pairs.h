#ifndef PAIRS_H
#define PAIRS_H

#include <QObject>
#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>




//*********************    TableModel    ************************

class ModelPairs: public QAbstractTableModel
{
    Q_OBJECT
public:
    struct Currency {
        int Id;
        QString label;
        long double lastPrice;
//        bool filter;
    };



    const int COLUMN = 4;

    ModelPairs(QObject* parent = 0);
    int rowCount( const QModelIndex& ) const override;
    int columnCount( const QModelIndex&  ) const override;
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    void appendCurrency(QJsonArray array);
    void clearCurrencies();
    void parsing(QJsonArray array);
    void checkFilter();
    void favoriteCheck();
    void applyChanges();
    void setNewPairs(QJsonArray array);
    void findProfit();
    enum KeyMarkets{
        filter=0 , Label, LastPrice, TradePairId,
        AskPrice, BidPrice, Low, High, Volume,
        BuyVolume, SellVolume, Change, Open, Close,
        BaseVolume, BaseBuyVolume, BaseSellVolume
    };
    bool chkfiltr;
    bool chkFindProfit = 0;
    QList<int> favoriteCoin;
//    typedef QList<Currency> Currencies;
    QList<Currency> currencies;
    QList<Currency> sourceCurrencies;
    Currency currentCurrency;
//    Currencies currencies;
public slots:

//    void replaceCheck(QModelIndex index);
    void selectedRow(QModelIndex index);
signals:
    void getMarket(int id);
};


#endif // PAIRS_H
