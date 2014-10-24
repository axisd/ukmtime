#include "timeset.h"
#include "ui_timeset.h"

#include <QFile>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QMessageBox>

// ################
bool generateDateTimeVar(const QDate &__date, const QTime &__time)
{
    QFile file_in(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("extra")
               .append(QDir::separator())
               .append("datetime.sh"));

    if (!file_in.open(QIODevice::WriteOnly))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file_in.fileName());
        return false;
    }

    QDateTime t;
    t.setDate(__date);
    t.setTime(__time);

    // "+%%Y/%%m/%%d %%k:%%M:%%S"

    QTextStream out(&file_in);
    out << "NEWTIME=\"" << t.toString("yyyy/MM/dd hh:mm:ss") << "\"";

    file_in.close();

    qDebug() << QString("Создание файла %1 - Успешно").arg("datetime.sh");

    return true;
}
// ################

TimeSet::TimeSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeSet)
{
    ui->setupUi(this);

    ui->curHostDate->setDate(QDate::currentDate());
    ui->curHostTime->setTime(QTime::currentTime());

    ui->otherHostDate->setDate(QDate::currentDate());
    ui->otherHostTime->setTime(QTime::currentTime());

    connect(ui->curHostRadio, SIGNAL(toggled(bool)), this, SLOT(selectCurDateTime(bool)));
    connect(ui->otherHostRadio, SIGNAL(toggled(bool)), this, SLOT(selectOtherDateTime(bool)));

    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(setTime()));

    timer = new QTimer();
    timer->start(1000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(clockTick()));

    selectOtherDateTime(false);
    loadIPList();

    proc = new MProcess(iplist);
    connect(proc, SIGNAL(endCmd()), this, SLOT(endCmd()));
    connect(proc, SIGNAL(result(QString)), this, SLOT(result(QString)));
    connect(this, SIGNAL(beginCmd(bool)), proc, SLOT(beginCmd(bool)));

    commThr = new QThread;
    proc->moveToThread(commThr);
    commThr->start();
}

TimeSet::~TimeSet()
{
    if(timer->isActive())
    {
        timer->stop();
    }

    if(commThr->isRunning())
    {
        commThr->terminate();
    }

    delete proc;
    delete commThr;

    delete timer;
    delete ui;
}

bool TimeSet::loadIPList()
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

        iplist.append(line);
    } while (1);

    file.close();

    qDebug() << "Загрузка содержимого файла iplist.txt - Успешно";

    return true;
}

void TimeSet::flushResultFile() const
{
    QFile file_in(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("result.txt"));

    if(file_in.exists())
    {
        file_in.remove();
    }
}

void TimeSet::selectCurDateTime(bool __select)
{
    if(__select)
    {
        ui->curHostDate->setEnabled(true);
        ui->curHostTime->setEnabled(true);
    }
    else
    {
        ui->curHostDate->setDisabled(true);
        ui->curHostTime->setDisabled(true);
    }
}

void TimeSet::selectOtherDateTime(bool __select)
{
    if(__select)
    {
        ui->otherHostDate->setEnabled(true);
        ui->otherHostTime->setEnabled(true);
    }
    else
    {
        ui->otherHostDate->setDisabled(true);
        ui->otherHostTime->setDisabled(true);
    }
}

void TimeSet::clockTick()
{
    ui->curHostTime->setTime(ui->curHostTime->time().addSecs(1));
    ui->otherHostTime->setTime(ui->otherHostTime->time().addSecs(1));
}

void TimeSet::setTime()
{
    flushResultFile();
    if(QMessageBox::Cancel == QMessageBox::warning(this, tr("Установка времени"),
                                    tr("На кассы будет установлено новое время.\n\n\n"
                                       "Начать установку?"),
                                    QMessageBox::Ok
                                    | QMessageBox::Cancel,
                                    QMessageBox::Cancel))
    {
        return;
    }

    generateDateTimeVar(ui->curHostRadio->isChecked() ?
                            ui->curHostDate->date() : ui->otherHostDate->date(),
                        ui->curHostRadio->isChecked() ?
                            ui->curHostTime->time() : ui->otherHostTime->time());

    ui->allBox->setDisabled(true);

    if(ui->curHostRadio->isChecked())
    {
        emit beginCmd(true);
    }
    else
    {
        emit beginCmd(false);
    }
}

void TimeSet::result(QString __result)
{
    QFile file_in(qApp->applicationDirPath()
               .append(QDir::separator())
               .append("result.txt"));

    if (!file_in.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qCritical() << QString("Невозможно открыть файл: %1").arg(file_in.fileName());
        return;
    }

    QTextStream out(&file_in);
    out << __result << "\n";

    file_in.close();
}

void TimeSet::endCmd()
{
    QMessageBox::information(this, tr("Установка времени"),
                             tr("Установка завершена.\n\n"
                                "Результат установки сохранен в файле result.txt"),
                             QMessageBox::Ok);

    ui->allBox->setEnabled(true);
}


MProcess::MProcess(const QStringList __iplist, QObject *parent) : iplist(__iplist), QObject(parent)
{}

MProcess::~MProcess()
{}

void MProcess::beginCmd(bool _regenerate_datetime)
{
    for(int i = 0; i < iplist.size(); ++i)
    {
        if(_regenerate_datetime)
        {
            generateDateTimeVar(QDate::currentDate(), QTime::currentTime());
        }

        QStringList args_list;
        args_list << "/C" << "rssh-internal-1host.cmd" << iplist.at(i);

        QByteArray output;
        if(execCommand("cmd.exe", args_list, output))
        {
            qDebug() << "Результат: " << output;
            if(output.contains("SUCCESS"))
            {
                qDebug() << QString("Результат инсталяции - OK");
                emit result(QString("%1 - %2").arg(iplist.at(i)).arg("SUCCESS"));
            }
            else
            {
                qDebug() << QString("Результат инсталяции - FAIL");
                emit result(QString("%1 - %2").arg(iplist.at(i)).arg("FAIL"));
            }
        }
        else
        {
            emit result(QString("%1 - %2").arg(iplist.at(i)).arg("FAIL"));
        }
    }

    emit endCmd();
}

bool MProcess::execCommand(const QString __command,
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

    if (!proc.waitForFinished(1800000))
    {
        qWarning() << "Таймаут при остановке. Прерывание процесса." << proc.readAllStandardOutput();
        return false;
    }

    __output = proc.readAllStandardOutput();
    return true;
}
