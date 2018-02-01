#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
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


class TableModel: public QAbstractTableModel
{
    Q_OBJECT
public:

    struct Currency {
        int Id;
        QString label;
        long double lastPrice;
        bool filter;
    };
    TableModel(QObject* parent = 0);
    int rowCount( const QModelIndex& ) const override {return currencies.count();};
    int columnCount( const QModelIndex&  ) const override {return 4;};
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    void appendCurrency(Currency cur);

    enum KeyMarkets{
        filter=0 , Label, LastPrice, TradePairId,
        AskPrice, BidPrice, Low, High, Volume,
        BuyVolume, SellVolume, Change, Open, Close,
        BaseVolume, BaseBuyVolume, BaseSellVolume
    };

    typedef QList<Currency> Currencies;
    Currencies currencies;
public slots:

    void replaceCheck(QModelIndex index);

};


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
    TableModel *tableModel;
    QItemSelectionModel selection;



private slots:
    // Обработчик данных полученных от объекта QNetworkAccessManager
//    void onResult(QNetworkReply *reply);
//    void onResponse( QNetworkReply* reply);

    void on_getRequest_clicked();
    void on_clearte_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
    void on_saveConfig_clicked();

    void response(QJsonObject json);

signals:
    void sendKey(QByteArray);
    void sendJson(QJsonObject);

};

#endif // MAINWINDOW_H
