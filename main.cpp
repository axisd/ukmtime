#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>

void myMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type)
    {
        case QtDebugMsg:
        {
            txt = QString("%1 ##   DEBUG  ## %2")
                    .arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss"))
                    .arg(msg);
            break;
        }
        case QtWarningMsg:
        {
            txt = QString("%1 ##  WARNING ## %2")
                    .arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss"))
                    .arg(msg);
            break;
        }
        case QtCriticalMsg:
        {
            txt = QString("%1 ## CRITICAL ## %2")
                    .arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss"))
                    .arg(msg);
            break;
        }
        case QtFatalMsg:
        {
            txt = QString("%1 ##   FATAL ## %2")
                    .arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss"))
                    .arg(msg);
            break;
        }
    }

    QFile outFile(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("logs.txt"));

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qInstallMsgHandler(myMessageHandler);

    QApplication a(argc, argv);

    MainWidget w;
    w.show();

    return a.exec();
}
