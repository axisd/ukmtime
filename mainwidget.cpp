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
    ui->zonesBox->setDisabled(true);

    loadIPList();
    loadTimeZone();

    connect(ui->zoneTable, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableButton()));
    connect(this, SIGNAL(checked()), this, SLOT(setEnableSetter()));
    connect(this, SIGNAL(checking()), this, SLOT(setDisableSetter()));

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
    ui->ipTable->clearContents();
    //ui->ipTable->setHorizontalHeaderLabels(QStringList() << "IP Кассы" << "Состояние");
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

    //ui->ipTable->verticalHeader()->stretchLastSection();
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
        case UNZIP_OK:
        {
            decodingStatus = "Распаковано";
            break;
        }
        case UNZIP_FAIL:
        {
            decodingStatus = "Неуспешная распаковка";
            break;
        }
        case CHMOD_OK:
        {
            decodingStatus = "Флаги запуска устновлены";
            break;
        }
        case CHMOD_FAIL:
        {
            decodingStatus = "Неуспешная установка флагов";
            break;
        }
        case INSTALL_OK:
        {
            decodingStatus = "Установлено";
            break;
        }
        case INSTALL_FAIL:
        {
            decodingStatus = "Неуспешная установка";
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

void MainWidget::setEnableSetter()
{
    if(!ui->zonesBox->isEnabled())
    {
        ui->zonesBox->setEnabled(true);
    }
}

void MainWidget::setDisableSetter()
{
    if(ui->zonesBox->isEnabled())
    {
        ui->zonesBox->setDisabled(true);
    }
}

void MainWidget::testIp()
{    
    emit checking();

    QHash<QString, HostIpStatus>::iterator i = iplist.begin();
    while (i != iplist.end())
    {
        QString addr("root@");
        addr.append(i.key());

        qDebug() << QString("Старт plink на %1").arg(i.key());
        QProcess plink;
        plink.start("plink", QStringList() << "-batch"
                   << "-pw" << "xxxxxx"
                   << addr
                   << "echo" << "OK");

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

    //updateIpStausTable();

    emit checked();
}

void MainWidget::setTime()
{
    if(!createSetTimezoneScript( timezonelist.value( ui->zoneTable->currentItem()->text() ) ))
    {
        qCritical() << "Создание скрипта с новой зоной - FAIL";
        return;
    }
    else
    {
        qDebug() << "Создание скрипта с новой зоной - OK";
    }

    QHash<QString, HostIpStatus>::iterator i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == ONLINE)
        {
            QStringList args_list;
            args_list << "/C" << "install.cmd" << i.key();

            qDebug() << QString("Старт скрипта работы с %1").arg(i.key());
            qDebug() << "cmd.exe" << args_list;

            QByteArray output;
            if(execCommand("cmd.exe", args_list, output))
            {
                if(!output.contains("FAIL"))
                {
                    qDebug() << QString("Результат инсталяции - OK");
                    i.value() = INSTALL_OK;
                }
                else
                {
                    qDebug() << QString("Результат инсталяции - FAIL");
                    i.value() = INSTALL_FAIL;
                }
            }
            else
            {
                i.value() = INSTALL_FAIL;
            }

            qDebug() << "Результат: " << output;

            QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);
            if(listI.size() > 0)
            {
                ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
            }
        }

        ++i;

        procEvent(100);
    }
}

bool MainWidget::execCommand(const QString __command,
                             const QStringList __args,
                             QByteArray &__output)
{
    QProcess proc;
    qDebug() << "Команда" << __command << __args;

    proc.setWorkingDirectory(qApp->applicationDirPath());
    proc.start(__command, __args);

    if (!proc.waitForStarted())
    {
        qWarning() << "Таймаут при старте. Прерывание процесса." << proc.readAllStandardOutput();
        return false;
    }

    if (!proc.waitForFinished())
    {
        qWarning() << "Таймаут при остановке. Прерывание процесса." << proc.readAllStandardOutput();
        return false;
    }

    __output = proc.readAllStandardOutput();
    return true;
}

void MainWidget::procEvent(int pause)
{
    QEventLoop *tELoop = new QEventLoop(this);
    QTimer::singleShot(pause, tELoop, SLOT(quit()));
    tELoop->exec();
}

bool MainWidget::createSetTimezoneScript(const QString __zone_name) const
{
    QFile file(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("setnewtimezone.sh"));

    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file.fileName());
        return false;
    }

    QTextStream out(&file);

    out << QString("#!/bin/sh\n\n")
        << QString("echo \" Starting setzone.sh with zone %1 (`date`)\"\n").arg(__zone_name)
        << QString("/usr/local/ukmtimeup/./setzone.sh %1\n").arg(__zone_name);

    file.close();

    qDebug() << "Создание файла setnewtimezone.sh - Успешно";

    return true;
}
