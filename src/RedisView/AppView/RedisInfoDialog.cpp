/**
* @file      RedisInfoDialog.cpp
* @brief     Redis服务端信息与分析
* @author    王长春
* @date      2019-05-30
* @version   001
* @copyright Copyright (c) 2018
*/
#include "RedisInfoDialog.h"
#include "ui_redisinfodialog.h"

RedisInfoDialog::RedisInfoDialog(RedisCluster *redisClient,
                                 QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RedisInfoDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("实例信息"));
    _redisClient = redisClient;
    setFixedSize(this->size());

    //    QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    //    resize(availableGeometry.width() * 5 / 7, availableGeometry.height() * 5 / 7);
    //    move((availableGeometry.width() - width()) / 2,
    //         (availableGeometry.height() - height()) / 2);

    if(_redisClient) {
        QString clientInfo;
        _vClients.clear();
        _vMasterClients.clear();
        _vClients = _redisClient->getClients(false);
        _isClusterMode = _redisClient->getClusterMode();
        _isReplicationMode = _redisClient->getReplicationMode();

        for(int j = 0; j < _vClients.size(); ++j) {
            _vClients[j]._client = nullptr;
            clientInfo = QString("%1:%2:%3")
                    .arg(_vClients[j]._host)
                    .arg(_vClients[j]._port)
                    .arg(_vClients[j]._master ? "Master" : "Slave");
            ui->_hostComboBox->addItem(clientInfo);
        }

        if(_isClusterMode) {
            _vMasterClients = _redisClient->getClients(true);
            for(int i = 0; i < _vMasterClients.size(); ++i) {
                _vMasterClients[i]._client = nullptr;
            }
        }

        ui->_itemComboBox->addItem("All");
        ui->_itemComboBox->addItem("Dbsize");
        ui->_itemComboBox->addItem("Analysis");
        ui->_itemComboBox->addItem("Clusterinfo");
        ui->_itemComboBox->addItem("Server");
        ui->_itemComboBox->addItem("Clients");
        ui->_itemComboBox->addItem("Memory");
        ui->_itemComboBox->addItem("Persistence");
        ui->_itemComboBox->addItem("Stats");
        ui->_itemComboBox->addItem("Replication");
        ui->_itemComboBox->addItem("CPU");
        ui->_itemComboBox->addItem("Cluster");
        ui->_itemComboBox->addItem("Keyspace");
    }
}

RedisInfoDialog::~RedisInfoDialog()
{
    delete ui;
}

