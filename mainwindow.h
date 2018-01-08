#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QJsonValue>
#include <QFile>
#include <QDataStream>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>


namespace Ui {
class MainWindow;
}

static const char* NAME_FILE_CONFIG = "config";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QFile   configFile;

    QString printJsonValueType(QJsonValue type);
    bool loadConfig();

private slots:
    // Обработчик данных полученных от объекта QNetworkAccessManager
    void onResult(QNetworkReply *reply);


    void on_getRequest_clicked();
    void on_clearte_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
    void on_saveConfig_clicked();
};

#endif // MAINWINDOW_H
