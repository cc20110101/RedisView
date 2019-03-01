#ifndef PUBSUBDIALOG_H
#define PUBSUBDIALOG_H

#include "Public/Define.h"

namespace Ui {
class PubsubDialog;
}

class PubsubDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PubsubDialog(RedisClient *redisClient = nullptr, QWidget *parent = nullptr);
    ~PubsubDialog();

private slots:
    void on__cancelButton_clicked();

    void on__queryButton_clicked();

    void on__comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::PubsubDialog *ui;
    RedisClient *_redisClient;
};

#endif // PUBSUBDIALOG_H
