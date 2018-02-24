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

//    grScene = new QGraphicsScene(QRectF(-100, -100, 300, 300));
//    QGraphicsScene scene(QRectF(-100, -100, 300, 300));
    drawWidget = new DrawWidget(this);
//    drawWidget->setScene(&scene);

//    QGraphicsRectItem* pRectItem = new QGraphicsRectItem(0, &scene);
//    pRectItem->setPen(QPen(Qt::black));
//    pRectItem->setBrush(QBrush(Qt::green));
//    pRectItem->setRect(QRectF(-30, -30, 120, 80));
//    pRectItem->setFlags(QGraphicsItem::ItemIsMovable);

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
    QObject::connect(network, &Network::sendMessage, this, &MainWindow::getResponse, Qt::QueuedConnection);
    QObject::connect(network, &Network::sendMessagePOST, this, &MainWindow::postResponse, Qt::QueuedConnection);
    QObject::connect(network, &Network::sendCountRequest, this, &MainWindow::countRequest,  Qt::QueuedConnection);

//    QObject::connect(netthr, &QThread::started, network, &Network::doWork, Qt::QueuedConnection);
//    connect(network, SIGNAL(send(int)), this, SLOT(update(int)));
//    connect(this, &MainWindow::sendKey, network, &Network::receivKey, Qt::ConnectionType::DirectConnection );

//    netthr->start();  `


    modelPairs = new ModelPairs(this);
    sellOrderModel = new ModelOrders(this, "Sell");
    ui->tableViewUp->setModel(sellOrderModel);
    bayOrderModel = new ModelOrders(this, "Bay");
    ui->tableViewDown->setModel(bayOrderModel);
    modelHystory = new QStringListModel(this);
    ui->listHystory->setModel(modelHystory);
    modelBalance = new ModelBalance(this);
    ui->tableBalance->setModel(modelBalance);
    openOrdrers = new OpenOrdrers(this);

    ui->tableOrders->setModel(openOrdrers);
//    ui->tableOrders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);                // Сжать или растянуть ячейки на видемой области отмен
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);    // Ячейка по размеру информации
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ui->tableOrders->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    ui->tableOrders->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableOrders, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));


    ui->tableViewDown->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewUp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBalance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableBalance->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    if(loadConfig())
    {
        qDebug() << "configuration data is loaded";
    } else {
        qDebug() << "config data not load";
    }

//    ui->listCurrencies->setMaximumWidth(300);
//    ui->listCurrencies->setSortingEnabled(true);
    ui->listCurrencies->setModel(modelPairs);
//    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
//    ui->listCurrencies->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listCurrencies->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#323232 }");
//    ui->listCurrencies->verticalHeader()->setDefaultSectionSize(22);

    QObject::connect(ui->listCurrencies, SIGNAL(clicked(QModelIndex)), modelPairs, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(ui->tableViewUp, SIGNAL(clicked(QModelIndex)), sellOrderModel, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(ui->tableViewDown, SIGNAL(clicked(QModelIndex)), bayOrderModel, SLOT(selectedRow(QModelIndex)) );
    QObject::connect(modelPairs, &ModelPairs::getMarket, this, &MainWindow::setMarket);
    QObject::connect(sellOrderModel, &ModelOrders::sendPrice ,this,&MainWindow::setPrice);
    QObject::connect(bayOrderModel, &ModelOrders::sendPrice ,this,&MainWindow::setPrice);
//    QObject::connect(sellOrderModel, &ModelOrders::clearSelection, this, &MainWindow::clearSelectOrder);
//    QObject::connect(bayOrderModel, &ModelOrders::clearSelection, this, &MainWindow::clearSelectOrder);

//    on_getRequest_clicked();
    on_filterPairs_clicked();

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

//    QJsonDocument json;
    QJsonObject obj;
    obj["GET"]=QString("GetMarkets");
    sendJson(obj);

}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::on_postRequest_clicked()
{
//    QJsonDocument json;
    QJsonObject obj;
    obj["POST"]=QString("GetOpenOrders");
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
        stream >> modelPairs->favoriteCoin;
        configFile.close();

        network->API_KEY.append(ui->leApiKey->text());
        network->API_SECRET.append(ui->leApiSecret->text());

        return true;
    } else {
        qDebug() << "conf file not found";
        return false;
    }
}

