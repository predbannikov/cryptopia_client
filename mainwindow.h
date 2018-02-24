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
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "mythread.h"
#include "network.h"
#include "hystorypair.h"
#include "orders.h"
#include "pairs.h"
#include "graphics.h"
#include "openordrers.h"



namespace Ui {
class MainWindow;
}

struct RequestData {
    QString url;
};

static QString dbgMessage;

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
    int rowCount( const QModelIndex& ) const override;
    int columnCount( const QModelIndex&  ) const override;
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

    enum StateResponse{StateGetMarket = 0, StateGetHystory, StateGetOrders, StateGetMyOrders, StateAvailable, OrderId } stateResponse;

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

    ModelPairs::Currency curCoin;


    QString printJsonValueType(QJsonValue type);

    Network *network;
    ModelPairs *modelPairs;
    QItemSelectionModel selection;
    QPainter *paint;
    QStringListModel *modelHystory;
    ModelOrders     *sellOrderModel;
    ModelOrders     *bayOrderModel;
    ModelBalance    *modelBalance;
    OpenOrdrers     *openOrdrers;

    QGraphicsScene  *grScene;
    DrawWidget      *drawWidget;

    enum StateThemes {darkTheme=0, customTheme=1} stateTheme = darkTheme;
    enum StateCalculation {stateCalculation = 0, stateWait = 1} stateCalc = stateWait;
    QHash <StateThemes,QString> themes;

    HystoryDeals hystory;

public slots:
    void getResponse(QJsonObject json);
    void postResponse(QJsonObject json);
    void setPrice(double price, double amount);
    bool loadConfig();
    void GetMarket(int id);
    void GetHystory(int id);
    void GetOrders(int id);
    void launchMarket();
    void setMarket(int pairId);
    void countRequest();
//    void clearSelectOrder(ModelOrders::Type type);
    void customMenuRequested(QPoint pos);
    void romoveOrder();

protected:
signals:
    void sendKey(QByteArray);
    void sendJson(QJsonObject);

private slots:


    void on_getRequest_clicked();
    void on_exitButton_clicked();
    void on_postRequest_clicked();
    void on_saveConfig_clicked();


    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_7_clicked();

    void on_themes_clicked();

    void on_pushButton_clicked();

    void on_pushButton_11_clicked();

    void on_HrzTblHdr_clicked();

    void on_bayAmount_valueChanged(double arg1);

    void on_bayPrice_valueChanged(double arg1);

    void on_sellAmount_valueChanged(double arg1);

    void on_sellPrice_valueChanged(double arg1);

    void on_tableBalance_clicked(const QModelIndex &index);

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_sbround_valueChanged(int arg1);

    void on_filterPairs_clicked();

    void on_chkbksFindProfit_clicked();

    void on_pushButton_14_clicked();

    void on_bayTotal_valueChanged(double arg1);

    void on_sellTotal_valueChanged(double arg1);

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();


    void on_pushButton_18_clicked();
};

#endif // MAINWINDOW_H
