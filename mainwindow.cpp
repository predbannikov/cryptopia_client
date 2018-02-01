#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFontMetrics>
#include <QSize>


static const char* NAME_FILE_CONFIG = "config";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<QByteArray>();

    ui->setupUi(this);

    network = new Network("network");
    netthr = new QThread(this);
    network->moveToThread(netthr);

    QObject::connect(netthr, &QThread::finished, network, &Network::deleteLater, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::sendKey, network, &Network::receivKey, Qt::QueuedConnection);
    QObject::connect(this, &MainWindow::sendJson, network, &Network::receivJson, Qt::QueuedConnection);
    QObject::connect(network, &Network::sendMessage, this, &MainWindow::response, Qt::QueuedConnection);

//    QObject::connect(netthr, &QThread::started, network, &Network::doWork, Qt::QueuedConnection);
//    connect(network, SIGNAL(send(int)), this, SLOT(update(int)));
//    connect(this, &MainWindow::sendKey, network, &Network::receivKey, Qt::ConnectionType::DirectConnection );

    netthr->start();

    if(loadConfig())
    {
        qDebug() << "configuration data is loaded";
    } else {
        qDebug() << "config data not load";
    }

    this->setStyleSheet("color: #929292; "
                        "selection-color: green;"
                        "selection-background-color: black;"
                        "background-color: #323232");

    tableModel = new TableModel(this);
    ui->listCurrencies->setMaximumWidth(300);
    ui->listCurrencies->setSortingEnabled(true);
    ui->listCurrencies->setModel(tableModel);
    selection.setModel(tableModel);
    ui->listCurrencies->setSelectionModel(&selection);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->listCurrencies->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->listCurrencies->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#323232 }");
    ui->listCurrencies->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listCurrencies->verticalHeader()->setDefaultSectionSize(22);
    QObject::connect(ui->listCurrencies, SIGNAL(clicked(QModelIndex)), tableModel, SLOT(replaceCheck(QModelIndex)) );
}

MainWindow::~MainWindow()
{
//    delete network;

    netthr->quit();
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
    ui->teData->clear();
}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::on_postRequest_clicked()
{

        int id = (int)QThread::currentThreadId();
        qDebug() << "mainwindow: " << QString::number(id);

        emit sendKey(QByteArray());


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
//        QByteArray keys[2];

//        keys[0].append(ui->leApiKey->text());
//        keys[1].append(ui->leApiSecret->text());
        QByteArray keys;
        emit sendKey(keys);
        return true;
    } else {
        qDebug() << "conf file not found";
        return false;
    }
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

void MainWindow::response(QJsonObject json)
{
    QStringList strstat;
//            strstat.append(QString::number(root.size(), 10));
    strstat.append(json.keys());
    QString tmp;
    qDebug() << "start";
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
        if(json.contains("Data") && json["Data"].isArray())
        {
            QJsonArray jarray = json["Data"].toArray();
            ui->teData->append("Колличество элементов: ");
            ui->teData->append(QString::number(jarray.size()));
            for(int i=0; i<jarray.size(); i++)
            {
//                Currency currency;
                QJsonObject jdata = jarray[i].toObject();
                TableModel::Currency curr;
//                if(jdata.contains("Name") && jdata["Name"].isString())
//                {
//                    QString tmp = "Монета: ";
//                    tmp.append(jdata["Name"].toString());
//                    ui->teData->append(tmp);
//                }
                if(jdata.contains("Label") && jdata["Label"].isString())
                {
                    curr.label = jdata["Label"].toString();
//                    ui->teData->append(jdata["Label"].toString());
                }
                if(jdata.contains("TradePairId") && jdata["TradePairId"].isDouble())
                {
                    curr.Id = jdata["TradePairId"].toInt();
//                    tableModel->setData( currency.insert(TradePairId, QString::number(jdata["TradePairId"].toInt()));
                }
                if(jdata.contains("LastPrice") && jdata["LastPrice"].isDouble())
                {
                    curr.lastPrice = jdata["LastPrice"].toDouble();
                    curr.filter = false;
                }
//                currencies.append(currency);
//                if(jdata.contains("") && jdata[""].is)
//                {

//                }
                tableModel->appendCurrency(curr);
            }
//            ui->listCurrencies->setModel();
        }
        tmp.append(printJsonValueType(json.value(tmp)));
        ui->teData->append(tmp);
//                ui->teData->append("\n");
    strstat << "\n";
    // Второе значение пропишем строкой
    QJsonValue jfirst = json.value("Success");
    if (jfirst.isBool())
        ui->lbSuccess->setText("true");
    else
        ui->lbSuccess->setText("false");
    QJsonValue jsecond = json.value("Message");
    ui->lbSuccess->setText(ui->lbSuccess->text()+"\nnull");
}

TableModel::TableModel(QObject *parent): QAbstractTableModel(parent)
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

void TableModel::appendCurrency(TableModel::Currency cur)
{
    beginInsertRows(QModelIndex(), 1, 1);
    currencies.append(cur);
    endInsertRows();
}

void TableModel::replaceCheck(QModelIndex index)
{
    if(index.column()==0){
        currencies[index.row()].filter ^= 1;
        emit dataChanged( index, index );
    }
}

