#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFontMetrics>
#include <QSize>


static const char* NAME_FILE_CONFIG = "config";
int indexwidget;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<QByteArray>();
    ui->setupUi(this);

    DrawWidget *drawWidget = new DrawWidget(this);
    indexwidget = ui->stackedWidget->addWidget(drawWidget);

    themes.insert(customTheme,this->styleSheet());
    themes.insert(darkTheme,"color: #929292; "
                              "selection-color: green;"
                              "selection-background-color: black;"
                              "background-color: #323232");
    on_themes_clicked();        // Приминить тему

    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(launchMarket()));


    network = new Network("network");
//    netthr = new QThread(this);
//    network->moveToThread(netthr);

//    QObject::connect(netthr, &QThread::finished, network, &Network::deleteLater, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::sendKey, network, &Network::receivKey, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::sendJson, network, &Network::receivJson, Qt::QueuedConnection);
    QObject::connect(network, &Network::sendMessage, this, &MainWindow::response, Qt::QueuedConnection);
    QObject::connect(network, &Network::sendCountRequest, this, &MainWindow::countRequest,  Qt::QueuedConnection);

//    QObject::connect(netthr, &QThread::started, network, &Network::doWork, Qt::QueuedConnection);
//    connect(network, SIGNAL(send(int)), this, SLOT(update(int)));
//    connect(this, &MainWindow::sendKey, network, &Network::receivKey, Qt::ConnectionType::DirectConnection );

//    netthr->start();

    if(loadConfig())
    {
        qDebug() << "configuration data is loaded";
    } else {
        qDebug() << "config data not load";
    }

    tableModel = new TableModel(this);
    sellOrderModel = new ModelOrders(this);
    bayOrderModel = new ModelOrders(this);
    modelHystory = new QStringListModel(this);
    modelBalance = new ModelBalance(this);
    ui->tableBalance->setModel(modelBalance);
    ui->listHystory->setModel(modelHystory);
    ui->tableViewDown->setModel(bayOrderModel);
    ui->tableViewUp->setModel(sellOrderModel);
    ui->tableViewDown->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewUp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBalance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBalance->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

//    ui->listCurrencies->setMaximumWidth(300);
//    ui->listCurrencies->setSortingEnabled(true);
    ui->listCurrencies->setModel(tableModel);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
