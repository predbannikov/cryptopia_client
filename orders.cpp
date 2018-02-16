#include "orders.h"

Orders::Orders()
{
}

void Orders::addOrder(double price, double vol, double tot, bool sell)
{
    Sum sum;
    sum.volume = vol;
    sum.total  = tot;
    if(sell)
    {
        sellOrders.insert(price, sum);
    } else {
        bayOrders.insert(price, sum);
    }
}
