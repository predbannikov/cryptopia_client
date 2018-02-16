#ifndef ORDERS_H
#define ORDERS_H

#include <QObject>
#include <QMap>


class Orders
{
public:
    QObject parent;
    Orders();
    void setOrder(double am, double ord);
    void addOrder(double price, double vol, double tot, bool sell);
    struct Sum {
        double volume;
        double total;
    };
    QMap<double, Sum> sellOrders;
    QMap<double, Sum> bayOrders;
    double price;
    double amount;
};

#endif // ORDERS_H
