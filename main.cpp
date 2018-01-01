#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
