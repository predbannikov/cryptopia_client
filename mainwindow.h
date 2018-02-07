#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QJsonValue>
#include <QFile>
#include <QTimer>
#include <QPainter>
#include <QDataStream>
#include <QByteArray>
#include <QStringListModel>
#include <QElapsedTimer>
#include "mythread.h"
#include "network.h"


//Q_DECLARE_METATYPE(QAbstractSocket::QByteArray)

namespace Ui {
class MainWindow;
}


struct RequestData {
    QString url;
};

//*********************    ModelBalance    ************************
class ModelBalance: public QAbstractTableModel
{
    Q_OBJECT
public:
    struct Balance {
        int Id;
        QString coin;
        double total;
        double available;
        double unconfirmed;
        double heldForTrades;
        double pendingWithdraw;
        QString address;
        QString baseAddress;
        QString status;
        QString statusMsg;
    };

    const int COLUMN = 11;

    ModelBalance(QObject* parent = 0);
    int rowCount( const QModelIndex& ) const override {return balances.count();};
    int columnCount( const QModelIndex&  ) const override {return COLUMN;};
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    void appendBalance(Balance bc);
    void clearBalance();

    enum KeyBalance{
        CurrencyId, Symbol, Total,
        Available, Unconfirmed, HeldForTrades,
        PendingWithdraw, Address, BaseAddress,
        Status, StatusMessage
    };

    typedef QList<Balance> Balances;
    Balances balances;
public slots:

    void selectedRow(QModelIndex index);
signals:
};

//*********************    ModelOrders    ************************
class ModelOrders: public QAbstractTableModel
{
    Q_OBJECT
public:
    struct Order {
        int Id;
        QString label;
        double price;
        double volume;
        double total;
    };

    const int COLUMN = 3;

    ModelOrders(QObject* parent = 0);
    int rowCount( const QModelIndex& ) const override {return orders.count();};
    int columnCount( const QModelIndex&  ) const override {return COLUMN;};
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    void appendOrder(Order ord);
    void clearOrders();

    enum KeyMarkets{
        Price, Volume, Total,
        TradePairId, Label
    };

    typedef QList<Order> Orders;
    Orders orders;
public slots:

    void selectedRow(QModelIndex index);
signals:
    void sendPrice(Order ord);
};



//*********************    TableModel    ************************

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

    const int COLUMN = 4;

    TableModel(QObject* parent = 0);
    int rowCount( const QModelIndex& ) const override {return currencies.count();};
    int columnCount( const QModelIndex&  ) const override {return COLUMN;};
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    void appendCurrency(Currency cur);
    void clearCurrencies();

    enum KeyMarkets{
        filter=0 , Label, LastPrice, TradePairId,
        AskPrice, BidPrice, Low, High, Volume,
        BuyVolume, SellVolume, Change, Open, Close,
        BaseVolume, BaseBuyVolume, BaseSellVolume
    };

    typedef QList<Currency> Currencies;
    Currencies currencies;
public slots:

//    void replaceCheck(QModelIndex index);
    void selectedRow(QModelIndex index);
signals:
    void getMarket(int id);
};

//*********************    DrawWidget    ************************

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    DrawWidget(QWidget* parent = 0);
    void paintEvent(QPaintEvent *);

};


//*********************    MainWindow    ************************
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
    QTimer  *timer;

    enum StateResponse{StateGetMarket = 0, StateGetHystory, StateGetOrders, StateGetMyOrders, StateAvailable } stateResponse;

    int countreq = 0;

    struct MarketOption {
        int timeoutMarkets, tickMarkets;
        int timeoutMarket, tickMarket;
        int timeoutHystory, tickHystory;
        int timeoutOrders, tickOrders;
        int msupdata;
        int msHystory;
        int countH;
        int Id;
        int msOrder;
        int countO;
        int course;
        int countC;
        int step;
    } marketOption;



    QString printJsonValueType(QJsonValue type);
    bool loadConfig();

    Network *network;
    TableModel *tableModel;
    QItemSelectionModel selection;
    QPainter *paint;
    QStringListModel *modelHystory;
    ModelOrders     *sellOrderModel;
    ModelOrders     *bayOrderModel;
    ModelBalance    *modelBalance;

    enum StateThemes {darkTheme=0, customTheme=1} stateTheme = darkTheme;
    QHash <StateThemes,QString> themes;

private slots:

    void on_getRequest_clicked();
    void on_clearte_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
    void on_saveConfig_clicked();

    void response(QJsonObject json);

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_7_clicked();

    void on_themes_clicked();

    void on_pushButton_clicked();

    void on_pushButton_11_clicked();

    void on_HrzTblHdr_clicked();
    void setPrice(ModelOrders::Order ord);

    void on_bayAmount_valueChanged(double arg1);

    void on_bayPrice_valueChanged(double arg1);

    void on_sellAmount_valueChanged(double arg1);

    void on_sellPrice_valueChanged(double arg1);

    void on_tableBalance_clicked(const QModelIndex &index);

    void on_pushButton_12_clicked();

public slots:
    void GetMarket(int id);
    void GetHystory(int id);
    void GetOrders(int id);
    void launchMarket();
    void setMarket(int pairId);
    void countRequest();
protected:
    void paintEvent(QPaintEvent *);
    TableModel::Currency  currency;
signals:
    void sendKey(QByteArray);
    void sendJson(QJsonObject);

};

#endif // MAINWINDOW_H
