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
    ui->newZoneTable->setDisabled(true);

    loadIPList();

    loadTimeZone("cur_timezone_list.txt", currenttimezonelist);
    loadTimeZone("new_timezone_list.txt", newtimezonelist);

    updateTimeZoneTables();

    connect(ui->currentZoneTable, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableNewTimeZone()));
    connect(ui->currentZoneTable, SIGNAL(itemSelectionChanged()), this, SLOT(setDisableButton()));
    connect(ui->newZoneTable, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableButton()));

    connect(this, SIGNAL(checked()), this, SLOT(setEnableZonesBox()));
    connect(this, SIGNAL(checking()), this, SLOT(setDisableZonesBox()));

    connect(ui->testButton, SIGNAL(clicked()), this, SLOT(testIp()));
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(setTime()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::updateTimeZoneTables()
{
    int row(0);

    ui->newZoneTable->setRowCount(newtimezonelist.size());
    QHash<QString, QString>::const_iterator i1 = newtimezonelist.constBegin();
    while (i1 != newtimezonelist.constEnd())
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(i1.key());
        ui->newZoneTable->setItem(row, 0, newItem);
        ++row;
        ++i1;
    }
    ui->newZoneTable->sortItems(0);

    row = 0;

    ui->currentZoneTable->setRowCount(currenttimezonelist.size());
    QHash<QString, QString>::const_iterator i2 = currenttimezonelist.constBegin();
    while (i2 != currenttimezonelist.constEnd())
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(i2.key());
        ui->currentZoneTable->setItem(row, 0, newItem);
        ++row;
        ++i2;
    }
    ui->currentZoneTable->sortItems(0);
}

bool MainWidget::loadTimeZone(const QString &__fileName, QHash<QString,QString> &__hashList)
{
    QFile file(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("extra")
               .append(QDir::separator())
               .append(__fileName));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file.fileName());
        QMessageBox::warning(this,
                             tr("Ошибка"),
                             tr("Ошибка при открытии файла %1").arg(file.fileName()),
                             QMessageBox::Ok
                             );
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
            __hashList.insert(list.at(0), list.at(1));
        }
        else
        {
            qCritical() << QString("Ошибка содержимого файла %1").arg(file.fileName());
            QMessageBox::warning(this,
                                 tr("Ошибка"),
                                 tr("Ошибка содержимого файла %1").arg(file.fileName()),
                                 QMessageBox::Ok
                                 );
            return false;
        }
    } while (1);

    file.close();

    qDebug() << tr("Загрузка содержимого файла %1 - Успешно").arg(file.fileName());

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

    updateIpStatusTable();

    qDebug() << "Загрузка содержимого файла iplist.txt - Успешно";

    return true;
}

