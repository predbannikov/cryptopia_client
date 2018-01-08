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
    if(loadConfig())
    {
        qDebug() << "configuration data is loaded";
    } else {
        qDebug() << "config data not load";
    }


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
//    QNetworkRequest req;
//    req.setUrl(QUrl("https://www.cryptopia.co.nz/Api/GetBalance"));
//    req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    const QByteArray req = "{}";
    QJsonObject json;

    QByteArray API_SECRET;
    API_SECRET.append(ui->leApiSecret->text());
    QString API_KEY = ui->leApiKey->text();
//    answer = nemo.post(req, json.toUtf8());
    QByteArray requestContentBase64String;
    requestContentBase64String = QCryptographicHash::hash(req, QCryptographicHash::Md5);
    QByteArray base64 = requestContentBase64String.toBase64();
    qint64 unixtimestamp = QDateTime::currentSecsSinceEpoch();

//    QJsonParseError parsError;
//    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &parsError);
//    QByteArray post = QJsonDocument::toJson(req);
//    QJsonObject root = document.object();
    QString nonce = QString::number(unixtimestamp);
    QString signature = API_KEY + "POST" + "https://www.cryptopia.co.nz/Api/GetBalance" + nonce + QString(base64);
    QMessageAuthenticationCode code(QCryptographicHash::Sha256);
    QByteArray api_secret = QCryptographicHash::hash(API_SECRET, QCryptographicHash::Md5);
    code.setKey(api_secret);
    code.addData(signature.toStdString().c_str());
    QByteArray hmacsignature = QCryptographicHash::hash(code.result(), QCryptographicHash::Md5);
    QString header_value = "amx " + API_KEY + ":" + hmacsignature + ":" + nonce;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString urlreq = "https://www.cryptopia.co.nz/api/GetCurrencies/";
    manager->post(QNetworkRequest(urlreq), header_value.toUtf8());

    qDebug() << nonce;


}

bool MainWindow::loadConfig()
{
    if(QFile::exists(NAME_FILE_CONFIG))
    {
        configFile.setFileName(NAME_FILE_CONFIG);
        QDataStream stream(&configFile);
        configFile.open(QIODevice::ReadOnly);

        QString str;
        stream >> str;
        ui->leApiKey->setText(str);
        stream >> str;
        ui->leApiSecret->setText(str);
        configFile.close();
        return true;
    } else {
        qDebug() << "conf file not found";
        return false;
    }
}

void MainWindow::on_saveConfig_clicked()
{
    if(QFile::exists(NAME_FILE_CONFIG)){
        if(QFile::remove(NAME_FILE_CONFIG))
            qDebug() << "File remove success";
        else
            qDebug() << "file not remove";
    }
    configFile.setFileName(NAME_FILE_CONFIG);
    configFile.open(QIODevice::ReadWrite);
    QDataStream csFile(&configFile);
    if (configFile.isOpen()){
        csFile << ui->leApiKey->text();
        csFile << ui->leApiSecret->text();
        qDebug() << "configuration save success";
    } else {
        qDebug() << "file not open";
    }

    configFile.close();
}