void MainWindow::launchMarket()                                                 //Вызывается в таймере
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

void MainWindow::customMenuRequested(QPoint pos)
{
    qDebug() << "pos=" << pos.x() << " " << pos.y();
    /* Создаем объект контекстного меню */
    QMenu * menu = new QMenu(this);
    /* Создаём действия для контекстного меню */
    QAction * editDevice = new QAction(trUtf8("Редактировать"), this);
    QAction * deleteOrder = new QAction(trUtf8("Удалить"), this);
    /* Подключаем СЛОТы обработчики для действий контекстного меню */
//    connect(editDevice, SIGNAL(triggered()), this, SLOT(slotEditRecord()));     // Обработчик вызова диалога редактирования
    connect(deleteOrder, SIGNAL(triggered()), this, SLOT(romoveOrder())); // Обработчик удаления записи
    /* Устанавливаем действия в меню */
    menu->addAction(editDevice);
    menu->addAction(deleteOrder);
    /* Вызываем контекстное меню */
    menu->popup(ui->tableOrders->viewport()->mapToGlobal(pos));
}

void MainWindow::romoveOrder()
{
    int rm = ui->tableOrders->selectionModel()->currentIndex().row();
    QJsonObject obj;
    obj["POST"]=QString("CancelTrade");
    obj["Type"]=QString("Trade");
    obj["OrderId"]=rm;
    sendJson(obj);
}

//void MainWindow::clearSelectOrder(ModelOrders::Type type)
//{
//    switch(type){
//    case    ModelOrders::sell:
//        ui->tableViewDown->clearSelection();
//        break;
//    case    ModelOrders::bay:
//        ui->tableViewUp->clearSelection();
//        break;
//    case ModelOrders::non:
//        qDebug() << "void MainWindow::clearSelectOrder(ModelOrders::Type type); Получен non";
//        break;
//    }
//}

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
        csFile << modelPairs->favoriteCoin;
        qDebug() << "configuration save success";
    } else {
        qDebug() << "file not open";
    }

    configFile.close();
}

