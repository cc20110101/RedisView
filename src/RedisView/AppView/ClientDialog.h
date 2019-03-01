#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include "Public/Define.h"

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(RedisCluster *redisClient = nullptr, QWidget *parent = 0);
    ~ClientDialog();

private slots:
    void on__pushButton_clicked();

private:
    Ui::ClientDialog *ui;
};

#endif // CLIENTDIALOG_H
