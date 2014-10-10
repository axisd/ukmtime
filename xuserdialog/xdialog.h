#ifndef XDIALOG_H
#define XDIALOG_H

#include <QWidget>

namespace Ui {
class XDialog;
}

class XDialog : public QWidget
{
    Q_OBJECT

public:
    explicit XDialog(QWidget *parent = 0);
    ~XDialog();

private:
    Ui::XDialog *ui;

    const QString m_dialog_main;
    const QString m_dialog_succ;
    const QString m_dialog_fail;

    bool m_continue;

    bool execCommand(const QString __command,
                     const QStringList __args = QStringList()) const;

signals:

private slots:
    void mainDialog();
    void succDialog();
    void failDialog();
};

#endif // XDIALOG_H
