#include "network.h"

qint64 Network::lastTimeStamp = 0;

Network::Network(QString s) : name(s)
{

    getNetManager = new QNetworkAccessManager(this);
    postNetManager = new QNetworkAccessManager(this);

    // Подключаем networkManager к обработчику ответа
    connect(getNetManager, &QNetworkAccessManager::finished, this, &Network::getResult);
    connect(postNetManager, &QNetworkAccessManager::finished, this, &Network::postResult);
    lastTimeStamp = 0;
}

Network::~Network()
{
    qDebug() << "destroed";
}

void Network::doWork()
{
//    QEventLoop loop;
    //    loop.exec();
}

void Network::receivJson(QJsonObject json)
{
    if(json.contains("GET") && json["GET"].isString())
    {
        if(json["GET"] == QString("GetMarkets"))
        {
            QString url = URL + "GetMarkets";
            getNetManager->get(QNetworkRequest(QUrl(url)));
//            getNetManager->get(QNetworkRequest(QUrl("https://www.cryptopia.co.nz/api/GetCurrencies")));
        }
        if(json["GET"] == QString("GetMarket"))
        {
            QString url = URL + "GetMarket/";
            url+=QString::number(json["par"].toInt())+'/';
            getNetManager->get(QNetworkRequest(QUrl(url)));
        }
        if(json["GET"] == QString("GetMarketHistory"))
        {
            QString url = URL + "GetMarketHistory/";
            url+=QString::number(json["par"].toInt())+'/';
            getNetManager->get(QNetworkRequest(QUrl(url)));
        }
        if(json["GET"] == QString("GetMarketOrders"))
        {
            QString url = URL + "GetMarketOrders/";
            url+=QString::number(json["par"].toInt())+'/';
            getNetManager->get(QNetworkRequest(QUrl(url)));
        }
        emit sendCountRequest();
    } else if(json.contains("POST") && json["POST"].isString())
    {
        if(json["POST"] == QString("GetBalance"))
        {
            url = URL + "GetBalance";
            jdoc = QJsonDocument::fromJson("{}", &parseError);
            arrayreq = QJsonDocument(testobj).toJson(QJsonDocument::Compact);
            postRequest();
        }
        if(json["POST"] == QString("GetOpenOrders"))
        {
            url = URL + "GetOpenOrders";
            jdoc = QJsonDocument::fromJson("{}", &parseError);
            arrayreq = QJsonDocument(testobj).toJson(QJsonDocument::Compact);
            postRequest();
//            qDebug() << "GetOpenOrders()";
        }
        if(json["POST"] == QString("CancelTrade"))
        {
            url = URL + "CancelTrade";
            json.remove("POST");
//            jobj["Type"] = json["Type"].toString();
//            jobj["OrderId"] = json["OrderId"].toInt();
            arrayreq = QJsonDocument(json).toJson(QJsonDocument::Compact);
            postRequest();
//            testobj = jobj;
//            teststr = "test";
//            getBalance("CancelTrade");
        }
        if(json["POST"] == QString("SubmitTrade"))
        {
            url = URL + "SubmitTrade";
            json.remove("POST");
//            qDebug() << "Rate=" << json["Rate"].toDouble();
//            qDebug() << "Amount=" << json["Amount"].toDouble();
//            qDebug() << "Type=" << json["Type"].toString();
//            qDebug() << "Id=" << json["TradePairId"].toInt();
            qDebug() << "SubmitTrade";
            arrayreq = QJsonDocument(json).toJson(QJsonDocument::Compact);
            postRequest();
        }
    } else {
        qDebug() << "got is: else(GET) in: void Network::receivJson(QJsonObject json)";
    }
}

void Network::postRequest()
{
    QByteArray requestContentBase64String;
    requestContentBase64String = QCryptographicHash::hash(arrayreq, QCryptographicHash::Md5);
    QByteArray base64 = requestContentBase64String.toBase64();
    qint64 unixtimestamp = QDateTime::currentMSecsSinceEpoch();
//    int step = unixtimestamp - lastTimeStamp;
    if(unixtimestamp == lastTimeStamp)  // Если предыдущий запрос был с таким же временем,
    {
        QThread::msleep(60);
        unixtimestamp = QDateTime::currentMSecsSinceEpoch();
    }
    lastTimeStamp = unixtimestamp;
    QString nonce = QString::number(unixtimestamp);
    QString signature = API_KEY + "POST" + QString(QUrl::toPercentEncoding(
                url,"", "/:")).toLower()
            + nonce + QString(base64);
    QByteArray api_secret = QByteArray::fromBase64(API_SECRET);
    QMessageAuthenticationCode code(QCryptographicHash::Sha256);
    code.setKey(api_secret);
//    code.addData(signature.toStdString().c_str());
    QByteArray sigtobyte = signature.toUtf8();
    code.addData(sigtobyte);
    QByteArray decodapisecret = code.result();


//    QByteArray hmacsignature = QCryptographicHash::hash(decodapisecret, QCryptographicHash::Md5);
    QByteArray hmacsignature = decodapisecret.toBase64();

    QString header_value = "amx " + API_KEY + ":" + hmacsignature + ":" + nonce;
//    qDebug() << header_value;

    QNetworkRequest reqest(QUrl(url.toLower()));
//    reqest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    reqest.setRawHeader(QByteArray("Content-Type"), "application/json; charset=utf-8");
    reqest.setRawHeader(QByteArray("Authorization"), header_value.toUtf8());

    postNetManager->post(reqest,arrayreq );
//    qDebug() << "post reqest" << nonce;
}

void Network::receivKey(QByteArray bytearray)
{
//    API_KEY.append(bytearray[0]);
//    API_SECRET.append(bytearray[1]);
//    qDebug() << "API_KEY: " << QString(API_KEY);
//    qDebug() << "API_SECRET: " << QString(API_SECRET);



//        Sleep(1000);
//        int id = (int)QThread::currentThreadId();
//        qDebug() << "doWork: " << QString::number(id);


}

void Network::getResult(QNetworkReply *reply)
{
//    QNetworkReply::NetworkError  rpl = reply->error();
    if ((!reply->error())){
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        if (!document.isNull()){
            // Забираем из документа корневой объект
            QJsonObject root = document.object();
            emit sendMessage(root);
        } else {
            qDebug() << parsError.errorString();
        }
    }
    reply->deleteLater();
}

void Network::postResult(QNetworkReply *reply)
{
//    qint64 unixtimestamp = QDateTime::currentSecsSinceEpoch();

//    QString nonce = QString::number(unixtimestamp);

//    qDebug() << "get POST result" << nonce;
    if ((!reply->error())){
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        if (!document.isNull() && parsError.error == QJsonParseError::NoError){
            // Забираем из документа корневой объект
            QJsonObject root = document.object();
            emit sendMessagePOST(root);
        } else {
            qDebug() << "void Network::postResult() " << parsError.errorString();
        }

    } else {
        qDebug() << "parsError.errorString()";
        qDebug() << reply->errorString();
    }
    reply->deleteLater();
}
