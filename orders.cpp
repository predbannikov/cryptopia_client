#include "orders.h"
#include <QtMath>

/***********************************************************************************
 ***                                 ModelOrders                                 **/

ModelOrders::SelectOrder ModelOrders::selOrder;


ModelOrders::ModelOrders(QObject *parent, QString name): QAbstractTableModel(parent)
{
    if(name == "Sell")
        type = sell;
    if(name == "Bay")
        type = bay;

}

int ModelOrders::rowCount(const QModelIndex &) const
{
    return orders.count();
}

int ModelOrders::columnCount(const QModelIndex &) const
{
    return COLUMN;
}

QVariant ModelOrders::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();
    const Order &rec = orders.at(index.row());

    switch(role){
        case Qt::DisplayRole:
        {
            int key = index.column();
            switch( key) {
                case Price:
                    return QString::number(rec.price, 'f', 8);
                case Volume:
                    return QString::number(rec.volume, 'f', 8);
                case Total:
                    return QString::number(rec.total, 'f', 8);
                }
        }
//        case Qt::TextColorRole:
//            return QColor();
        case Qt::BackgroundRole:
//            qDebug() << "row = " << index.row() << " column = " << index.column();
            if(saveIndex.isValid() )   // рисуем выделение
            {
//                qDebug() << "sell.start.row = " << selOrder.indexStart.row() << " sell.stop.row = " << selOrder.indexStop.row();
                    if(saveIndex.row() == index.row())
                        return QColor(0, 0, 255, 100);
            }
            if(type == bay)
            {
                return QColor(0, 255, 0, rec.intensColor);
            }
            else if(type == sell)
            {
                return QColor(255, 0, 0, rec.intensColor);
            }
        case Qt::DecorationRole:{
//            this->parent->myOrder.size();
            if(index.column() == 0 && openOrders.contains(rec.price))
                return QIcon("galka.png");

        }
    default:
            return QVariant();
    }
//     Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    return result;
}

bool ModelOrders::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
//    emit dataChanged(createIndex(selOrder.indexStart.row(),0), selOrder.indexStop);
//    return true;
}

QVariant ModelOrders::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Для любой роли, кроме запроса на отображение, прекращаем обработку
    if (role != Qt::DisplayRole)
    return QVariant();
    // формируем заголовки по номуру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case Price:
            return tr("Price");
            case Volume:
            return tr("Volume");
            case Total:
            return tr("Total");
        }
    }
    return QVariant();
}

Qt::ItemFlags ModelOrders::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ModelOrders::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        orders.removeAt(position);
    }
    endRemoveRows();
    return true;
}

bool ModelOrders::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
//    for (int i = 0; i < count; ++i)
//        zipcodes.insert(row, ZipcodeItem());
    endInsertRows();
    return true;
}

void ModelOrders::appendOrders(QJsonArray array)
{
    sourceOrd.clear();
    for(int i=0; i<array.size(); i++)
    {
        QJsonObject jobj = array[i].toObject();
        Order curr;
        if(jobj.contains("Volume") && jobj["Volume"].isDouble())
        {
            curr.volume = jobj["Volume"].toDouble();
        }
        if(jobj.contains("Total") && jobj["Total"].isDouble())
        {
            curr.total = jobj["Total"].toDouble();
        }
        if(jobj.contains("Price") && jobj["Price"].isDouble())
        {
            curr.price = jobj["Price"].toDouble();
        }
        if(jobj.contains("TradePairId") && jobj["TradePairId"].isDouble())
        {
            curr.Id = jobj["TradePairId"].toInt();
        }
        sourceOrd.append(curr);
    }
    checkFilter();
}

void ModelOrders::clearOrders()
{
    if(orders.empty())
        return;
    removeRows(0,orders.count(),QModelIndex());
    orders.clear();
//    sourceOrd.clear();
}

void ModelOrders::checkFilter()
{
    clearOrders();
//    orders.clear();
    if(filter == 0)
        qDebug() << "void ModelOrders::checkFilter(); if(filter == 0) Фильтер не должен быть равено нулю";
    else {
        Order temp;
        temp.price = 0;         // Последний прайс, с которым делаем сверку фильтрации
        temp.Id = sourceOrd[0].Id;
        for(int i=sourceOrd.size()-1; i>=0 ; --i)
        {
            if (qFuzzyIsNull(temp.price))                        // Получаем не нулевой прайс
            {
                while(temp.price == 0)
                {
                    temp.price = qFloor(sourceOrd[i].price * filter + 0.5)/filter;
                    if(qFuzzyIsNull(temp.price))
                        filter*=10;
//                    qDebug() << QString::number(filter,'f',8);;
                }
//                qDebug() << QString::number(temp.price,'f',8);
                temp.volume = sourceOrd[i].volume;
                temp.total = sourceOrd[i].total;
            }
            else {
                double roundPrice = floor(sourceOrd[i].price * filter + 0.5)/filter;
                if(qFuzzyCompare(roundPrice,temp.price))
                {
//                    qDebug() << "update amount " << QString::number(roundPrice,'f',8);
                    temp.price = floor(sourceOrd[i].price * filter + 0.5)/filter;
                    temp.volume += sourceOrd[i].volume;
                    temp.total += sourceOrd[i].total;

                }
                else {
                    if(type == bay)
                        orders.prepend(temp);
                    if(type == sell)
                        orders.append(temp);
                    temp.price = floor(sourceOrd[i].price * filter + 0.5)/filter;
                    temp.volume = sourceOrd[i].volume;
                    temp.total = sourceOrd[i].total;
                }
            }
        }
        // Добавим последний элемент temp в список
        if(type == bay)
            orders.prepend(temp);
        if(type == sell)
            orders.append(temp);
//        qDebug() << "sourceOrd.size=" << sourceOrd.size() << " orders.size=" << orders.size();
    }
    int countord = orders.size();
    if(countord == 0 )
    {
        qDebug() << "Нет элементов в QList<Order>";
        return;
    }
//    checkLevel();
    checkColor();
    insertRows(0, countord, QModelIndex());
}

