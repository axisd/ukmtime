#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QHash>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    enum HostIpStatus { NA, ONLINE, OFFLINE,
                        CP_OK, CP_FAIL,
                        MKDIR_OK, MKDIR_FAIL,
                        UNZIP_OK, UNZIP_FAIL,
                        CHMOD_OK, CHMOD_FAIL,
                        INSTALL_OK, INSTALL_FAIL,
                        REBOOT_OK, REBOOT_FAIL};

    Ui::MainWidget *ui;
    bool ipTested;
    bool loadTimeZone(const QString &__fileName, QHash<QString,QString> &__hashList);
    void updateTimeZoneTables();

    bool loadIPList();
    void addLog(const QString __log_string) const;

    void updateIpStatusTable();

    QString getHostIpStatus(HostIpStatus __status) const;

    QHash<QString,QString> currenttimezonelist;
    QHash<QString,QString> newtimezonelist;
    QHash<QString,HostIpStatus> iplist;

    bool execCommand(const QString __command,
                     const QStringList __args,
                     QByteArray &__output);

    void procEvent(int pause);

    bool createSetTimezoneScript(const QString __zone_name) const;

private slots:
    void setEnableButton();
    void setEnableZonesBox();
    void setDisableZonesBox();
    void testIp();
    void setTime();
    void setEnableNewTimeZone();

signals:
    void checking();
    void checked();
};

#endif // MAINWIDGET_H
