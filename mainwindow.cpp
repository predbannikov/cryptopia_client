#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager();
    // Подключаем networkManager к обработчику ответа
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onResult);


}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::printJsonValueType(QJsonValue type)
{
    switch(type.type())
    {
        case QJsonValue::Null: return "Null";
        case QJsonValue::Bool: return "Bool";
        case QJsonValue::Double: return "Double";
        case QJsonValue::String: return "String";
        case QJsonValue::Array: return "Array";
        case QJsonValue::Object: return "Object";
        case QJsonValue::Undefined: return "Undefined";
        default:    return "uncnown";
    }

}

void MainWindow::onResult(QNetworkReply *reply)
{
    // Если ошибки отсутсвуют
//    ui->teData->append(reply->readAll());
    if ((!reply->error())){  //&& reply->bytesAvailable()

        // То создаём объект Json Document, считав в него все данные из ответа
        QJsonParseError parsError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
        if (!document.isNull()){
            ui->teData->append(reply->readAll());
            // Забираем из документа корневой объект
            QJsonObject root = document.object();
            QStringList strstat;
//            strstat.append(QString::number(root.size(), 10));
            strstat.append(root.keys());
            QString tmp;
            for(int i=0; i<root.size(); i++)
            {
                tmp = strstat.at(i);
                if(tmp==QString("Success"))
                {
                    if(1)
                    {
                        ui->lbStat->setText("Успешно");
                        ui->lbStat->setStyleSheet("background-color: #5EED00; font-size: 15px; font-weight: bold; color: #000000");
                        ui->lbStat->setAlignment(Qt::AlignCenter);
                    } else {

                    }
                }
                if(tmp==QString("Message"))
                {
                    if(printJsonValueType(root.value(tmp))=="null")
                    {
                        ui->lbInsidMsg->setText("Сообщений нет");
                        ui->lbInsidMsg->setStyleSheet("background-color: #828282; font-size: 15px; font-weight: bold; color: #000000");
                        ui->lbInsidMsg->setAlignment(Qt::AlignCenter);
                    } else {

                    }
                }
                tmp.append(printJsonValueType(root.value(tmp)));
                ui->teData->append(tmp);
//                ui->teData->append("\n");
            }

            strstat << "\n";



            // Второе значение пропишем строкой
            QJsonValue jfirst = root.value("Success");
            if (jfirst.isBool())
                ui->lbSuccess->setText("true");
            else
                ui->lbSuccess->setText("false");

            QJsonValue jsecond = root.value("Message");

            ui->lbSuccess->setText(ui->lbSuccess->text()+"\nnull");


//            QJsonValue jv = root.value("Data");
//            // Если значение является массивом, ...
//            if(jv.isArray()){
//                // ... то забираем массив из данного свойства
//                QJsonArray ja = jv.toArray();
//                // Перебирая все элементы массива ...
//                for(int i = 0; i < ja.count(); i++){
//                    QJsonObject subtree = ja.at(i).toObject();
//                    // Забираем значения свойств имени и фамилии добавляя их в textEdit
//                    ui->teData->append(subtree.value("firstName").toString() +
//                                         " " +
//                                         subtree.value("lastName").toString());
//                }
//            }
//            ui->statste->append(strstat.join(" "));

        } else {

            qDebug() << parsError.errorString();

        }



    }
    reply->deleteLater();
}

void MainWindow::on_getRequest_clicked()
{
    // Получаем данные, а именно JSON файл с сайта по определённому url
    networkManager->get(QNetworkRequest(QUrl("https://www.cryptopia.co.nz/api/GetCurrencies")));
}

void MainWindow::on_clearte_clicked()
{
    ui->teData->clear();
}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::on_postRequest_clicked()
{
    QNetworkRequest request(QUrl("https://www.cryptopia.co.nz/api/GetCurrencies"));




}










