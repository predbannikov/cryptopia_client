#ifndef ORDERS_H
#define ORDERS_H

#include <QObject>
#include <QMap>
#include <QAbstractTableModel>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QColor>

//*********************    ModelOrders    ************************
class ModelOrders: public QAbstractTableModel
{
    Q_OBJECT
public:
    struct Order {
        int Id;
        QString label;
        double price;
        double volume;
        double total;
        int level;
        int intensColor;
    };

    enum Type { sell, bay} type;

    static double startProfit;



    const int COLUMN = 3;

    ModelOrders(QObject* parent = 0, QString name = "name");
    int rowCount( const QModelIndex& ) const override;
    int columnCount( const QModelIndex&  ) const override;
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    void appendOrder(Order ord);
    void appendOrders(QJsonArray array);
    void clearOrders();
    void checkFilter();
    void checkLevel();
    void checkColor();
    void checkSelect();
    enum KeyMarkets{
        Price, Volume, Total,
        TradePairId, Label
    };
    QModelIndex saveIndex;
    typedef QList<Order> Orders;
//    typedef QMap<double, Order> Orders;
    Orders orders, sourceOrd;
    double filter = 100000000;
    int select = 0;



public slots:

    void selectedRow(QModelIndex index);
signals:
    void sendPrice(double price, double amount);
};



#endif // ORDERS_H
