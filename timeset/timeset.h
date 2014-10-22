#ifndef TIMESET_H
#define TIMESET_H

#include <QWidget>
#include <QTimer>
#include <QThread>

namespace Ui {
class TimeSet;
}

class MProcess : public QObject
{
    Q_OBJECT

public:
    explicit MProcess(const QStringList __iplist, QObject *parent = 0);
    ~MProcess();

private:
    bool execCommand(const QString __command,
                     const QStringList __args,
                     QByteArray &__output);
    const QStringList iplist;

public slots:
    void beginCmd(bool _regenerate_datetime);

signals:
    void endCmd();
    void result(QString);
};

class TimeSet : public QWidget
{
    Q_OBJECT

public:
    explicit TimeSet(QWidget *parent = 0);
    ~TimeSet();

private:
    Ui::TimeSet *ui;

    QTimer *timer;
    QThread *commThr;
    MProcess *proc;

    QStringList iplist;

    bool loadIPList();    

private slots:
    void selectCurDateTime(bool __select);
    void selectOtherDateTime(bool __select);
    void clockTick();
    void setTime();
    void result(QString __result);

public slots:
    void endCmd();

signals:
    void beginCmd(bool _regenerate_datetime);
};

#endif // TIMESET_H