void MainWidget::updateIpStatusTable()
{
    ui->ipTable->clearContents();
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
        case REBOOT_OK:
        {
            decodingStatus = "Установлено и перезагружено";
            break;
        }
        case REBOOT_FAIL:
        {
            decodingStatus = "Установлено и не перезагружено";
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

void MainWidget::setDisableButton()
{
    if(ui->setButton->isEnabled())
    {
        ui->setButton->setDisabled(true);
    }
}

void MainWidget::setEnableZonesBox()
{
    if(!ui->zonesBox->isEnabled())
    {
        ui->zonesBox->setEnabled(true);
    }
}

void MainWidget::setDisableZonesBox()
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
        plink.start("extra/plink", QStringList() << "-batch"
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

    emit checked();
}

void MainWidget::setTime()
{
    if(!createSetTimezoneScript( newtimezonelist.value( ui->newZoneTable->currentItem()->text() ) , "newzone.sh", "setnewzone.sh"))
    {
        qCritical() << "Создание скрипта с новой зоной - FAIL";
        return;
    }
    else
    {
        qDebug() << "Создание скрипта с новой зоной - OK";
    }

    if(!createSetTimezoneScript( currenttimezonelist.value( ui->currentZoneTable->currentItem()->text() ) , "curzone.sh", "setcurzone.sh"))
    {
        qCritical() << "Создание скрипта с текущей зоной - FAIL";
        return;
    }
    else
    {
        qDebug() << "Создание скрипта с текущей зоной - OK";
    }

    if(QMessageBox::Cancel == QMessageBox::warning(this, tr("Установка временной зоны"),
                                    tr("Выбранная текущия зона: %1.\n"
                                       "Выбранная новая зона:   %2.\n"
                                       "Вы уверены?")
                                                   .arg(ui->currentZoneTable->currentItem()->text())
                                                   .arg(ui->newZoneTable->currentItem()->text()),
                                    QMessageBox::Ok
                                    | QMessageBox::Cancel,
                                    QMessageBox::Cancel))
    {
        return;
    }

    emit checking();

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
                qDebug() << "Результат: " << output;
                if(!output.contains("FAIL"))
                {
                    qDebug() << QString("Результат инсталяции - OK");
                    i.value() = INSTALL_OK;
                    if(ui->rebootCheckBox->isChecked())
                    {
                        qDebug() << QString("Перезагрузка %1").arg(i.key());

                        QString addr("root@");
                        addr.append(i.key());

                        QStringList args_list;
                        args_list << "-batch"
                                  << "-pw" << "xxxxxx"
                                  << addr
                                  << "reboot";
                        if(execCommand("plink.exe", args_list, output))
                        {
                            qDebug() << QString("Результат перезагрузки - OK");
                            i.value() = REBOOT_OK;
                        }
                        else
                        {
                            qDebug() << QString("Результат перезагрузки - FAIL");
                            i.value() = REBOOT_FAIL;
                        }
                    }
                }
                else
                {
                    qDebug() << QString("Результат инсталяции - FAIL");
                    qDebug() << "Результат: " << output;
                    i.value() = INSTALL_FAIL;
                }
            }
            else
            {
                i.value() = INSTALL_FAIL;
            }            

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

void MainWidget::setEnableNewTimeZone()
{
    QString utc;
    QRegExp r("[0-9][0-9]");
    int pos = r.indexIn(ui->currentZoneTable->currentItem()->text());
    if (pos > -1)
    {
         utc = r.cap();
    }
    else
    {
        qCritical() << "Ошибка при выборе ближайших зон";
    }

    int utc_num = utc.toInt();

    ui->newZoneTable->clearContents();

    int row(0);

    QHash<QString, QString>::const_iterator i = newtimezonelist.constBegin();
    while (i != newtimezonelist.constEnd())
    {
        if(i.key().contains(( utc_num <= 9 ) ? QString("0%1").arg(utc_num) : QString("%1").arg(utc_num))
           || i.key().contains( ( (utc_num + 1) <= 9 ) ? QString("0%1").arg(utc_num + 1) : QString("%1").arg(utc_num + 1))
           || i.key().contains( ( (utc_num + 2) <= 9 ) ? QString("0%1").arg(utc_num + 2) : QString("%1").arg(utc_num + 2))
           || i.key().contains( ( (utc_num - 1) <= 9 ) ? QString("0%1").arg(utc_num - 1) : QString("%1").arg(utc_num - 1))
           || i.key().contains( ( (utc_num - 2) <= 9 ) ? QString("0%1").arg(utc_num - 2) : QString("%1").arg(utc_num - 2)))
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(i.key());
            ui->newZoneTable->setItem(row, 0, newItem);
            ++row;
        }
        ++i;
    }
    ui->newZoneTable->sortItems(0);

    if(!ui->newZoneTable->isEnabled())
    {
        ui->newZoneTable->setEnabled(true);
    }
}

bool MainWidget::execCommand(const QString __command,
                             const QStringList __args,
                             QByteArray &__output)
{
    QProcess proc;
    qDebug() << "Команда" << __command << __args;

    proc.setWorkingDirectory(qApp->applicationDirPath().append(QDir::separator()).append("extra"));
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

bool MainWidget::createSetTimezoneScript(const QString &__zone_name, const QString &__filename, const QString &_execScript) const
{
    QFile file(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("extra")
               .append(QDir::separator())
               .append(__filename));

    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file.fileName());
        return false;
    }

    QTextStream out(&file);

    out << QString("#!/bin/sh\n\n")
        << QString("echo \" Starting %1 with zone %2 (`date`)\"\n").arg(_execScript).arg(__zone_name)
        << QString("/usr/local/ukmtimeup/%1 %2\n").arg(_execScript).arg(__zone_name);

    file.close();

    qDebug() << QString("Создание файла %1 - Успешно").arg(_execScript);

    return true;
}
