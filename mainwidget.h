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
                        INSTALL_OK, INSTALL_FAIL };

    Ui::MainWidget *ui;
    bool ipTested;
    bool loadTimeZone();
    bool loadIPList();
    void addLog(const QString __log_string) const;

    void updateIpStausTable();

    QString getHostIpStatus(HostIpStatus __status) const;

    QHash<QString,QString> timezonelist;
    QHash<QString,HostIpStatus> iplist;

    bool execCommand(const QString __command,
                     const QStringList __args,
                     QByteArray &__output);

    void procEvent(int pause);

private slots:
    void setEnableButton();
    void setEnableSetter();
    void setDisableSetter();
    void testIp();
    void setTime();

signals:
    void checking();
    void checked();
};

#endif // MAINWIDGET_H
