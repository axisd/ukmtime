#include "xdialog.h"
#include "ui_xdialog.h"

#include <QDateTime>
#include <QFile>
#include <QProcess>

#include <QDebug>

XDialog::XDialog(QWidget *parent) :
    m_dialog_main(
            "Внимание!\n"
            "Обновлена информация о часовых поясах.\n"
            "Установлено время: %1\n"
            "Установленный часовой пояс: %2\n\n"
            "Для продолжения нажмите соответствующую кнопку."),
    m_dialog_succ(
            "При запуске кассовой программы проверьте\n"
            "время на экране и на фискальном устройстве\n"
            "(снимите X отчёт).\n"
            "В случае несоответствия времени вызовите\n"
            "системного администратора.\n\n"
            "Для запуска кассовой программы нажмите кнопку \"Продолжить\""),
    m_dialog_fail(
            "После обновления информации о часовых поясах\n"
            "установлено неверное время.\n"
            "Работа кассовой программы заблокирована.\n"
            "Необходимо установить верное время.\n"
            "Вызовите системного администратора."),
    m_continue(false),
    QWidget(parent),
    ui(new Ui::XDialog)
{
    ui->setupUi(this);

    mainDialog();

    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(succDialog()));
    connect(ui->rejectButton, SIGNAL(clicked()), this, SLOT(failDialog()));
}

XDialog::~XDialog()
{
    delete ui;
}

void XDialog::mainDialog()
{
    qDebug() << " **** xuserdialog started **** ";
    QString localtime = QFile::symLinkTarget("/etc/localtime");
    QString timezone = localtime.right(localtime.size() - localtime.indexOf("info/") - 5);


    ui->dialogText->clear();
    ui->dialogText->append(m_dialog_main
                           .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"))
                           .arg(timezone)
                           );

    ui->confirmButton->setText("Время верное");
    ui->rejectButton->setText("Время неверное");
}

void XDialog::succDialog()
{
    if(m_continue)
    {
        execCommand("/usr/bin/ukmon");
        qDebug() << " **** xuserdialog finished **** ";
        this->close();
    }
    else
    {
        qDebug() << " Pressed \"Correct time\" ";
        ui->dialogText->clear();
        ui->dialogText->append(m_dialog_succ);

        ui->confirmButton->setText("Продолжить");
        ui->rejectButton->hide();

        m_continue = true;
    }
}

void XDialog::failDialog()
{
    if(m_continue)
    {
        execCommand("/usr/bin/ukmoff");
        qDebug() << " **** xuserdialog finished **** ";
        this->close();
    }
    else
    {
        qDebug() << " Pressed \"Wrong time\" ";
        ui->dialogText->clear();
        ui->dialogText->append(m_dialog_fail);

        ui->confirmButton->hide();
        ui->rejectButton->setText("Продолжить");

        m_continue = true;
    }
}

bool XDialog::execCommand(const QString __command,
                          const QStringList __args) const
{
    QProcess proc;
    qDebug() << " Command " << __command << __args;

    proc.start(__command, __args);

    if (!proc.waitForStarted())
    {
        qDebug() << " Timeout at the start " << proc.readAllStandardOutput();
        return false;
    }

    if (!proc.waitForFinished(60000))
    {
        qDebug() << " Timeout at the finish " << proc.readAllStandardOutput();
        return false;
    }

    return true;
}
