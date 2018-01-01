#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QJsonValue>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;


    QString printJsonValueType(QJsonValue type);

private slots:
    // Обработчик данных полученных от объекта QNetworkAccessManager
    void onResult(QNetworkReply *reply);


    void on_getRequest_clicked();
    void on_clearte_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
};

#endif // MAINWINDOW_H
