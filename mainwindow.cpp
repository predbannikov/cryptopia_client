#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>




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

void MainWindow::onResult(QNetworkReply *reply)
{
    // Если ошибки отсутсвуют
//    ui->teData->append(reply->readAll());
    if ((!reply->error())){  //&& reply->bytesAvailable()

        // То создаём объект Json Document, считав в него все данные из ответа
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        if (!document.isNull()){
            ui->teData->append(reply->readAll());
            // Забираем из документа корневой объект
            QJsonObject root = document.object();

        } else {
             qDebug() << parsError.errorString();
        }
    }
    reply->deleteLater();
}

void MainWindow::onResponse(QNetworkReply *reply)
{
//    ui->teData->append(reply->readAll());
//    qDebug() << "Got on";
//    if ((!reply->error())){
//        qDebug() << "Got in if";
//        QJsonParseError parsError;
//        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
//        QByteArray data;
//        data = reply->readAll();
//        qDebug() << QString(data);

//        if (!document.isNull()){
//            qDebug() << "Got in isNull";
//            ui->teData->append(reply->readAll());
//            // Забираем из документа корневой объект
//            QJsonObject root = document.object();
//        } else {
//            qDebug() << parsError.errorString();
//        }

//    } else {
//        qDebug() << "parsError.errorString()";
//        qDebug() << reply->errorString();
//    }

//    reply->deleteLater();
}

void MainWindow::on_getRequest_clicked()
{
    // Получаем данные, а именно JSON файл с сайта по определённому url
//    networkManager->get(QNetworkRequest(QUrl("https://www.cryptopia.co.nz/api/GetCurrencies")));

    QJsonDocument json;
    QJsonObject obj;
    obj["GET"]=QString("Currencies");
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



//    const QByteArray reqjsonrec = "{}";
//    QJsonParseError parseError;
//    QJsonDocument jsonDoc = QJsonDocument::fromJson(reqjsonrec, &parseError);


//    QByteArray API_SECRET;
//    API_SECRET.append(ui->leApiSecret->text());

//    QString API_KEY = ui->leApiKey->text();

//    QByteArray requestContentBase64String;
//    QByteArray arrayreq = jsonDoc.toJson(QJsonDocument::Compact);
//    requestContentBase64String = QCryptographicHash::hash(arrayreq, QCryptographicHash::Md5);
//    QByteArray base64 = requestContentBase64String.toBase64();
//    qint64 unixtimestamp = QDateTime::currentSecsSinceEpoch();

//    QString nonce = QString::number(unixtimestamp);
////    QString signature = API_KEY + "POST" + QString("https:%2f%2fwww.cryptopia.co.nz%2fApi%2fGetBalance%2f").toLower().toStdString().c_str() + nonce + QString(base64);
//    QString signature = API_KEY + "POST" + QString(QUrl::toPercentEncoding(
//                "https://www.cryptopia.co.nz/Api/GetBalance","", "/:")).toLower()
//            + nonce + QString(base64);
////    signature = "d1c55e1cc3234dbebd279e0224c9a959POSThttps%3a%2f%2fwww.cryptopia.co.nz%2fapi%2fgetbalance1515684198mZFLkyvTelC5g8XnyQrpOw==";
////    nonce = "1515684198";
//    //    QByteArray api_secret = QCryptographicHash::hash(API_SECRET, QCryptographicHash::Md5);
//    QByteArray api_secret = QByteArray::fromBase64(API_SECRET);
//    QMessageAuthenticationCode code(QCryptographicHash::Sha256);
//    code.setKey(api_secret);
////    code.addData(signature.toStdString().c_str());
//    QByteArray sigtobyte = signature.toUtf8();
//    code.addData(sigtobyte);
//    QByteArray decodapisecret = code.result();


////    QByteArray hmacsignature = QCryptographicHash::hash(decodapisecret, QCryptographicHash::Md5);
//    QByteArray hmacsignature = decodapisecret.toBase64();

//    QString header_value = "amx " + API_KEY + ":" + hmacsignature + ":" + nonce;
//    qDebug() << header_value;

//    QNetworkRequest reqest(QUrl(QString("https://www.cryptopia.co.nz/Api/GetBalance").toLower()));
////    reqest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
//    reqest.setRawHeader(QByteArray("Content-Type"), "application/json; charset=utf-8");
//    reqest.setRawHeader(QByteArray("Authorization"), header_value.toUtf8());

//    manager->post(reqest,arrayreq );
//    qDebug() << header_value;
//    qDebug() << nonce;
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

void MainWindow::update(int i)
{
    int id = (int) QThread::currentThread();
    qDebug() << "update()" << QString::number(id);
    qDebug() << QString::number(i);
}

void MainWindow::response(QJsonObject json)
{
    QStringList strstat;
//            strstat.append(QString::number(root.size(), 10));
    strstat.append(json.keys());
    QString tmp;
    for(int i=0; i<json.size(); i++)
    {
        tmp = strstat.at(i);
        if(tmp==QString("Success"))
        {
            if(1)
            {
                ui->lbStat->setText("Успешно");
                ui->lbStat->setStyleSheet("background-color: #5EED00; font-size: 15px; font-weight: bold; color: #000000");
                ui->lbStat->setAlignment(Qt::AlignCenter);
            } else {

            }
        }
        if(tmp==QString("Message"))
        {
            if(printJsonValueType(json.value(tmp))=="null")
            {
                ui->lbInsidMsg->setText("Сообщений нет");
                ui->lbInsidMsg->setStyleSheet("background-color: #828282; font-size: 15px; font-weight: bold; color: #000000");
                ui->lbInsidMsg->setAlignment(Qt::AlignCenter);
            } else {
            }
        }
        tmp.append(printJsonValueType(json.value(tmp)));
        ui->teData->append(tmp);
//                ui->teData->append("\n");
    }
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