//    ui->listCurrencies->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listCurrencies->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#323232 }");
//    ui->listCurrencies->verticalHeader()->setDefaultSectionSize(22);

    QObject::connect(ui->listCurrencies, SIGNAL(clicked(QModelIndex)), tableModel, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(ui->tableViewUp, SIGNAL(clicked(QModelIndex)), sellOrderModel, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(ui->tableViewDown, SIGNAL(clicked(QModelIndex)), bayOrderModel, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(tableModel, &TableModel::getMarket, this, &MainWindow::setMarket);
    QObject::connect(sellOrderModel, &ModelOrders::sendPrice ,this,&MainWindow::setPrice);
    QObject::connect(bayOrderModel, &ModelOrders::sendPrice ,this,&MainWindow::setPrice);

    on_getRequest_clicked();
}

MainWindow::~MainWindow()
{
//    delete network;

//    netthr->quit();
    delete ui;
}

QString MainWindow::printJsonValueType(QJsonValue type)
{
    switch(type.type())
    {
        case QJsonValue::Null: return "Null";
        case QJsonValue::Bool: return "Bool";
        case QJsonValue::Double: return "Double";
        case QJsonValue::String: return "String";
        case QJsonValue::Array: return "Array";
        case QJsonValue::Object: return "Object";
        case QJsonValue::Undefined: return "Undefined";
        default:    return "uncnown";
    }
}

void MainWindow::on_getRequest_clicked()
{
    // Получаем данные, а именно JSON файл с сайта по определённому url
//    networkManager->get(QNetworkRequest(QUrl("https://www.cryptopia.co.nz/api/GetCurrencies")));

    QJsonDocument json;
    QJsonObject obj;
    obj["GET"]=QString("GetMarkets");
    sendJson(obj);

}

void MainWindow::on_clearte_clicked()
{

}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::on_postRequest_clicked()
{

    QJsonDocument json;
    QJsonObject obj;
    obj["POST"]=QString("GetBalance");
    sendJson(obj);

}

bool MainWindow::loadConfig()
{
    if(QFile::exists(NAME_FILE_CONFIG))
    {
        configFile.setFileName(NAME_FILE_CONFIG);
        QDataStream stream(&configFile);
        configFile.open(QIODevice::ReadOnly);

        QString str;
        stream >> str;
        ui->leApiKey->setText(str);
        stream >> str;
        ui->leApiSecret->setText(str);
        configFile.close();

        network->API_KEY.append(ui->leApiKey->text());
        network->API_SECRET.append(ui->leApiSecret->text());
        return true;
    } else {
        qDebug() << "conf file not found";
        return false;
    }
}

void MainWindow::launchMarket()    //Вызывается в таймере
{
    //Тут сделать
    //проверку ответов с сервера, за определённую еденицу времени, возможно придётся вычислить максимальное отведённое время
    //запрос на историю сделок
    //запрос на открытые ордера
    //подсчёт всех объемов и вывод на панель
    //запрос на текущий курс и отправка его на отрисовку графика
//    qDebug() << "got is" << marketOption.Id;
    if(marketOption.timeoutMarkets == marketOption.tickMarkets)
    {
        on_getRequest_clicked();    //Markets
        marketOption.tickMarkets = 0;
    }
    if(marketOption.timeoutHystory == marketOption.tickHystory)
    {
        GetHystory(marketOption.Id);
        marketOption.tickHystory = 0;
    }
    if(marketOption.timeoutMarket == marketOption.tickMarket)
    {
        GetMarket(marketOption.Id);
        marketOption.tickMarket = 0;
    }
    if(marketOption.timeoutOrders == marketOption.tickOrders)
    {
        GetOrders(marketOption.Id);
        marketOption.tickOrders = 0;
    }
    marketOption.tickHystory++;
    marketOption.tickMarket++;
    marketOption.tickOrders++;
    marketOption.tickMarkets++;

}

void MainWindow::setMarket(int pairId)
{
    marketOption.Id = pairId;
    if(timer->isActive())
    {
        timer->stop();
    }
    marketOption.timeoutHystory = 8;
    marketOption.timeoutMarket = 2;     // Текущая котировка выбранной пары
    marketOption.timeoutMarkets = 20;   // Котировки всех пар
    marketOption.timeoutOrders = 5;



    marketOption.tickHystory = 0;
    marketOption.tickMarket = 0;
    marketOption.tickOrders = 0;
    marketOption.tickMarkets = 0;
    marketOption.msupdata = 700;
    timer->start(marketOption.msupdata);
}

void MainWindow::countRequest()
{
    countreq++;
    ui->countRequest->setText(QString::number(countreq));
}

void MainWindow::on_saveConfig_clicked()
{
    if(QFile::exists(NAME_FILE_CONFIG)){
        if(QFile::remove(NAME_FILE_CONFIG))
            qDebug() << "File remove success";
        else
            qDebug() << "file not remove";
    }
    configFile.setFileName(NAME_FILE_CONFIG);
    configFile.open(QIODevice::ReadWrite);
    QDataStream csFile(&configFile);
    if (configFile.isOpen()){
        csFile << ui->leApiKey->text();
        csFile << ui->leApiSecret->text();
        qDebug() << "configuration save success";
    } else {
        qDebug() << "file not open";
    }

    configFile.close();
}

void MainWindow::response(QJsonObject json)                                        // Парсинг ответа
{
//    qDebug() << "response: start parsing JSON";
    QElapsedTimer time;
    time.start();

    if(json.contains("Success") && json["Success"].isBool())
    {
        if(json["Success"].toBool())
        {
            ui->lbStat->setText("Успешно");
            ui->lbStat->setStyleSheet("background-color: #5EED00; font-size: 15px; font-weight: bold; color: #000000");
            ui->lbStat->setAlignment(Qt::AlignCenter);
        } else {
            qDebug() << "json response get not Success";
        }
    }
    if(json.contains("Message") )
    {
        if(printJsonValueType(json.value("Message"))=="Null")
        {
            ui->lbInsidMsg->setText("Сообщений нет");
            ui->lbInsidMsg->setStyleSheet("background-color: #828282; font-size: 15px; font-weight: bold; color: #000000");
            ui->lbInsidMsg->setAlignment(Qt::AlignCenter);
        } else {
            QString str = json["Message"].toString();
            ui->lbInsidMsg->setText(str);
            ui->lbInsidMsg->setStyleSheet("background-color: #FF0000; font-size: 15px; font-weight: bold; color: #000000");
            ui->lbInsidMsg->setAlignment(Qt::AlignCenter);
        }
    }
    if(json.contains("Error") && !json["Error"].isNull()){
        QString str = json["Error"].toString();
        ui->lbInsidMsg->setText(str);
        ui->lbInsidMsg->setStyleSheet("background-color: #FF0000; font-size: 15px; font-weight: bold; color: #000000");
        ui->lbInsidMsg->setAlignment(Qt::AlignCenter);
    }
    if(json.contains("Data") && json["Data"].isArray())     // *************** Если вложен МАССИВ *****************
    {
        QJsonArray jarray = json["Data"].toArray();
        qDebug() << "Array objects: " << jarray.count();
        if(jarray.empty())
            return;
        StateResponse state;
        QJsonObject stobj = jarray[0].toObject();
        if(stobj.contains("Type"))
        {
            state = StateGetHystory;
            if (!modelHystory->stringList().empty())
                    modelHystory->removeRows(0, modelHystory->stringList().count(), QModelIndex() );
        }
        else if(stobj.contains("BidPrice"))
        {
            state = StateGetMarket;
            if(!tableModel->currencies.isEmpty())
                tableModel->clearCurrencies();
        } else if(stobj.contains("Available"))
        {
            state = StateAvailable;
            if(!modelBalance->balances.isEmpty())
                modelBalance->clearBalance();
        }
        switch(state)
        {
            case StateGetHystory:                                                       //        ИСТОРИЯ
            {
                QStringList list;
                qint64 time;
                double price, amount, total;
                for(int i=0; i<jarray.size(); i++)
                {
                    QJsonObject jdata = jarray[i].toObject();
                    QString str;
                    if(jdata.contains("Timestamp")){
                        QDateTime timestamp;
                        time = jdata["Timestamp"].toInt();
                        timestamp.setTime_t(time);
                        str.append(timestamp.toString("yyyy.MM.dd hh:mm:ss"));
                    }
                    if (jdata.contains("Type")){
                        str.append("\t").append(jdata["Type"].toString());
                    }
                    if (jdata.contains("Price")){
                        price = jdata["Price"].toDouble();
                        str.append("\tPrice ").append(QString::number(price,'f',8));
                    }
                    if (jdata.contains("Amount")){
                        amount = jdata["Amount"].toDouble();
                        str.append("\tAmount ").append(QString::number(amount,'f',8));
                    }
                    if (jdata.contains("Total")){
                        total = jdata["Total"].toDouble();
                        str.append("\tTotal ").append(QString::number(total,'f',8));
                    }

                    hystory.addDeal(time, price, amount, total);
                    list << str;
                }
                modelHystory->setStringList(list);
                break;
            }
            case StateGetMarket:                                                        //          КОТИРОВКИ ПАР
            {
                for(int i=0; i<jarray.size(); i++)
                {
                    QJsonObject jdata = jarray[i].toObject();
                    TableModel::Currency curr;
                    if(jdata.contains("Label") && jdata["Label"].isString())
                    {
                        curr.label = jdata["Label"].toString();
                    }
                    if(jdata.contains("TradePairId") && jdata["TradePairId"].isDouble())
                    {
                        curr.Id = jdata["TradePairId"].toInt();
                    }
                    if(jdata.contains("LastPrice") && jdata["LastPrice"].isDouble())
                    {
                        curr.lastPrice = jdata["LastPrice"].toDouble();
//                        curr.filter = false;
                    }
                    tableModel->appendCurrency(curr);
                }
                break;
            }
            case StateAvailable:
            {
            for(int i=0; i<jarray.size(); i++)
            {
                QJsonObject jdata = jarray[i].toObject();
                ModelBalance::Balance bal;
                if(jdata.contains("CurrencyId") && jdata["CurrencyId"].isDouble())
                {
                    bal.Id = jdata["CurrencyId"].toInt();
                }
                if(jdata.contains("Symbol") && jdata["Symbol"].isString())
                {
                    bal.coin = jdata["Symbol"].toString();
                }
                if(jdata.contains("Total") && jdata["Total"].isDouble())
                {
                    bal.total = jdata["Total"].toDouble();
                }
                if(jdata.contains("Available") && jdata["Available"].isDouble())
                {
                    bal.available = jdata["Available"].toDouble();
                }
                if(jdata.contains("Unconfirmed") && jdata["Unconfirmed"].isDouble())
                {
                    bal.unconfirmed = jdata["Unconfirmed"].toDouble();
                }
                if(jdata.contains("HeldForTrades") && jdata["HeldForTrades"].isDouble())
                {
                    bal.heldForTrades = jdata["HeldForTrades"].toDouble();
                }
                if(jdata.contains("PendingWithdraw") && jdata["PendingWithdraw"].isDouble())
                {
                    bal.pendingWithdraw = jdata["PendingWithdraw"].toDouble();
                }
                if(jdata.contains("Address") && jdata["Address"].isString())
                {
                    bal.address = jdata["Address"].toString();
                }
                if(jdata.contains("BaseAddress") && jdata["BaseAddress"].isString())
                {
                    bal.baseAddress = jdata["BaseAddress"].toString();
                }
                if(jdata.contains("Status") && jdata["Status"].isString())
                {
                    bal.status = jdata["Status"].toString();
                }
                if(jdata.contains("StatusMessage") && jdata["StatusMessage"].isString())
                {
                    bal.statusMsg = jdata["StatusMessage"].toString();
                }
                modelBalance->appendBalance(bal);
            }
                break;
            }
            default:
            {
                QString error = "Не подходящего состояния для парсинга ответа с сервера ";
                ui->lbInsidMsg->setText(error);
                qDebug() << error;
                return;
            }
        }
    }
    if(json.contains("Data") && json["Data"].isObject())    // *************** Если вложен ОДИН объект *****************
    {
        QJsonObject jdata = json["Data"].toObject();
        if(jdata.contains("Buy") || (jdata.contains("Sell"))){
            if(!sellOrderModel->orders.isEmpty())
                sellOrderModel->clearOrders();
            if(!bayOrderModel->orders.isEmpty())
                bayOrderModel->clearOrders();
            if(jdata.contains("Buy")){                                       // Парсим список ордеров
                QJsonArray jarray = jdata["Buy"].toArray();
                if(jarray.empty())
                    return;
                for(int i=0; i<jarray.size(); i++)
                {
                    QJsonObject jobj = jarray[i].toObject();
                    ModelOrders::Order curr;
                    if(jobj.contains("Price") && jobj["Price"].isDouble())
                    {
                        curr.price = jobj["Price"].toDouble();
                    }
                    if(jobj.contains("Volume") && jobj["Volume"].isDouble())
                    {
                        curr.volume = jobj["Volume"].toDouble();
                    }
                    if(jobj.contains("Total") && jobj["Total"].isDouble())
                    {
                        curr.total = jobj["Total"].toDouble();
                    }
                    bayOrderModel->appendOrder(curr);
                    orders.addOrder(curr.price, curr.volume, curr.total, 0);
                }
            }
            if(jdata.contains("Sell")){
                QJsonArray jarray = jdata["Sell"].toArray();
                if(jarray.empty())
                    return;
                for(int i=jarray.size()-1; i>=0; i--)
                {
                    QJsonObject jobj = jarray[i].toObject();
                    ModelOrders::Order curr;
                    if(jobj.contains("Price") && jobj["Price"].isDouble())
                    {
                        curr.price = jobj["Price"].toDouble();
                    }
                    if(jobj.contains("Volume") && jobj["Volume"].isDouble())
                    {
                        curr.volume = jobj["Volume"].toDouble();
                    }
                    if(jobj.contains("Total") && jobj["Total"].isDouble())
                    {
                        curr.total = jobj["Total"].toDouble();
                    }
                    sellOrderModel->appendOrder(curr);
                    orders.addOrder(curr.price, curr.volume, curr.total, 1);
                }
                ui->tableViewUp->scrollToBottom();
            }
            // отобразить ордера
            updateOrders();
        } else {
//                QJsonObject jdata = json["Data"].toObject();
            qDebug() << "One object: " << jdata["Label"].toString();
            QString str = jdata["Label"].toString();
            str.append("\nlastPrice");
            str.append(QString::number(jdata["LastPrice"].toDouble(),'f',8));
            str.append("\nVolume");
            str.append(QString::number(jdata["Volume"].toDouble(),'f',8));

            ui->label_23->setStyleSheet("background-color: #828282; font-size: 15px; font-weight: bold; color: #000000");
            ui->label_23->setAlignment(Qt::AlignCenter);
            ui->label_23->setText(str);
        }
    }
    qDebug() << "Время парсинга " << time.nsecsElapsed();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(indexwidget);
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::GetMarket(int id)
{
    QJsonObject obj;
    obj["GET"]=QString("GetMarket");
    obj["par"]=id;
    sendJson(obj);
}

void MainWindow::GetHystory(int id)
{
    QJsonObject obj;
    obj["GET"]=QString("GetMarketHistory");
    obj["par"]=id;
    sendJson(obj);
}

void MainWindow::GetOrders(int id)
{
    QJsonObject obj;
    obj["GET"]=QString("GetMarketOrders");
    obj["par"]=id;
    sendJson(obj);
}

/***********************************************************************************
 ***                                 TableModel                                  **/

TableModel::TableModel(QObject *parent): QAbstractTableModel(parent)                // Торговые пары
{

}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();

    switch(role){
        case Qt::CheckStateRole:
            if (index.column()==filter)
                return currencies.at(index.row()).filter;
            break;
        case Qt::DisplayRole:{
            const TableModel::Currency &rec = currencies.at(index.row());
            int key = index.column();
            switch( key) {
                case TradePairId:
                    return rec.Id;
                case Label:
                    return rec.label;
                case LastPrice:
                    return QString::number(rec.lastPrice, 'f', 8);
            }
        }
        default:
            return QVariant();
    }
//     Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    return result;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::SizeHintRole){

    }
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Для любой роли, кроме запроса на отображение, прекращаем обработку
    if (role != Qt::DisplayRole)
    return QVariant();
    // формируем заголовки по номуру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case TradePairId:
            return tr("Id");
            case Label:
            return tr("Label");
            case LastPrice:
            return tr("LastPrice");
            case filter:
            return tr("X");
        }
    }
    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;

}

