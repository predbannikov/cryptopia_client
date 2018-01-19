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

void Network::receivJson(QJsonDocument json)
{
    QJsonObject jsonObj = json.object();
    for(jsonObj.keys())

}

void Network::getBalance()
{
    const QByteArray reqjsonrec = "{}";
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reqjsonrec, &parseError);


//    QByteArray API_SECRET;
//    API_SECRET.append(ui->leApiSecret->text());

//    QString API_KEY = ui->leApiKey->text();

    QByteArray requestContentBase64String;
    QByteArray arrayreq = jsonDoc.toJson(QJsonDocument::Compact);
    requestContentBase64String = QCryptographicHash::hash(arrayreq, QCryptographicHash::Md5);
    QByteArray base64 = requestContentBase64String.toBase64();
    qint64 unixtimestamp = QDateTime::currentSecsSinceEpoch();

    QString nonce = QString::number(unixtimestamp);
//    QString signature = API_KEY + "POST" + QString("https:%2f%2fwww.cryptopia.co.nz%2fApi%2fGetBalance%2f").toLower().toStdString().c_str() + nonce + QString(base64);
    QString signature = API_KEY + "POST" + QString(QUrl::toPercentEncoding(
                "https://www.cryptopia.co.nz/Api/GetBalance","", "/:")).toLower()
            + nonce + QString(base64);
//    signature = "d1c55e1cc3234dbebd279e0224c9a959POSThttps%3a%2f%2fwww.cryptopia.co.nz%2fapi%2fgetbalance1515684198mZFLkyvTelC5g8XnyQrpOw==";
//    nonce = "1515684198";
    //    QByteArray api_secret = QCryptographicHash::hash(API_SECRET, QCryptographicHash::Md5);
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

    QNetworkRequest reqest(QUrl(QString("https://www.cryptopia.co.nz/Api/GetBalance").toLower()));
//    reqest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    reqest.setRawHeader(QByteArray("Content-Type"), "application/json; charset=utf-8");
    reqest.setRawHeader(QByteArray("Authorization"), header_value.toUtf8());

    postNetManager->post(reqest,arrayreq );
    qDebug() << header_value;
    qDebug() << nonce;
}

void Network::receivKey(QByteArray bytearray)
{
//    API_KEY = bytearray[0];
//    API_SECRET = bytearray[1];
//    qDebug() << "API_KEY: " << QString(API_KEY);
//    qDebug() << "API_SECRET: " << QString(API_SECRET);



//        Sleep(1000);
        int id = (int)QThread::currentThreadId();
        qDebug() << "doWork: " << QString::number(id);


}

void Network::getResult(QNetworkReply *reply)
{

}

void Network::postResult(QNetworkReply *reply)
{
    qDebug() << "Got on";
    if ((!reply->error())){
        qDebug() << "Got in if";
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        QByteArray data;
        data = reply->readAll();
        qDebug() << QString(data);

        if (!document.isNull()){
            qDebug() << "Got in isNull";

            // Забираем из документа корневой объект
            QJsonObject root = document.object();
        } else {
            qDebug() << parsError.errorString();
        }

    } else {
        qDebug() << "parsError.errorString()";
        qDebug() << reply->errorString();
    }

    reply->deleteLater();
}
