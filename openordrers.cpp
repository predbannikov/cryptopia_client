#include "openordrers.h"

OpenOrdrers::OpenOrdrers(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant OpenOrdrers::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    return QVariant();
    // формируем заголовки по номеру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case OrderId:
                return tr("OrderId");
            case TradePairId:
                return tr("PairId");
            case Market:
                return tr("Market");
            case Type:
                return tr("Type");
            case Rate:
                return tr("Rate");
            case Amount:
                return tr("Amount");
            case Total:
                return tr("Total");
            case TimeStamp:
                return tr("TimeStamp");
        }
    }
    return QVariant();
}

int OpenOrdrers::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return myOrders.size();
}

int OpenOrdrers::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COLUMN;
}

QVariant OpenOrdrers::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    const MyOrder &myOrd = myOrders.at(index.row());
    switch(role){
        case Qt::DisplayRole:{
            int key = index.column();
            switch( key) {
                case OrderId:
                    return myOrd.orderId;
                case TradePairId:
                    return myOrd.TradePairId;
                case Market:
                    return myOrd.market;
                case Type:
                    return myOrd.type;
                case Rate:
                    return QString::number(myOrd.rate, 'f', 8);
                case Amount:
                    return QString::number(myOrd.amount, 'f', 8);
                case Total:
                    return QString::number(myOrd.total, 'f', 8);
                case TimeStamp:
                    return myOrd.timestamp;
            }
        }
        default:
            return QVariant();

    }


    return QVariant();
}

bool OpenOrdrers::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
//    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool OpenOrdrers::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool OpenOrdrers::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

bool OpenOrdrers::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}

void OpenOrdrers::clearOpenOrders()
{
    if(myOrders.empty())
        return;
    removeRows(0,myOrders.count(),QModelIndex());
    myOrders.clear();
}

void OpenOrdrers::setNewOpenOrders(QJsonArray array)
{
    if(array.empty())
        return;
    clearOpenOrders();
    parsing(array);
    handler();
    applyChanges();
}

void OpenOrdrers::parsing(QJsonArray array)
{
//    qDebug() << "void OpenOrdrers::parsing(QJsonArray array)";
    MyOrder myOrder;
    for(int i=0; i<array.size(); i++)
    {
        QJsonObject jdata = array[i].toObject();
        if(jdata.contains("OrderId") && jdata["OrderId"].isDouble())
        {
            myOrder.orderId = jdata["OrderId"].toInt();
        }
        if(jdata.contains("Market") && jdata["Market"].isString())
        {
            myOrder.market = jdata["Market"].toString();
        }
        if(jdata.contains("TradePairId") && jdata["TradePairId"].isDouble())
        {
            myOrder.TradePairId = jdata["TradePairId"].toInt();
        }
        if(jdata.contains("Type") && jdata["Type"].isString())
        {
            myOrder.type = jdata["Type"].toString();
        }
        if(jdata.contains("Rate") && jdata["Rate"].isDouble())
        {
            myOrder.rate = jdata["Rate"].toDouble();
        }
        if(jdata.contains("Amount") && jdata["Amount"].isDouble())
        {
            myOrder.amount = jdata["Amount"].toDouble();
        }
        if(jdata.contains("Total") && jdata["Total"].isDouble())
        {
            myOrder.total = jdata["Total"].toDouble();
        }
        if(jdata.contains("Remaining") && jdata["Remaining"].isDouble())
        {
            myOrder.remaining = jdata["Remaining"].toDouble();
        }
        if(jdata.contains("TimeStamp"))
        {
            myOrder.timestamp = jdata["TimeStamp"].toString();
        }
        myOrders.append(myOrder);
    }
}

void OpenOrdrers::applyChanges()
{
    int size = myOrders.size();
    if(size == 0)
    {
        qDebug() << "Нет элементов в QList<Currencies>";
        return;
    }
    insertRows(0, size, QModelIndex());
}

void OpenOrdrers::setNotifi(double price, double amount, int id)
{
    MyOrder ord;
    ord.rate = price;
    ord.amount = amount;
    ord.TradePairId = id;
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();

    if(stackNotifi.find(curTime) != stackNotifi.end())      // Если find вернул итератор не равный .end() значит ключ найден
        curTime++;
    stackNotifi.insert(curTime, ord);
//    qDebug() << "senNotifi.curTime=" << curTime;
}

QVector<int> OpenOrdrers::getOpenOrders()
{
    QVector<int> vec;
    for(int i = 0; i < myOrders.size(); i++)
    {
        vec.append(myOrders[i].orderId);
    }
    return vec;
}

void OpenOrdrers::handler()
{
    if(stackNotifi.empty())
        return;
    MyOrder ord;
    QMap<qint64, MyOrder>::const_iterator it = stackNotifi.constBegin();
    QVector <qint64> stack;
    qDebug() << "myOrder.size()=" << myOrders.size();
    qDebug() << "stackNotify.size()=" << stackNotifi.size();
    while(it != stackNotifi.end())
    {
        ord.rate = it.value().rate;
        ord.amount = it.value().amount;
        ord.TradePairId = it.value().TradePairId;
        qDebug() << "myOrder.size()=" << myOrders.size();
        for(int j=0; j<myOrders.size(); j++)
        {
//            qDebug() << "stackNotifiN_=" << it.key() << " rate=" << QString::number(ord.rate, 'f', 8) << " pairId=" << ord.TradePairId << " size=" << stackNotifi.size();
            if(qFuzzyCompare(myOrders[j].rate, ord.rate) && qFuzzyCompare(myOrders[j].amount, ord.amount) && myOrders[j].TradePairId == ord.TradePairId)
            {
                ord.orderId = myOrders[j].orderId;
                stack.append(it.key());
                emit sendNotifi(ord.rate, ord.amount, ord.TradePairId, ord.orderId);
//                stackNotifi.remove(it.key());
                qDebug() << "save for remove timeStamp=" << it.key();
//                stackNotifi.removeAt(i);
            }
        }
         ++it;
    }
    qDebug() << "Section remove save timeStamp";
    for(int i = 0; i < stack.size(); i++)
    {
        stackNotifi.remove(stack[i]);
        qDebug() << "remove one item";
    }
    //    for(int i=0; i< stackNotifi.size(); i++)
//    {
//        ord.rate = stackNotifi[i].rate;
//        ord.amount = stackNotifi[i].amount;
//        ord.TradePairId = stackNotifi[i].TradePairId;
//        for(int j=0; j<myOrders.size(); j++)
//        {
//            if(myOrders[j].rate == ord.rate && myOrders[j].amount == ord.amount && myOrders[j].TradePairId == ord.TradePairId)
//            {
//                ord.orderId = myOrders[j].orderId;
//                emit sendNotifi(ord.rate, ord.amount, ord.TradePairId, ord.orderId);
//                stackNotifi.removeAt(i);
//            }
//        }
//    }
}

QList<double> OpenOrdrers::getOpenOrdersId(int price)
{
    QList <double> prepar;
    for(int i = 0; i < myOrders.size(); i++)
        if(myOrders[i].TradePairId == price)
            prepar.append(myOrders[i].rate);
    return prepar;
}
