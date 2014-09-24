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
    QString path("/usr/local/ukmtimeup");
    QStringList plink_args;
    plink_args << "-batch" << "-pw" << "xxxxxx";

    // создаём папки
    QHash<QString, HostIpStatus>::iterator i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == ONLINE)
        {
            QString addr("root@");
            addr.append(i.key());

            QStringList args_list;
            args_list << plink_args << addr
                      << "mkdir" << path;

            qDebug() << QString("Создание папки %1 на %2").arg(path).arg(i.key());
            qDebug() << "plink" << args_list;

            QByteArray output;
            if(execCommand("plink", args_list, output))
            {
                i.value() = MKDIR_OK;
            }
            else
            {
                i.value() = MKDIR_FAIL;
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

    // копируем файлы
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == MKDIR_OK)
        {
            QString addr("root@");
            addr.append(i.key()).append(":").append(path);

            QStringList args_list;
            args_list << "/C" << "rssh-1host.cmd" << i.key();

            qDebug() << QString("Старт копирования файла %1").arg(i.key());
            qDebug() << "cmd.exe" << args_list;

            QByteArray output;
            if(execCommand("cmd.exe", args_list, output))
            {
                if(output.contains("completed"))
                {
                    qDebug() << QString("Результат копирования - OK");
                    i.value() = CP_OK;
                }
                else
                {
                    qDebug() << QString("Результат копирования - FAIL");
                    i.value() = CP_FAIL;
                }
            }
            else
            {
                i.value() = CP_FAIL;
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

    // распаковываем
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == CP_OK)
        {
            QString addr("root@");
            addr.append(i.key());

            QString patch_file("/usr/local/ukmtimeup/timezone_patch.zip");

            QStringList args_list;
            args_list << plink_args << addr
                      << "unzip" << patch_file << "-d" << path;

            qDebug() << QString("Распаковка архива %1 в %2 на %3")
                        .arg(patch_file)
                        .arg(path)
                        .arg(i.key());
            qDebug() << "plink" << args_list;

            QByteArray output;
            if(execCommand("plink", args_list, output))
            {
                i.value() = UNZIP_OK;
            }
            else
            {
                i.value() = UNZIP_FAIL;
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

    // делаем скрипт запускаемым
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == UNZIP_OK)
        {
            QString addr("root@");
            addr.append(i.key());

            QString install_script("/usr/local/ukmtimeup/install.sh");

            QStringList args_list;
            args_list << plink_args << addr
                      << "chmod" << "-v" << "a+x" << install_script;

            qDebug() << QString("Создание запускаемого скрипта на %1").arg(i.key());
            qDebug() << "plink" << args_list;

            QByteArray output;
            if(execCommand("plink", args_list, output))
            {
                i.value() = CHMOD_OK;
            }
            else
            {
                i.value() = CHMOD_FAIL;
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

    // устанавливаем
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == CHMOD_OK)
        {
            QString addr("root@");
            addr.append(i.key());

            QString install_script("/usr/local/ukmtimeup/install.sh");

            QStringList args_list;
            args_list << plink_args << addr
                      << install_script;

            qDebug() << QString("Установка скриптов на %1").arg(i.key());
            qDebug() << "plink" << args_list;

            QByteArray output;
            if(execCommand("plink", args_list, output))
            {
                if(output.contains("SUCCESS"))
                {
                    qDebug() << QString("Результат установки - OK");
                    i.value() = INSTALL_OK;
                }
                else
                {
                    qDebug() << QString("Результат установки - FAIL");
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

    /*QStringList plink_args;
    plink_args << "-batch" << "-pw" << "xxxxxx";

    // создаём папки
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
            args_list << plink_args << addr
                      << "mkdir" << path;

            qDebug() << QString("Создание папки %1 на %2").arg(path).arg(i.key());
            qDebug() << "plink" << args_list;

            plink.start("plink", args_list);

            if (!plink.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = MKDIR_FAIL;
                ++i;
                break;
            }

            if (!plink.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке plink. Прерывание процесса." << plink.readAllStandardOutput();
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

    // копируем файлы
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

            if (!pscp.waitForStarted())
            {
                qWarning() << "Таймаут при старте pscp. Прерывание процесса." << pscp.readAllStandardOutput();
                i.value() = CP_FAIL;
                ++i;
                break;
            }

            if (!pscp.waitForFinished())
            {
                qWarning() << "Таймаут при остановке pscp. Прерывание процесса." << pscp.readAllStandardOutput();
                i.value() = CP_FAIL;
                ++i;
                break;
            }

            QByteArray output = pscp.readAllStandardOutput();
            if(output.contains("completed"))
            {
                qDebug() << QString("Результат копирования - OK");
                i.value() = CP_OK;
            }
            else
            {
                qDebug() << QString("Результат копирования - FAIL");
                i.value() = CP_FAIL;
            }

            qDebug() << "Результат: " << output;

            procEvent(100);
        }

        QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);
        if(listI.size() > 0)
        {
            ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
        }

        ++i;
    }

    // распаковываем
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == CP_OK)
        {
            QString addr("root@");
            QString path("/usr/local/ukmtimeup");

            addr.append(i.key());

            QString patch_file("/usr/local/ukmtimeup/timezone_patch.zip");

            // tar xvzf timezone_patch.zip
            QProcess plink;
            QStringList args_list;
            args_list << plink_args << addr
                      << "unzip" << patch_file << "-d" << path;

            qDebug() << QString("Распаковка архива %1 в %2 на %3")
                        .arg(patch_file)
                        .arg(path)
                        .arg(i.key());

            qDebug() << "plink" << args_list;

            plink.start("plink", args_list);

            if (!plink.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = UNZIP_FAIL;
                ++i;
                break;
            }

            if (!plink.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = UNZIP_FAIL;
                ++i;
                break;
            }

            i.value() = UNZIP_OK;

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

    // делаем скрипт запускаемым
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == UNZIP_OK)
        {
            QString addr("root@");
            addr.append(i.key());

            QString install_script("/usr/local/ukmtimeup/install.sh");

            // tar xvzf timezone_patch.zip
            QProcess plink;
            QStringList args_list;
            args_list << plink_args << addr
                      << "chmod" << "-v" << "a+x" << install_script;

            qDebug() << QString("Создание запускаемого скрипта на %1")
                        .arg(i.key());

            qDebug() << "plink" << args_list;

            plink.start("plink", args_list);

            if (!plink.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = CHMOD_FAIL;
                ++i;
                break;
            }

            if (!plink.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = CHMOD_FAIL;
                ++i;
                break;
            }

            i.value() = CHMOD_OK;

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

    // устанавливаем
    i = iplist.begin();
    while (i != iplist.end())
    {
        if(i.value() == CHMOD_OK)
        {
            QString addr("root@");
            addr.append(i.key());

            QString install_script("/usr/local/ukmtimeup/install.sh");

            // tar xvzf timezone_patch.zip
            QProcess plink;
            QStringList args_list;
            args_list << plink_args << addr
                      << install_script;

            qDebug() << QString("Установка скриптов на %1")
                        .arg(i.key());

            qDebug() << "plink" << args_list;

            plink.start("plink", args_list);

            if (!plink.waitForStarted(1000))
            {
                qWarning() << "Таймаут при старте plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = INSTALL_FAIL;
                ++i;
                break;
            }

            if (!plink.waitForFinished(1000))
            {
                qWarning() << "Таймаут при остановке plink. Прерывание процесса." << plink.readAllStandardOutput();
                i.value() = INSTALL_FAIL;
                ++i;
                break;
            }

            QByteArray output = plink.readAllStandardOutput();
            if(output.contains("SUCCESS"))
            {
                qDebug() << QString("Результат установки - OK");
                i.value() = INSTALL_OK;
            }
            else
            {
                qDebug() << QString("Результат установки - FAIL");
                i.value() = INSTALL_FAIL;
            }

            qDebug() << "Результат: " << output;

            procEvent(100);
        }

        QList<QTableWidgetItem*> listI = ui->ipTable->findItems(i.key(), Qt::MatchFixedString);
        if(listI.size() > 0)
        {
            ui->ipTable->item(listI.at(0)->row(), 1)->setText(getHostIpStatus(i.value()));
        }

        ++i;
    }

    updateIpStausTable();*/
}

bool MainWidget::execCommand(const QString __command,
                             const QStringList __args,
                             QByteArray &__output)
{
    QProcess proc;
    qDebug() << "Команда" << __command << __args;

    proc.setWorkingDirectory(qApp->applicationDirPath());
    proc.start(__command, __args);

    if (!proc.waitForStarted(1000))
    {
        qWarning() << "Таймаут при старте. Прерывание процесса." << proc.readAllStandardOutput();
        return false;
    }

    if (!proc.waitForFinished(1000))
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