bool TableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        currencies.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void TableModel::appendCurrency(TableModel::Currency cur)
{
    beginInsertRows(QModelIndex(), 1, 1);
    currencies.append(cur);
    endInsertRows();
}

void TableModel::clearCurrencies()
{
    removeRows(0,currencies.count(),QModelIndex());
    currencies.clear();
}

void TableModel::selectedRow(QModelIndex index)
{
    if(index.column()==0){
        currencies[index.row()].filter ^= 1;
        emit dataChanged( index, index );
    } else {
        emit getMarket(this->currencies.at(index.row()).Id);

    }

}

void MainWindow::on_pushButton_8_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::paintEvent(QPaintEvent *)
{
}

void MainWindow::on_themes_clicked()
{
    switch(stateTheme){
    case darkTheme:
        stateTheme=customTheme;
        break;
    case customTheme:
        stateTheme=darkTheme;
        break;
    }
    this->setStyleSheet(themes.value(stateTheme));
}

DrawWidget::DrawWidget(QWidget *parent):
QWidget(parent)
{

}

void DrawWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", this->rect().height()/3));
    painter.drawText(this->rect(), Qt::AlignCenter, "Crypto bot");

}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(indexwidget);

}

void MainWindow::on_pushButton_11_clicked()             // Тестовая кнопка
{
//    GetMarket(1261);
    GetOrders(1261);
}


