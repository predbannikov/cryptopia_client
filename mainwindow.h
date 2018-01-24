#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QJsonValue>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include "mythread.h"
#include "network.h"


//Q_DECLARE_METATYPE(QAbstractSocket::QByteArray)

namespace Ui {
class MainWindow;
}


struct RequestData {
    QString url;
};
static const char* NAME_FILE_CONFIG = "config";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QThread *netthr;
private:
    Ui::MainWindow *ui;

    QFile   configFile;

    QString printJsonValueType(QJsonValue type);
    bool loadConfig();
    Network *network;
private slots:
    // Обработчик данных полученных от объекта QNetworkAccessManager
    void onResult(QNetworkReply *reply);
    void onResponse( QNetworkReply* reply);

    void on_getRequest_clicked();
    void on_clearte_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
    void on_saveConfig_clicked();

    void update (int i);

    void response(QJsonObject json);

signals:
    void sendKey(QByteArray);
    void sendJson(QJsonObject);

};

#endif // MAINWINDOW_H
