#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    ipTested(false)
{
    ui->setupUi(this);
    ui->setButton->setDisabled(true);

    loadIPList();
    loadTimeZone();

    connect(ui->zoneTable, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableButton()));
    connect(ui->testButton, SIGNAL(clicked()), this, SLOT(testIp()));
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(setTime()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

bool MainWidget::loadTimeZone()
{
    QFile file(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("timezone_list.txt"));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file.fileName());
        return false;
    }

    QTextStream in(&file);
    QString line;
    do
    {
        line = in.readLine();
        if(line.size() == 0)
        {
            break;
        }
        QStringList list = line.split(";", QString::SkipEmptyParts);
        if(list.size() == 2)
        {
            timezonelist.insert(list.at(0), list.at(1));
        }
        else
        {
            qCritical() << "Ошибка содержимого файла timezone_list.txt";
            QMessageBox::warning(this,
                                 tr("Ошибка"),
                                 tr("Ошибка содержимого файла timezone_list.txt"),
                                 QMessageBox::Ok
                                 );
            return false;
        }
    } while (1);

    file.close();

    ui->zoneTable->setRowCount(timezonelist.size());
    int row(0);
    QHash<QString, QString>::const_iterator i = timezonelist.constBegin();
    while (i != timezonelist.constEnd())
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(i.key());
        ui->zoneTable->setItem(row, 0, newItem);
        ++row;
        ++i;
    }

    qDebug() << "Загрузка содержимого файла timezone_list.txt - Успешно";

    return true;
}

bool MainWidget::loadIPList()
{
    QFile file(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("iplist.txt"));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file.fileName());
        return false;
    }

    QTextStream in(&file);
    QString line;
    do
    {
        line = in.readLine();

        if(line.size() == 0)
        {
            break;
        }

        iplist.insert(line, NA);
    } while (1);

    file.close();

    updateIpStausTable();

    qDebug() << "Загрузка содержимого файла iplist.txt - Успешно";

    return true;
}

void MainWidget::updateIpStausTable()
{
    ui->ipTable->clear();
    ui->ipTable->setHorizontalHeaderLabels(QStringList() << "IP Кассы" << "Состояние");
    ui->ipTable->setRowCount(iplist.size());
    int row(0);
    QHash<QString, HostIpStatus>::const_iterator i = iplist.constBegin();
    while (i != iplist.constEnd())
    {
        QTableWidgetItem *newIp = new QTableWidgetItem(i.key());
        QTableWidgetItem *newStat = new QTableWidgetItem(getHostIpStatus(i.value()));
        ui->ipTable->setItem(row, 0, newIp);
        ui->ipTable->setItem(row, 1, newStat);
        ++row;
        ++i;
    }
    ui->ipTable->resizeColumnsToContents();
}

QString MainWidget::getHostIpStatus(MainWidget::HostIpStatus __status) const
{
    QString decodingStatus;
    switch (__status)
    {
        case ONLINE:
        {
            decodingStatus = "В сети";
            break;
        }
        case OFFLINE:
        {
            decodingStatus = "Не в сети";
            break;
        }
        case MKDIR_OK:
        {
            decodingStatus = "Папка создана";
            break;
        }
        case MKDIR_FAIL:
        {
            decodingStatus = "Неуспешное создание папки";
            break;
        }
        case CP_OK:
        {
            decodingStatus = "Скопировано";
            break;
        }
        case CP_FAIL:
        {
            decodingStatus = "Неуспешное копирование";
            break;
        }
        default:
        {
            decodingStatus = "Не известно";
            break;
        }
    }

    return decodingStatus;
}

void MainWidget::setEnableButton()
{
    if(!ui->setButton->isEnabled())
    {
        ui->setButton->setEnabled(true);
    }
}

void MainWidget::testIp()
{
    QHash<QString, HostIpStatus>::iterator i = iplist.begin();
    while (i != iplist.end())
    {
        QString addr("root@");
        addr.append(i.key());

        qDebug() << QString("Старт plink на %1").arg(i.key());
        QProcess plink;
        plink.start("plink", QStringList() << "-batch"
                   << "-pw"
                   << "xxxxxx"
                   << addr
                   << "echo"
                   << "OK");

        if (!plink.waitForStarted(1000))
        {
            qWarning() << "Таймаут при старте plink";
        }

        if (!plink.waitForFinished(1000))
        {
            qWarning() << "Таймаут при остановке plink";
        }

        if(plink.readAll().contains("OK"))
        {
            i.value() = ONLINE;
            qDebug() << QString("Результат проверки %1 - ONLINE").arg(i.key());
        }
        else
        {
            qDebug() << QString("Результат проверки %1 - OFFLINE").arg(i.key());
            i.value() = OFFLINE;
        }

        QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);

        if(listI.size() > 0)
        {
            ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
        }

        procEvent(100);

        ++i;
    }

    updateIpStausTable();
}

void MainWidget::setTime()
{
    QHash<QString, HostIpStatus>::iterator i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == ONLINE)
        {
            QString addr("root@");
            addr.append(i.key());
            QString path("/usr/local/ukmtimeup");

            QProcess plink;
            QStringList args_list;
            args_list << "-batch"
                      << "-pw"
                      << "xxxxxx"
                      << addr
                      << "mkdir"
                      << path;

            qDebug() << QString("Создание папки %1 на %2").arg(path).arg(i.key());
            qDebug() << "plink" << args_list;

            plink.start("plink", args_list);

            if (!plink.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте plink. Прерывание процесса.";
                i.value() = MKDIR_FAIL;
                ++i;
                break;
            }

            if (!plink.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке plink. Прерывание процесса.";
                i.value() = MKDIR_FAIL;
                ++i;
                break;
            }

            i.value() = MKDIR_OK;

            qDebug() << "Результат: " << plink.readAllStandardOutput();

            procEvent(100);
        }

        QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);
        if(listI.size() > 0)
        {
            ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
        }

        ++i;
    }

    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == MKDIR_OK)
        {
            QString addr("root@");
            QString path("/usr/local/ukmtimeup");

            addr.append(i.key()).append(":").append(path);

            QString patch_file("timezone_patch.zip");

            QProcess pscp;
            QStringList args_list;
            args_list << "/C"
                      << "rssh-1host.cmd"
                      << i.key();

            pscp.setWorkingDirectory(qApp->applicationDirPath());

            qDebug() << QString("Старт копирования файла %1 на %2")
                        .arg(patch_file)
                        .arg(i.key());
            qDebug() << "cmd.exe" << args_list;

            pscp.start("cmd.exe", args_list);

            if (!pscp.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте pscp. Прерывание процесса.";
                i.value() = CP_FAIL;
                ++i;
                break;
            }

            if (!pscp.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке pscp. Прерывание процесса.";
                i.value() = CP_FAIL;
                ++i;
                break;
            }

            i.value() = CP_OK;

            qDebug() << "Результат: " << pscp.readAllStandardOutput();

            procEvent(100);
        }

        QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);
        if(listI.size() > 0)
        {
            ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
        }

        ++i;
    }

    updateIpStausTable();
}

void MainWidget::procEvent(int pause)
{
    QEventLoop *tELoop = new QEventLoop(this);
    QTimer::singleShot(pause, tELoop, SLOT(quit()));
    tELoop->exec();
}
