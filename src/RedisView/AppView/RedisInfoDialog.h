/**
* @file      RedisInfoDialog.h
* @brief     Redis服务端信息与分析
* @author    王长春
* @date      2019-05-30
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef REDISINFODIALOG_H
#define REDISINFODIALOG_H

#include "Public/Define.h"

namespace Ui {
class RedisInfoDialog;
}

class RedisInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RedisInfoDialog(RedisCluster *redisClient = nullptr,
                             QWidget *parent = nullptr);
    ~RedisInfoDialog();

private slots:

    void on__queryPushButton_clicked();
    void on__exitPushButton_clicked();
    void on__hostComboBox_currentTextChanged(const QString &arg1);
    void on__itemComboBox_currentTextChanged(const QString &arg1);
    void on__textBrowser_textChanged();

private:

    bool _isClusterMode;
    bool _isReplicationMode;
    QString _strCmd;
    QString _appendInfo;
    QByteArray _byteArray;
    RedisCluster *_redisClient;
    Ui::RedisInfoDialog *ui;
    QList<QByteArray> _cmdRsult;
    QList<ClusterClient> _vClients;
};

#endif // REDISINFODIALOG_H
