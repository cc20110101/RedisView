/**
* @file      BatchOperateDialog.cpp
* @brief     批量操作
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "BatchOperateDialog.h"
#include "ui_batchoperatedialog.h"

BatchOperateDialog::BatchOperateDialog(RedisCluster *redisClient, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchOperateDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("批量操作"));
    _vTaskId.clear();
    _isRun = false;
    _workThread = nullptr;
    _redisCluster = redisClient;
    _isCalculateTimeout = PubLib::getConfigB("calculateTimeOut",true);
    ui->_batchComboBox->addItem("Delete");
    ui->_batchComboBox->addItem("Scan key");
    ui->_batchComboBox->addItem("Import from oracle");
    ui->_batchComboBox->addItem("Import from mysql");
    ui->_batchComboBox->addItem("Export to oracle");
    ui->_batchComboBox->addItem("Export to mysql");
    ui->_batchComboBox->addItem("Delete oracle key");
    ui->_batchComboBox->addItem("Delete mysql key");
    ui->_progressBar->setMinimum(0);
    ui->_progressBar->setMaximum(100);
    ui->_progressBar->setValue(0);
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->_patternCountLineEdit);
    ui->_patternCountLineEdit->setValidator(validator);

    ui->_patternCountLineEdit->setPlaceholderText("2");
    ui->_patternSeparatorLineEdit->setPlaceholderText("|");
    ui->_patternlineEdit->setPlaceholderText("a*|b*");
    ui->_tablelineEdit->setPlaceholderText("database table");

    if(_redisCluster) {
        _isClusterMode = _redisCluster->getClusterMode();
        if(_isClusterMode) {
            _idbNums = 1;
        } else {
            if(!_redisCluster->getDbNum(_idbNums)) {
                _idbNums = 1;
                QMessageBox::information(this, tr("错误"), tr("获取DB个数失败!"));
            }
        }
        _vMasterClients = _redisCluster->getClients(true);
        for(int i = 0; i < _vMasterClients.size(); ++i) {
            _vMasterClients[i]._client = nullptr;
        }
    }

    _threadPool = QThreadPool::globalInstance();
    _threadPool->setMaxThreadCount(MAX_THREAD_COUNT);
    _threadPool->setExpiryTimeout(5000); //5s

    connect(ui->_batchComboBox, SIGNAL(currentTextChanged(const QString)), this,
            SLOT(changeOperate(const QString)));
    emit ui->_batchComboBox->currentTextChanged(ui->_batchComboBox->currentText());
}

BatchOperateDialog::~BatchOperateDialog()
{
    delete ui;
}

void BatchOperateDialog::changeOperate(const QString operate) {
    _operate = operate;
    if(operate == "Delete") {
        ui->_textBrowser->setPlaceholderText(tr("删除以a或b开头的键"));
        ui->_textBrowser->clear();
        ui->_tablelineEdit->setEnabled(false);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(false);
        ui->_patternCountLineEdit->setEnabled(true);
        ui->_patternSeparatorLineEdit->setEnabled(true);
        ui->_patternlineEdit->setEnabled(true);
        _dbType = -1;
    } else if(operate == "Scan key") {
        ui->_textBrowser->setPlaceholderText(tr("扫描以a或b开头的键"));
        ui->_textBrowser->clear();
        ui->_tablelineEdit->setEnabled(false);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(false);
        ui->_patternCountLineEdit->setEnabled(true);
        ui->_patternSeparatorLineEdit->setEnabled(true);
        ui->_patternlineEdit->setEnabled(true);
        _dbType = -1;
    } else if(operate == "Import from oracle") {
        ui->_textBrowser->setPlaceholderText(tr("从ORACLE表导入键值"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(true);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(false);
        ui->_patternSeparatorLineEdit->setEnabled(false);
        ui->_patternlineEdit->setEnabled(false);
        ui->_radioButton->setChecked(true);
        _dbType = ORACLE_DB;
    } else if(operate == "Export to oracle") {
        ui->_textBrowser->setPlaceholderText(tr("导出a或b开头键值到ORACLE表"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(true);
        ui->_patternSeparatorLineEdit->setEnabled(true);
        ui->_patternlineEdit->setEnabled(true);
        _dbType = ORACLE_DB;
    } else if(operate == "Import from mysql") {
        ui->_textBrowser->setPlaceholderText(tr("从MYSQL表导入键值"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(true);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(false);
        ui->_patternSeparatorLineEdit->setEnabled(false);
        ui->_patternlineEdit->setEnabled(false);
        ui->_radioButton->setChecked(true);
        _dbType = MYSQL_DB;
    } else if(operate == "Export to mysql") {
        ui->_textBrowser->setPlaceholderText(tr("导出a或b开头键值到MYSQ表"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(true);
        ui->_patternSeparatorLineEdit->setEnabled(true);
        ui->_patternlineEdit->setEnabled(true);
        _dbType = MYSQL_DB;
    } else if(operate == "Delete oracle key") {
        ui->_textBrowser->setPlaceholderText(tr("删除ORACLE表对应键值的键"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(false);
        ui->_patternSeparatorLineEdit->setEnabled(false);
        ui->_patternlineEdit->setEnabled(false);
        _dbType = ORACLE_DB;
    } else if(operate == "Delete mysql key") {
        ui->_textBrowser->setPlaceholderText(tr("删除MYSQL表对应键值的键"));
        ui->_tablelineEdit->setEnabled(true);
        ui->_radioButton->setEnabled(false);
        ui->_setPushButton->setEnabled(true);
        ui->_patternCountLineEdit->setEnabled(false);
        ui->_patternSeparatorLineEdit->setEnabled(false);
        ui->_patternlineEdit->setEnabled(false);
        _dbType = MYSQL_DB;
    }
}

void BatchOperateDialog::recvData(const TaskMsg taskMsg) {
    if(taskMsg._taskid == THREAD_BATCH_DEL_KEY_TASK) {
        _TASK_LOCK.lockForWrite();
        _NowKeyNum += taskMsg._sequence;
        if(_KeyNum == 0) {
            _processValue = 100;
        } else {
            _processValue = _NowKeyNum * 100 / _KeyNum;
            _processValue = _processValue > 100 ? 100 : _processValue;
            _processValue = _processValue < 0 ? 0 : _processValue;
        }
        ui->_progressBar->setValue(_processValue);
        ui->_textBrowser->moveCursor(QTextCursor::End);
        QTextCursor cursor=ui->_textBrowser->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        ui->_textBrowser->append(QString("%1 keys have been deleted").arg(_NowKeyNum));
        _TASK_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_BATCH_SCAN_KEY_TASK) {
        _TASK_LOCK.lockForWrite();
        _NowKeyNum += taskMsg._sequence;
        if(_KeyNum == 0) {
            _processValue = 100;
        } else {
            _processValue = _NowKeyNum * 100 / _KeyNum;
            _processValue = _processValue > 100 ? 100 : _processValue;
            _processValue = _processValue < 0 ? 0 : _processValue;
        }
        ui->_progressBar->setValue(_processValue);
        ui->_textBrowser->moveCursor(QTextCursor::End);
        QTextCursor cursor=ui->_textBrowser->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        ui->_textBrowser->append(QString("%1 keys have been scaned").arg(_NowKeyNum));
        _TASK_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_BATCH_OEM_KEY_TASK ||
              taskMsg._taskid == THREAD_BATCH_MEM_KEY_TASK) {
        _TASK_LOCK.lockForWrite();
        _NowKeyNum += taskMsg._sequence;
        if(_KeyNum == 0) {
            _processValue = 100;
        } else {
            _processValue = _NowKeyNum * 100 / _KeyNum;
            _processValue = _processValue > 100 ? 100 : _processValue;
            _processValue = _processValue < 0 ? 0 : _processValue;
        }
        ui->_progressBar->setValue(_processValue);
        ui->_textBrowser->moveCursor(QTextCursor::End);
        QTextCursor cursor=ui->_textBrowser->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        ui->_textBrowser->append(QString("%1 keys have been export").arg(_NowKeyNum));
        _TASK_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_BATCH_OIM_KEY_TASK ||
              taskMsg._taskid == THREAD_BATCH_MIM_KEY_TASK) {
        _TASK_LOCK.lockForWrite();
        _NowKeyNum += taskMsg._sequence;
        if(_KeyNum == 0) {
            _processValue = 100;
        } else {
            _processValue = _NowKeyNum * 100 / _KeyNum;
            _processValue = _processValue > 100 ? 100 : _processValue;
            _processValue = _processValue < 0 ? 0 : _processValue;
        }
        ui->_progressBar->setValue(_processValue);
        ui->_textBrowser->moveCursor(QTextCursor::End);
        QTextCursor cursor=ui->_textBrowser->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        ui->_textBrowser->append(QString("%1 key value have been import").arg(_NowKeyNum));
        _TASK_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_BATCH_MDE_KEY_TASK ||
              taskMsg._taskid == THREAD_BATCH_ODE_KEY_TASK) {
        _TASK_LOCK.lockForWrite();
        _NowKeyNum += taskMsg._sequence;
        if(_KeyNum == 0) {
            _processValue = 100;
        } else {
            _processValue = _NowKeyNum * 100 / _KeyNum;
            _processValue = _processValue > 100 ? 100 : _processValue;
            _processValue = _processValue < 0 ? 0 : _processValue;
        }
        ui->_progressBar->setValue(_processValue);
        ui->_textBrowser->moveCursor(QTextCursor::End);
        QTextCursor cursor=ui->_textBrowser->textCursor();
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        ui->_textBrowser->append(QString("%1 keys have been delete").arg(_NowKeyNum));
        _TASK_LOCK.unlock();
    }
}

void BatchOperateDialog::runError(const int taskid, const QString & error) {
    //if(taskid == THREAD_BATCH_DEL_KEY_TASK)
    QMessageBox::critical(this, tr("错误"), error);
    Q_UNUSED(taskid)
}

void BatchOperateDialog::finishWork(const int taskid) {
    _vTaskId.removeOne(taskid);
    if(taskid == THREAD_BATCH_DEL_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End delete key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_SCAN_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End scan key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_OIM_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End oracle import key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_MIM_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End mysql import key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_OEM_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End oracle emport key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_MEM_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End mysql emport key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_MDE_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End delete mysql key").arg(_NowKeyNum));
        }
    } else if(taskid == THREAD_BATCH_ODE_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            ui->_okPushButton->setEnabled(true);
            ui->_batchComboBox->setEnabled(true);
            _isRun = false;
            ui->_progressBar->setValue(ui->_progressBar->maximum());
            ui->_textBrowser->append("");
            ui->_textBrowser->append(QString("[%1] %2,total num is %3...")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg("End delete oracle key").arg(_NowKeyNum));
        }
    }
}

void BatchOperateDialog::on__cancelPushButton_clicked()
{
    if(!_isRun)
        return;
    if(_operate == "Delete")
        emit cancelWork(THREAD_BATCH_DEL_KEY_TASK);
    else if(_operate == "Scan key")
        emit cancelWork(THREAD_BATCH_SCAN_KEY_TASK);
    else if(_operate == "Import from oracle")
        emit cancelWork(THREAD_BATCH_OIM_KEY_TASK);
    else if(_operate == "Import from mysql")
        emit cancelWork(THREAD_BATCH_MIM_KEY_TASK);
    else if(_operate == "Export to oracle")
        emit cancelWork(THREAD_BATCH_OEM_KEY_TASK);
    else if(_operate == "Export to mysql")
        emit cancelWork(THREAD_BATCH_MEM_KEY_TASK);
    else if(_operate == "Delete mysql key")
        emit cancelWork(THREAD_BATCH_MDE_KEY_TASK);
    else if(_operate == "Delete oracle key")
        emit cancelWork(THREAD_BATCH_ODE_KEY_TASK);
}

void BatchOperateDialog::on__okPushButton_clicked()
{
    if(_vTaskId.size() > 0) {
        QMessageBox::information(this, tr("错误"), tr("存在未完成任务,稍后重试!"));
        return;
    }

    if(_vMasterClients.size() <= 0) {
        QMessageBox::information(this, tr("错误"), tr("客户端主实例连接为空!"));
        return;
    }

    if(!_redisCluster) {
        QMessageBox::information(this, tr("错误"), tr("客户端连接为空!"));
        return;
    }

    if(ui->_patternCountLineEdit->isEnabled()) {
        _patternCount = ui->_patternCountLineEdit->text().toInt();
        if(_patternCount <= 0) {
            QMessageBox::information(this, tr("错误"), tr("键模式个数最小为1！"));
            return;
        }

        _patternSeparator = ui->_patternSeparatorLineEdit->text();
        if(_patternCount > 1 && _patternSeparator.isEmpty()) {
            QMessageBox::information(this, tr("错误"), tr("键模式分隔符不可为空！"));
            return;
        }

        _keyPattern = ui->_patternlineEdit->text();
        QStringList vKeyPattern = _keyPattern.split(_patternSeparator,QString::SkipEmptyParts);
        if(vKeyPattern.size() != _patternCount) {
            QMessageBox::information(this, tr("错误"), tr("键模式与个数不匹配！"));
            return;
        }

        if(!_redisCluster->dbsize(_KeyNum)) {
            QMessageBox::information(this, tr("错误"), tr("获取数据库键总数失败!"));
            return;
        }
    }

    _tableName.clear();
    if(ui->_tablelineEdit->isEnabled()) {
        _tableName = ui->_tablelineEdit->text().trimmed();
        if(_tableName.isEmpty()) {
            QMessageBox::information(this, tr("错误"), tr("数据库表不可为空！"));
            return;
        }

        if(PubLib::getConfig(QString("lastUseTag%1").arg(_dbType))
                .trimmed().isEmpty()) {
            QMessageBox::information(this, tr("错误"), tr("数据库连接信息未配置！"));
            return;
        }
    }

    _vTaskId.clear();
    _isRun = true;
    _NowKeyNum = 0;
    ui->_textBrowser->clear();
    ui->_okPushButton->setEnabled(false);
    ui->_batchComboBox->setEnabled(false);
    _operate = ui->_batchComboBox->currentText();
    if(_operate == "Delete") {
        _nowOperate = THREAD_BATCH_DEL_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin delete key..."));
    } else if(_operate == "Scan key") {
        _nowOperate = THREAD_BATCH_SCAN_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin scan key..."));
    } else if(_operate == "Import from oracle") {
        _nowOperate = THREAD_BATCH_OIM_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin import key from oracle..."));
    } else if(_operate == "Export to oracle") {
        _nowOperate = THREAD_BATCH_OEM_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin emport key to oracle..."));
    } else if(_operate == "Import from mysql") {
        _nowOperate = THREAD_BATCH_MIM_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin import key from mysql..."));
    } else if(_operate == "Export to mysql") {
        _nowOperate = THREAD_BATCH_MEM_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin export key to mysql..."));
    } else if(_operate == "Delete oracle key") {
        _nowOperate = THREAD_BATCH_ODE_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin delete key from oracle..."));
    } else if(_operate == "Delete mysql key") {
        _nowOperate = THREAD_BATCH_MDE_KEY_TASK;
        ui->_textBrowser->append(QString("[%1] %2")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                 .arg("Begin delete key from mysql..."));
    }
    ui->_textBrowser->append("");

    if(!checkTable()) {
        _isRun = false;
        ui->_textBrowser->clear();
        ui->_okPushButton->setEnabled(true);
        ui->_batchComboBox->setEnabled(true);
        return;
    }

    if(_nowOperate == THREAD_BATCH_DEL_KEY_TASK ||
            _nowOperate == THREAD_BATCH_SCAN_KEY_TASK ||
            _nowOperate == THREAD_BATCH_OEM_KEY_TASK ||
            _nowOperate == THREAD_BATCH_MEM_KEY_TASK) {
        for(int i = 0; i < _vMasterClients.size(); ++i) {
            if(_isClusterMode) {
                _taskMsg = new TaskMsg();
                _taskMsg->_taskid = _nowOperate;
                _vTaskId.push_back(_taskMsg->_taskid);
                _taskMsg->_sequence = i;
                _taskMsg->_host = _vMasterClients[i]._host;
                _taskMsg->_port = _vMasterClients[i]._port;
                _taskMsg->_passwd = _vMasterClients[i]._passwd;
                _taskMsg->_key = _patternSeparator;
                _taskMsg->_keyPattern = _keyPattern;
                _taskMsg->_dbIndex = 0;
                _taskMsg->_tableName = _tableName;
                _workThread = new WorkThread(_taskMsg);

                connect(_workThread, &WorkThread::sendData, this, &BatchOperateDialog::recvData);
                connect(_workThread, &WorkThread::finishWork, this, &BatchOperateDialog::finishWork);
                connect(_workThread, &WorkThread::runError, this, &BatchOperateDialog::runError);
                connect(this, &BatchOperateDialog::cancelWork, _workThread, &WorkThread::cancelWork);

                _threadPool->start(_workThread);
                _taskMsg = nullptr;
                _workThread = nullptr;
            } else {
                for(int j = 0; j < _idbNums; ++j) {
                    _taskMsg = new TaskMsg();
                    _taskMsg->_taskid = _nowOperate;
                    _vTaskId.push_back(_taskMsg->_taskid);
                    _taskMsg->_sequence = i;
                    _taskMsg->_host = _vMasterClients[i]._host;
                    _taskMsg->_port = _vMasterClients[i]._port;
                    _taskMsg->_passwd = _vMasterClients[i]._passwd;
                    _taskMsg->_key = _patternSeparator;
                    _taskMsg->_keyPattern = _keyPattern;
                    _taskMsg->_dbIndex = j;
                    _taskMsg->_tableName = _tableName;
                    _workThread = new WorkThread(_taskMsg);

                    connect(_workThread, &WorkThread::sendData, this, &BatchOperateDialog::recvData);
                    connect(_workThread, &WorkThread::finishWork, this, &BatchOperateDialog::finishWork);
                    connect(_workThread, &WorkThread::runError, this, &BatchOperateDialog::runError);
                    connect(this, &BatchOperateDialog::cancelWork, _workThread, &WorkThread::cancelWork);

                    _threadPool->start(_workThread);
                    _taskMsg = nullptr;
                    _workThread = nullptr;
                }
            }
        }
    } else if(_nowOperate == THREAD_BATCH_OIM_KEY_TASK ||
              _nowOperate == THREAD_BATCH_MIM_KEY_TASK ||
              _nowOperate == THREAD_BATCH_ODE_KEY_TASK ||
              _nowOperate == THREAD_BATCH_MDE_KEY_TASK) {
        for(int k = 0; k < MAX_THREAD_COUNT; ++k) {
            _taskMsg = new TaskMsg();
            _taskMsg->_taskid = _nowOperate;
            _vTaskId.push_back(_taskMsg->_taskid);
            _taskMsg->_sequence = k;
            _taskMsg->_host = _vMasterClients[0]._host;
            _taskMsg->_port = _vMasterClients[0]._port;
            _taskMsg->_passwd = _vMasterClients[0]._passwd;
            _taskMsg->_clientIndex = _isClusterMode;
            if(ui->_radioButton->isEnabled() &&
                    ui->_radioButton->isChecked())
                _taskMsg->_dbIndex = 1;
            else
                _taskMsg->_dbIndex = 0;
            _taskMsg->_tableName = _tableName;
            _workThread = new WorkThread(_taskMsg);

            connect(_workThread, &WorkThread::sendData, this, &BatchOperateDialog::recvData);
            connect(_workThread, &WorkThread::finishWork, this, &BatchOperateDialog::finishWork);
            connect(_workThread, &WorkThread::runError, this, &BatchOperateDialog::runError);
            connect(this, &BatchOperateDialog::cancelWork, _workThread, &WorkThread::cancelWork);

            _threadPool->start(_workThread);
            _taskMsg = nullptr;
            _workThread = nullptr;
        }
    }
}

bool BatchOperateDialog::checkTable() {

    if(_tableName.isEmpty())
        return true;

    int count = 0;
    QString sql;
    DbMgr dbMgr;
    QSqlDatabase *db = new QSqlDatabase();

    if(_nowOperate == THREAD_BATCH_OIM_KEY_TASK ||
            _nowOperate == THREAD_BATCH_OEM_KEY_TASK ||
            _nowOperate == THREAD_BATCH_ODE_KEY_TASK) {
        dbMgr.setCfg(ORACLE_DB, ORACLE_DRIVE);
        sql = QString("select count(1) from user_tables where "
                      "table_name = '%1'").arg(_tableName);
        if(!dbMgr.getDb(db)) {
            QMessageBox::information(this, tr("错误"), dbMgr.error());
            return false;
        }

        QSqlQuery sql_query(*db);
        if(!sql_query.exec(sql)) {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            sql_query.clear();
            return false;
        }

        if(sql_query.next()) {
            count = sql_query.value(0).toInt();
        } else {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            return false;
        }
        sql_query.clear();

        if(count == 0) {
            sql = QString("CREATE TABLE %1 (ID NUMBER(16) PRIMARY KEY,KEY VARCHAR2(100), "
                          "KEY_TYPE VARCHAR2(8), FILED VARCHAR2(100), VALUE VARCHAR2(200), "
                          "TIME_OUT NUMBER(16), WEIGHT NUMBER(16), EXP_DATE DATE, DB_INDEX "
                          "NUMBER(9), STATE NUMBER(9))").arg(_tableName);
            if(!sql_query.exec(sql)) {
                QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
                sql_query.clear();
                return false;
            }
            sql_query.clear();
        }

        sql = QString("SELECT MAX(ID) FROM %1").arg(_tableName);
        if(!sql_query.exec(sql)) {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            sql_query.clear();
            return false;
        }

        if(sql_query.next()) {
            PubLib::setSequenceId(sql_query.value(0).toLongLong());
        } else {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            return false;
        }
        sql_query.clear();
    } else if(_nowOperate == THREAD_BATCH_MIM_KEY_TASK ||
              _nowOperate == THREAD_BATCH_MEM_KEY_TASK ||
              _nowOperate == THREAD_BATCH_MDE_KEY_TASK) {
        dbMgr.setCfg(MYSQL_DB, MYSQL_DRIVE);
        sql = QString("SELECT count(1) FROM information_schema.TABLES WHERE "
                      "table_name ='%1' and table_schema='%2'")
                .arg(_tableName).arg(dbMgr.database());
        if(!dbMgr.getDb(db)) {
            QMessageBox::information(this, tr("错误"), dbMgr.error());
            return false;
        }

        QSqlQuery sql_query(*db);
        if(!sql_query.exec(sql)) {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            sql_query.clear();
            return false;
        }

        if(sql_query.next()) {
            count = sql_query.value(0).toInt();
        } else {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            return false;
        }
        sql_query.clear();

        if(count == 0) {
            sql = QString("CREATE TABLE `%1` (`ID` int(16) primary key, `KEY` "
                          "varchar(100), `KEY_TYPE` varchar(8), `FILED` varchar(100),"
                          " `VALUE` varchar(100), `TIME_OUT` int(16), `WEIGHT` int(16),"
                          " `EXP_DATE` datetime, `DB_INDEX` int(8), `STATE` int(1))")
                    .arg(_tableName);
            if(!sql_query.exec(sql)) {
                QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
                sql_query.clear();
                return false;
            }
            sql_query.clear();
        }

        sql = QString("select max(id) from %1").arg(_tableName);
        if(!sql_query.exec(sql)) {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            sql_query.clear();
            return false;
        }

        if(sql_query.next()) {
            PubLib::setSequenceId(sql_query.value(0).toLongLong());
        } else {
            QMessageBox::information(this, tr("错误"), sql_query.lastError().text());
            return false;
        }
        sql_query.clear();
    }

    delete db;
    db = nullptr;
    dbMgr.close();
    return true;
}

void BatchOperateDialog::on__exitPushButton_clicked()
{
    if(_isRun) {
        if(QMessageBox::Yes != QMessageBox::question(this, tr("确认"), tr("任务未结束，确定退出么？")))
            return;
        on__cancelPushButton_clicked();
    }
    accept();
}

void BatchOperateDialog::on__setPushButton_clicked()
{
    if(_operate == "Delete oracle key" ||
            _operate == "Export to oracle" ||
            _operate == "Import from oracle") {
        DbCfgDialog dbcfg(0);
        dbcfg.exec();
    } else {
        DbCfgDialog dbcfg(1);
        dbcfg.exec();
    }
}

void BatchOperateDialog::on__radioButton_toggled(bool checked)
{
    _isCalculateTimeout = checked;
    PubLib::setConfigB("calculateTimeOut",_isCalculateTimeout);
}