void MainWindow::getResponse(QJsonObject json)                                        // Парсинг ответа
{
//    qDebug() << "response: start parsing JSON";
    QElapsedTimer time;
    time.start();

    if(json.contains("Success") && json["Success"].isBool())
    {
        if(json["Success"].toBool())
        {
            ui->lbStat->setText("Success");
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
            ui->lbInsidMsg->setText("no messages");
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
            if(!modelPairs->currencies.isEmpty())
                modelPairs->clearCurrencies();
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
            case StateGetMarket:                                            //          КОТИРОВКИ ПАР
            {
                modelPairs->setNewPairs(jarray);
                break;
            }
            case StateAvailable:                                            //          БАЛАНС
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
                QString error = "Не подходящего состояния для парсинга ответа GET с сервера ";
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
            if(jdata.contains("Buy")){                                       // Парсим список ордеров
                QJsonArray jarray = jdata["Buy"].toArray();
                if(jarray.empty())
                    return;
                bayOrderModel->appendOrders(jarray);
                if(bayOrderModel->saveIndex.isValid())
                {
                    ui->tableViewDown->selectRow(bayOrderModel->saveIndex.row());
                } else {
                    ui->tableViewDown->scrollToTop();
                }
            }
            if(jdata.contains("Sell")){
                QJsonArray jarray = jdata["Sell"].toArray();
                if(jarray.empty())
                    return;
                sellOrderModel->appendOrders(jarray);
                if(!sellOrderModel->saveIndex.isValid())
                {
                    ui->tableViewUp->scrollToBottom();
                }
                else {
                    ui->tableViewUp->selectRow(sellOrderModel->saveIndex.row());
                }
            }
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
    //    qDebug() << "Время парсинга " << time.nsecsElapsed();
}

void MainWindow::postResponse(QJsonObject json)
{
    //    qDebug() << "response: start parsing JSON";
        QElapsedTimer time;
        time.start();
        if(json.contains("Success") && json["Success"].isBool())
        {
            if(json["Success"].toBool())
            {
                ui->lbStat->setText("Success");
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
                ui->lbInsidMsg->setText("no messages");
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
            if(stobj.contains("Available"))
            {
                state = StateAvailable;
                if(!modelBalance->balances.isEmpty())
                    modelBalance->clearBalance();
            } else if (stobj.contains("OrderId"))
            {
                state = OrderId;
            }
            switch(state)
            {
                case StateAvailable:                                            //          БАЛАНС
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
                case OrderId:
                {
                    openOrdrers->setNewOpenOrders(jarray);
                    break;
                }
                default:
                {
                    QString error = "Не подходящего состояния для парсинга ответа POST с сервера ";
                    ui->lbInsidMsg->setText(error);
                    qDebug() << error;
                    return;
                }
            }
        }
        if(json.contains("Data") && json["Data"].isObject())    // *************** Если вложен ОДИН объект *****************
        {
            QJsonObject jdata = json["Data"].toObject();
            qDebug() << "Ответ с сервера на POST запрос >> вложен ОДИН объект";
        }
        //    qDebug() << "Время парсинга " << time.nsecsElapsed();
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

void MainWindow::on_pushButton_8_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
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

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(indexwidget);

}

void MainWindow::on_pushButton_11_clicked()                                          // Тестовая кнопка
{
//    GetMarket(1261);
    // ETN/BTC 5662
    // ETH/BTC 5203
    // BCH/BTC 5355
    GetOrders(5355);
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

void MainWindow::setPrice(double price, double amount)
{
    ui->bayAmount->setValue(amount);
    ui->bayPrice->setValue(price);
    ui->baySum->setValue(amount*price);
    ui->bayFee->setValue(amount*price/500);
    ui->bayTotal->setValue(ui->baySum->value()+ui->bayFee->value());
    ui->sellAmount->setValue(amount);
    ui->sellPrice->setValue(price);
    ui->sellSum->setValue(amount*price);
    ui->sellFee->setValue(amount*price/500);
    ui->sellTotal->setValue(ui->baySum->value()-ui->bayFee->value());

}

void MainWindow::on_bayAmount_valueChanged(double arg1)
{
    if(stateCalc == stateCalculation)
        return;
    double sum, fee, total, price=ui->bayPrice->value();
    sum = arg1*price;
    fee = sum/500;
    total = sum+fee;
    stateCalc = stateCalculation;
    ui->baySum->setValue(sum);
    ui->bayFee->setValue(fee);
    ui->bayTotal->setValue(total);
    stateCalc = stateWait;
}

void MainWindow::on_bayPrice_valueChanged(double arg1)
{
    if(stateCalc == stateCalculation)
        return;
    double sum, fee, total, amount=ui->bayAmount->value();
    sum = arg1*amount;
    fee = sum/500;
    total = sum+fee;
    stateCalc = stateCalculation;
    ui->baySum->setValue(sum);
    ui->bayFee->setValue(fee);
    ui->bayTotal->setValue(total);
    stateCalc = stateWait;
}

void MainWindow::on_sellAmount_valueChanged(double arg1)
{
    if(stateCalc == stateCalculation)
        return;
    double sum, fee, total, price=ui->sellPrice->value();
    sum = arg1*price;
    fee = sum/500;
    total = sum-fee;
    stateCalc = stateCalculation;
    ui->sellSum->setValue(sum);
    ui->sellFee->setValue(fee);
    ui->sellTotal->setValue(total);
    stateCalc = stateWait;
}

void MainWindow::on_sellPrice_valueChanged(double arg1)
{
    double sum, fee, total, amount=ui->sellAmount->value();
    sum = arg1*amount;
    fee = sum/500;
    total = sum-fee;
    ui->sellSum->setValue(sum);
    ui->sellFee->setValue(fee);
    ui->sellTotal->setValue(total);
}

ModelBalance::ModelBalance(QObject *parent): QAbstractTableModel(parent)
{

}

int ModelBalance::rowCount(const QModelIndex &) const {return balances.count();}

int ModelBalance::columnCount(const QModelIndex &) const {return COLUMN;}

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

void MainWindow::on_pushButton_12_clicked()         // Узнать баланс
{
    ui->stackedWidget_2->setCurrentIndex(3);
}

void MainWindow::on_pushButton_13_clicked()
{
    GetHystory(1261);
}

void MainWindow::on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    hystory.getVolume(dateTime.toSecsSinceEpoch(), ui->dateTimeEdit_2->dateTime().toSecsSinceEpoch());
}

void MainWindow::on_sbround_valueChanged(int arg1)
{
    if(arg1 == 0){
        bayOrderModel->filter = 1;
        return;
    }
    int i = 10;
    for(int j=1; j<arg1; ++j)
        i *=10;
    bayOrderModel->filter = i;
    sellOrderModel->filter = i;
    bayOrderModel->checkFilter();
    sellOrderModel->checkFilter();
}

void MainWindow::on_filterPairs_clicked()
{
    if(ui->filterPairs->isChecked())
    {
        modelPairs->chkfiltr = true;
    } else {
        modelPairs->chkfiltr = false;
    }
    modelPairs->checkFilter();
}

void MainWindow::on_chkbksFindProfit_clicked()
{
    if(ui->chkbksFindProfit->isChecked())
    {
        modelPairs->chkFindProfit = true;
    } else {
        modelPairs->chkFindProfit = false;
    }
    modelPairs->checkFilter();
}


void MainWindow::on_pushButton_14_clicked()
{
    sellOrderModel->applyChange();
}

void MainWindow::on_bayTotal_valueChanged(double arg1)
{
    if(stateCalc == stateCalculation)
        return;
    double price = ui->bayPrice->value();
    if(qFuzzyIsNull(price))
        return;
    if(qFuzzyIsNull(arg1))
        return;
    double fee, sum, total = arg1;
    sum = total/(1+1./500.);
    fee = sum/500.;
    stateCalc = stateCalculation;
    ui->bayAmount->setValue(sum/price);
    ui->bayFee->setValue(fee);
    ui->baySum->setValue(sum);
    stateCalc = stateWait;
}

void MainWindow::on_sellTotal_valueChanged(double arg1)
{
    if(stateCalc == stateCalculation)
        return;
    double price = ui->sellPrice->value();
    if(qFuzzyIsNull(price))
        return;
    if(qFuzzyIsNull(arg1))
        return;
    double fee, sum, total = arg1;
//    qDebug() << "price=" <<  QString::number(price, 'f', 8);
    sum = total/(1-1./500.);
//    qDebug() << "sum" << QString::number(sum, 'f', 8);
    fee = sum/500;
//    qDebug() << "fee" << QString::number(fee, 'f', 8);
    stateCalc = stateCalculation;
    ui->sellAmount->setValue(sum/price);
    ui->sellFee->setValue(fee);
    ui->sellSum->setValue(sum);
    stateCalc = stateWait;
}

void MainWindow::on_pushButton_15_clicked()
{
    sellOrderModel->updateCell();
}

void MainWindow::on_pushButton_16_clicked()
{
    QJsonObject obj;
    obj["POST"]=QString("GetBalance");
    sendJson(obj);
}

void MainWindow::on_pushButton_17_clicked()
{
    QJsonObject obj;
    obj["POST"]=QString("GetOpenOrders");
    sendJson(obj);
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_18_clicked()
{
    QJsonObject obj;
    obj["POST"]=QString("GetOpenOrders");
    sendJson(obj);
}