void ModelOrders::checkLevel()
{
    QMap <double, int> sort;
    for(int i=0; i<orders.size(); i++)
    {
        sort.insert(orders[i].volume, i);
    }
    QMap<double, int>::const_iterator startIt;
    QMap<double, int>::const_iterator stopIt;
    startIt = sort.begin();
    stopIt = sort.end();
    int i = 0;
    while(startIt != stopIt)
    {
        orders[startIt.value()].level = i;
        startIt++;
        i++;
    }
//    for(int i=0; i<orders.size(); i++)
//        qDebug() << "volume=" << orders[i].volume << " level=" << orders[i].level;
}

void ModelOrders::checkColor()
{
    double max = 0, min = 0;
    for(int i = 0; i < orders.size(); i++)
        if(max < orders[i].volume)
            max = orders[i].volume;
//    qDebug() << "max=" << max;
    min = 255/max;
    for(int i = 0; i < orders.size(); i++){
        orders[i].intensColor=(int)(orders[i].volume*min);
//        qDebug() << "orders[i].intensColor" << orders[i].intensColor;
    }
}

void ModelOrders::checkSelect()
{

}

void ModelOrders::applyChange()
{
    //    emit dataChanged(saveIndex, saveIndex2);
}

void ModelOrders::mysuperfunk()
{

}

void ModelOrders::updateCell()
{
    emit dataChanged(createIndex(1,1), createIndex(2,2));
}

void ModelOrders::getNotifyOpenOrders(QVector <double> ord)
{
    openOrders = ord;
}

void ModelOrders::appendOrder(ModelOrders::Order ord)
{

}

void ModelOrders::selectedRow(QModelIndex index)
{
//    qDebug() << "index row=" << index.row();
    Order ord = orders.at(index.row());
    emit sendPrice(ord.price, ord.volume, ord.Id);      // Отправляем цену и количество монет на покупку в окно покупок и продаж
    if(saveIndex.row() == index.row())
    {
        emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
        saveIndex = createIndex(-1,-1);
    }
    else
    {
        if(!saveIndex.isValid())
        {
            saveIndex = index;
            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
        } else {
            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
            saveIndex = index;
            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
        }
    }
//    double price, volume, sum, fee, total;
//    price = ord.price;
//    volume = ord.volume;


//                        // Расчёт, где мы можем начать отбивать прибыль
//    sum = price * volume;
//    fee = sum/500;
//    double x, y1, y2, s=0;
//    double d = 0.00000001;
//    int it = 1;
//    while(price/it > 0.1)
//        it *=10;
//    d *= it;
//    if(type == sell)
//    {
//        total = sum - fee;
//        x=price;
//        y1 = x*volume;
//        y2 = y1/500;
//        s = y1 + y2;
//        for(x = price; s > total; x-=d)
//        {
//            y1 = x*volume;
//            y2 = y1/500;
//            s = y1 + y2;
//        }

////        qDebug() << "startProfit id=" << QString::number(selOrder.startProfit,'f',8);
//        selOrder.startProfit = x;
//        selOrder.type = sell;
//        selOrder.selectPrice = price;
//        selOrder.indexStart = index;
//        int i = orders.size() - 1;
//        while(orders[i].price < x)
//            i--;
//        selOrder.indexStop = createIndex(i, COLUMN);
//    } else if( type == bay)
//    {
//        total = sum + fee;
//        x=price;
//        y1 = x*volume;
//        y2 = y1/500;
//        s = y1 - y2;
//        for(x = price; s < total; x+=d)
//        {
//            y1 = x*volume;
//            y2 = y1/500;
//            s = y1 - y2;
//        }
////        qDebug() << "startProfit id=" << QString::number(selOrder.indexStop,'f',8);
//        selOrder.startProfit = x;
//        selOrder.type = bay;
//        selOrder.selectPrice = price;

//    }

//    if(selOrder.indexStart.row() == index.row())
//    {
//        emit dataChanged(createIndex(selOrder.indexStart.row(),0), selOrder.indexStop);
//        qDebug() << "IndexStart.row() = " << selOrder.indexStart.row() << " indexStop.row() = " << selOrder.indexStop.row();
//        selOrder.indexStart = createIndex(-1,-1);
//        selOrder.indexStop = createIndex(-1,-1);
//        selOrder.type = non;
//    }
//    else
//    {
//        if(!saveIndex.isValid())
//        {
//            saveIndex = index;
//            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
//        } else {
//            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
//            saveIndex = index;
//            emit dataChanged(createIndex(saveIndex.row(),0), createIndex(saveIndex.row(), COLUMN));
//        }
//    }
//    qDebug() << "profit: " << "x=" << QString::number(x,'f',8) << " s=" << QString::number(s,'f',8) << ":" << QString::number(total,'f',8);
//    emit clearSelection(type);

}
