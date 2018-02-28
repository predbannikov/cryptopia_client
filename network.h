#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QUrl>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QNetworkAccessManager>
#include <QThread>
#include <QEventLoop>

class Network : public QObject
{
    Q_OBJECT
public:
    Network(QString name);
    ~Network();
    QByteArray  API_KEY;
    QByteArray  API_SECRET;
private:
    QString name;
    QNetworkAccessManager *getNetManager;
    QNetworkAccessManager *postNetManager;
    const QString URL = "https://www.cryptopia.co.nz/api/";
    QString url;
    QJsonObject testobj;
    QByteArray arrayreq;
    QJsonDocument jdoc;
    QJsonParseError parseError;
    qint64 lastTimeStamp;

    void postRequest();
signals:
    void send(int);
    void sendMessage(QJsonObject);
    void sendMessagePOST(QJsonObject);
    void sendCountRequest();
private slots:
//    void getBalance(QString request);

    void getResult(QNetworkReply *reply);
    void postResult(QNetworkReply *reply);
public slots:
    void doWork();
    void receivJson(QJsonObject json);
    void receivKey(QByteArray);

};

#endif // NETWORK_H