/***********************************************************************************
 ***                                 ModelOrders                                 **/

ModelOrders::ModelOrders(QObject *parent): QAbstractTableModel(parent)
{
}

QVariant ModelOrders::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();

    switch(role){
        case Qt::DisplayRole:
        {
            const Order &rec = orders.at(index.row());
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
//        case Qt::BackgroundRole:
//            return QColor(44, 214, 21, 10);
        default:
            return QVariant();
    }
//     Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    return result;
}

bool ModelOrders::setData(const QModelIndex &index, const QVariant &value, int role)
{

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

void ModelOrders::appendOrder(ModelOrders::Order ord)
{
    beginInsertRows(QModelIndex(), 1, 1);
    orders.append(ord);
    endInsertRows();
}

void ModelOrders::clearOrders()
{
    removeRows(0,orders.count(),QModelIndex());
    orders.clear();
}

void ModelOrders::selectedRow(QModelIndex index)
{
    qDebug() << "index row=" << index.row();
    emit sendPrice(orders.at(index.row()));
}

void MainWindow::on_HrzTblHdr_clicked()
{
    if(ui->HrzTblHdr->isChecked())
    {
//        ui->tableViewUp->horizontalHeader()->setVisible(true);
        ui->tableViewUp->horizontalHeader()->hide();
    } else {
        ui->tableViewUp->horizontalHeader()->show();
    }
    this->update();
}

void MainWindow::setPrice(ModelOrders::Order ord)
{
    ui->bayAmount->setValue(ord.volume);
    ui->bayPrice->setValue(ord.price);
    ui->baySum->setValue((ord.volume*ord.price));
    ui->bayFee->setValue(ord.volume*ord.price/500);
    ui->bayTotal->setValue(ui->baySum->value()+ui->bayFee->value());
    ui->sellAmount->setValue(ord.volume);
    ui->sellPrice->setValue(ord.price);
    ui->sellSum->setValue((ord.volume*ord.price));
    ui->sellFee->setValue(ord.volume*ord.price/500);
    ui->sellTotal->setValue(ui->baySum->value()+ui->bayFee->value());

}

void MainWindow::on_bayAmount_valueChanged(double arg1)
{
    double sum, fee, total, price=ui->bayPrice->value();
    sum = arg1*price;
    fee = sum/500;
    total = sum+fee;
    ui->baySum->setValue(sum);
    ui->bayFee->setValue(fee);
    ui->bayTotal->setValue(total);
}

void MainWindow::on_bayPrice_valueChanged(double arg1)
{
    double sum, fee, total, amount=ui->bayAmount->value();
    sum = arg1*amount;
    fee = sum/500;
    total = sum+fee;
    ui->baySum->setValue(sum);
    ui->bayFee->setValue(fee);
    ui->bayTotal->setValue(total);
}

void MainWindow::on_sellAmount_valueChanged(double arg1)
{
    double sum, fee, total, price=ui->sellPrice->value();
    sum = arg1*price;
    fee = sum/500;
    total = sum+fee;
    ui->sellSum->setValue(sum);
    ui->sellFee->setValue(fee);
    ui->sellTotal->setValue(total);
}

void MainWindow::on_sellPrice_valueChanged(double arg1)
{
    double sum, fee, total, amount=ui->sellAmount->value();
    sum = arg1*amount;
    fee = sum/500;
    total = sum+fee;
    ui->sellSum->setValue(sum);
    ui->sellFee->setValue(fee);
    ui->sellTotal->setValue(total);
}

ModelBalance::ModelBalance(QObject *parent): QAbstractTableModel(parent)
{

}

QVariant ModelBalance::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();
    switch(role){
        case Qt::DisplayRole:{
            const Balance &rec = balances.at(index.row());
            int key = index.column();
            switch( key) {
                case CurrencyId:
                    return QString::number(rec.Id);
                case Symbol:
                    return rec.coin;
                case Total:
                    return QString::number(rec.total, 'f', 8);
                case Available:
                    return QString::number(rec.available, 'f', 8);
                case Unconfirmed:
                    return QString::number(rec.unconfirmed, 'f', 8);
                case HeldForTrades:
                    return QString::number(rec.heldForTrades, 'f', 8);
                case PendingWithdraw:
                    return QString::number(rec.pendingWithdraw, 'f', 8);
                case Address:
                    return rec.address;
                case BaseAddress:
                    return rec.baseAddress;
                case Status:
                    return rec.status;
                case StatusMessage:
                    return rec.statusMsg;
            }
        }
        default:
            return QVariant();
    }
    return result;
}

