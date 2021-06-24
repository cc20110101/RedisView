/**
* @file      PubsubDialog.h
* @brief     订阅发布模式
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
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

    void on__comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::PubsubDialog *ui;
    RedisClient *_redisClient;
};

#endif // PUBSUBDIALOG_H
