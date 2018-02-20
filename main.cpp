#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

//static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

//void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
//{
//    QByteArray localMsg = msg.toLocal8Bit();
////    printf(msg.toStdString().c_str());
//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        abort();

//    }
//}

//static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);


//void myCustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
//{
//    // Handle the messages!

//    // Call the default handler.
//    dbgMessage = msg;
//    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
//}

int main(int argc, char *argv[])
{
//    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
//    qInstallMessageHandler(myCustomMessageHandler);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