/***********************************************************************************
 ***                                ModelBalance                                 **/


bool ModelBalance::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

QVariant ModelBalance::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Для любой роли, кроме запроса на отображение, прекращаем обработку
    if (role != Qt::DisplayRole)
    return QVariant();
    // формируем заголовки по номеру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case CurrencyId:
                return tr("Id");
            case Symbol:
                return tr("Coin");
            case Total:
                return tr("Total");
            case Available:
                return tr("Available");
            case Unconfirmed:
                return tr("Unconfirmed");
            case HeldForTrades:
                return tr("HeldForTrades");
            case PendingWithdraw:
                return tr("PendingWithdraw");
            case Address:
                return tr("Address");
            case BaseAddress:
                return tr("BaseAddress");
            case Status:
                return tr("Status");
            case StatusMessage:
                return tr("StatusMessage");
        }
    }
    return QVariant();
}

Qt::ItemFlags ModelBalance::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ModelBalance::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row = 0; row < rows; ++row) {
        balances.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void ModelBalance::appendBalance(ModelBalance::Balance bc)
{
    beginInsertRows(QModelIndex(), 1, 1);
    balances.append(bc);
    endInsertRows();
}

void ModelBalance::clearBalance()
{
    removeRows(0,balances.count(),QModelIndex());
    balances.clear();
}

void ModelBalance::selectedRow(QModelIndex index)
{
    qDebug() << "index row=" << index.row();
}

void MainWindow::on_tableBalance_clicked(const QModelIndex &index)
{
    qDebug() << "index row=" << index.row();
}

void MainWindow::on_pushButton_12_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_13_clicked()
{
    GetHystory(1261);
}

void MainWindow::on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    hystory.getVolume(dateTime.toSecsSinceEpoch(), ui->dateTimeEdit_2->dateTime().toSecsSinceEpoch());
}

void MainWindow::updateOrders()
{

//    bayOrderModel->appendOrder();

}
