#include "network.h"

Network::Network(QString s) : name(s)
{

    getNetManager = new QNetworkAccessManager(this);
    postNetManager = new QNetworkAccessManager(this);

    // Подключаем networkManager к обработчику ответа
    connect(getNetManager, &QNetworkAccessManager::finished, this, &Network::getResult);
    connect(postNetManager, &QNetworkAccessManager::finished, this, &Network::postResult);

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
            getBalance("GetBalance");
        }
        if(json["POST"] == QString("GetOpenOrders"))
        {
            getBalance("GetOpenOrders");
        }
        if(json["POST"] == QString("CancelTrade"))
        {
//            QJson
            getBalance("CancelTrade");
        }
    } else {
        qDebug() << "got is: else(GET) in: void Network::receivJson(QJsonObject json)";
    }
}

void Network::getBalance(QString request)
{
    QString url = URL + request;
    const QByteArray reqjsonrec = "{}";
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reqjsonrec, &parseError);
    QByteArray requestContentBase64String;
    QByteArray arrayreq = jsonDoc.toJson(QJsonDocument::Compact);
    requestContentBase64String = QCryptographicHash::hash(arrayreq, QCryptographicHash::Md5);
    QByteArray base64 = requestContentBase64String.toBase64();
    qint64 unixtimestamp = QDateTime::currentSecsSinceEpoch();
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
    qDebug() << header_value;

    QNetworkRequest reqest(QUrl(url.toLower()));
//    reqest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    reqest.setRawHeader(QByteArray("Content-Type"), "application/json; charset=utf-8");
    reqest.setRawHeader(QByteArray("Authorization"), header_value.toUtf8());

    postNetManager->post(reqest,arrayreq );
    qDebug() << header_value;
    qDebug() << nonce;
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
    QNetworkReply::NetworkError  rpl = reply->error();
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
    qDebug() << "get POST result";
    if ((!reply->error())){
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        if (!document.isNull()){
            // Забираем из документа корневой объект
            QJsonObject root = document.object();
            emit sendMessagePOST(root);
        } else {
            qDebug() << parsError.errorString();
        }

    } else {
        qDebug() << "parsError.errorString()";
        qDebug() << reply->errorString();
    }
    reply->deleteLater();
}
