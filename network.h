#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <windows.h>
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
private:
    QString name;
    QNetworkAccessManager *getNetManager;
    QNetworkAccessManager *postNetManager;
    QByteArray  API_KEY;
    QByteArray  API_SECRET;
signals:
    void send(int);
private slots:
    void getBalance();

    void getResult(QNetworkReply *reply);
    void postResult(QNetworkReply *reply);
public slots:
    void doWork();
    void receivJson(QJsonDocument json);
    void receivKey(QByteArray);

};

#endif // NETWORK_H