void RedisInfoDialog::on__queryPushButton_clicked()
{
    if(!_redisClient) {
        QMessageBox::information(this, tr("错误"), tr("客户端连接为空!"));
        return;
    }

    if(ui->_hostComboBox->count() <= 0 ||
            ui->_itemComboBox->count() <= 0)
        return;

    int iRet = 0;
    qlonglong llRet = 0;
    _cmdRsult.clear();
    _byteArray.clear();
    ui->_textBrowser->clear();
    _strCmd = ui->_itemComboBox->currentText();

    if(_strCmd == "Dbsize") {
        if(!_redisClient->dbsize(llRet))
            llRet = -1;
        _appendInfo = QString("# Dbsize\r\nall_master_total_key:%1\r\n").arg(llRet);
        ui->_textBrowser->insertPlainText(_appendInfo);
        return;
    } else if(_strCmd == "Analysis") {
        int index = -1;
        QMap<QString,QString> infoMap;
        QStringList infoList;
        for(int i = 0; i < _vClients.size(); ++i) {
            if(!_vClients[i]._master)
                continue;
            _byteArray.clear();
            infoMap.clear();
            _cmdRsult.clear();
            _strCmd = "info";
            _cmdRsult = _redisClient->command(_strCmd, "", i);
            if(_cmdRsult.size() <= 0)
                continue;
            _redisClient->formatToText(_cmdRsult[0], _byteArray , iRet);
            _strCmd = QTextCodec::codecForLocale()->toUnicode(_byteArray);
            infoList = _strCmd.split("\r\n",QString::SkipEmptyParts);
            for(int j = 0; j < infoList.size(); ++j) {
                index = infoList[j].indexOf(":");
                if(index == -1) {
                    infoMap[infoList[j]] = "nil";
                    continue;
                }
                infoMap[infoList[j].mid(0, index)] = infoList[j].mid(index + 1);
            }
            _appendInfo = QString("# Analysis");
            _appendInfo += QString("\r\n");
            _appendInfo += tr("主机实例:");
            _appendInfo += QString("%1:%2").arg(_vClients[i]._host).arg(_vClients[i]._port);
            _appendInfo += QString("\r\n");
            _appendInfo += tr("最大内存:");
            _appendInfo += QString("%1").arg(infoMap.value("maxmemory","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("使用内存:");
            _appendInfo += QString("%1").arg(infoMap.value("used_memory","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("内存碎片率:");
            _appendInfo += QString("%1").arg(infoMap.value("mem_fragmentation_ratio","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("客户端连接数:");
            _appendInfo += QString("%1").arg(infoMap.value("connected_clients","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("拒绝连接请求数:");
            _appendInfo += QString("%1").arg(infoMap.value("rejected_connections","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("每秒执行命令数:");
            _appendInfo += QString("%1").arg(infoMap.value("instantaneous_ops_per_sec","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("内存限制被删除键数:");
            _appendInfo += QString("%1").arg(infoMap.value("evicted_keys","null"));
            _appendInfo += QString("\r\n");
            _appendInfo += tr("连接从实例数:");
            _appendInfo += QString("%1").arg(infoMap.value("connected_slaves","null"));
            _appendInfo += QString("\r\n");
            if(infoMap.value("evicted_keys","0").toLongLong() > 0) {
                _appendInfo += QString("\r\n");
                _appendInfo += tr("当evicted_keys>0时,表示碎片过多或空间不足,可以通过碎片整理、增加maxmemory参数值或进行集群扩容解决!");
            }
            if(infoMap.value("rejected_connections","0").toLongLong() > 0) {
                _appendInfo += QString("\r\n");
                _appendInfo += tr("当rejected_connections>0时,表示客户端数量过多,请减小不必要的应用或通过增加maxclients参数值解决!");
            }
            if(infoMap.value("instantaneous_ops_per_sec","0").toLongLong() > 12000) {
                _appendInfo += QString("\r\n");
                _appendInfo += tr("当instantaneous_ops_per_sec>12000时,表示该实例压力较大,可以通过slot迁移或进行集群扩容解决!");
            }
            if(infoMap.value("maxmemory","-1").toLongLong() > 0) {
                if(infoMap.value("used_memory","0").toLongLong() * 100 / infoMap.value("maxmemory","-1").toLongLong() > 60) {
                    _appendInfo += QString("\r\n");
                    _appendInfo += tr("当内存使用率大于60%时,可以通过数据清理、slot迁移或进行集群扩容解决!");
                }
            }
            if((infoMap.value("used_memory","0").toLongLong()
                * infoMap.value("mem_fragmentation_ratio","0").toInt()
                + infoMap.value("used_memory","0").toLongLong())
                    > infoMap.value("maxmemory","0").toLongLong()) {
                _appendInfo += QString("\r\n");
                _appendInfo += tr("当使用内存*(1+内存碎片率)>最大内存时,表示碎片过多或空间不足,可以通过碎片整理、增加maxmemory参数值或进行集群扩容解决!");
            }
            ui->_textBrowser->append(_appendInfo);
            ui->_textBrowser->append("\r\n");
        }

        if(_isClusterMode) {
            _byteArray.clear();
            infoList.clear();
            infoMap.clear();
            _strCmd = "cluster info";
            _cmdRsult = _redisClient->command(_strCmd, "", ui->_hostComboBox->currentIndex());
            if(_cmdRsult.size() <= 0)
                return;
            _redisClient->formatToText(_cmdRsult[0], _byteArray , iRet);
            _strCmd = QTextCodec::codecForLocale()->toUnicode(_byteArray);
            infoList = _strCmd.split("\r\n",QString::SkipEmptyParts);
            for(int k = 0; k < infoList.size(); ++k) {
                index = infoList[k].indexOf(":");
                if(index == -1) {
                    infoMap[infoList[k]] = "nil";
                    continue;
                }
                infoMap[infoList[k].mid(0, index)] = infoList[k].mid(index + 1);
            }
            if(infoMap.value("cluster_state","no") == "ok") {
                _appendInfo = tr("集群状态:正常");
            } else {
                _appendInfo = tr("集群状态:异常");
            }

            for(int m = 0; m < _vMasterClients.size(); ++m) {
                index = 0;
                for(int n = 0; n < _vMasterClients.size(); ++n) {
                    if(_vMasterClients[m]._host == _vMasterClients[n]._host)
                        ++index;
                }
                if(index * 2 >= _vMasterClients.size()) {
                    _appendInfo += "\r\n";
                    _appendInfo += tr("主实例分布:异常");
                    _appendInfo += "\r\n";
                    _appendInfo += tr("超过半数主实例分布在");
                    _appendInfo += QString("%1").arg(_vMasterClients[m]._host);
                    _appendInfo += tr("主机,若发生宕机无法切主，请修复!");
                    break;
                }
            }
            _appendInfo += "\r\n";
            ui->_textBrowser->append(_appendInfo);
        }
        return;
    } else if(_strCmd == "Clusterinfo") {
        if(!_isClusterMode) {
            ui->_textBrowser->append(tr("非集群模式"));
            return;
        }
        _strCmd = "cluster info";
    } else if(_strCmd == "All") {
        _strCmd = "info";
    } else {
        _strCmd = QString("info %1").arg(_strCmd);
    }

    _cmdRsult = _redisClient->command(_strCmd, "", ui->_hostComboBox->currentIndex());
    for(int i = 0; i < _cmdRsult.size(); ++i) {
        _redisClient->formatToText(_cmdRsult[i], _byteArray , iRet);
        _strCmd = QTextCodec::codecForLocale()->toUnicode(_byteArray);
        ui->_textBrowser->append(_strCmd);
    }

    if(ui->_itemComboBox->currentText() == "All") {
        if(!_redisClient->dbsize(llRet))
            llRet = -1;
        _appendInfo = QString("# Dbsize\r\nall_master_total_key:%1\r\n").arg(llRet);
        ui->_textBrowser->insertPlainText(_appendInfo);
    }
}

void RedisInfoDialog::on__exitPushButton_clicked()
{
    accept();
}

void RedisInfoDialog::on__hostComboBox_currentTextChanged(const QString &arg1)
{
    on__queryPushButton_clicked();
    Q_UNUSED(arg1);
}

void RedisInfoDialog::on__itemComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Dbsize" || arg1 == "Analysis" || arg1 == "Clusterinfo")
        ui->_hostComboBox->setEnabled(false);
    else
        ui->_hostComboBox->setEnabled(true);
    on__queryPushButton_clicked();
}

void RedisInfoDialog::on__textBrowser_textChanged()
{
    ui->_textBrowser->moveCursor(QTextCursor::End);
}
