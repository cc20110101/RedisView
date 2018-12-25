#include "MainWidget.h"

MainWidget::MainWidget(RedisCluster *redisClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    initSet(redisClient);
    initView();
    initKeyView();
    //大数据量卡顿，暂时不初始化key列表，手动刷新获取数据
    //initKeyListDat();

    connect(_mCount, &QAction::triggered, this, &MainWidget::count);
    connect(_mRefresh, &QAction::triggered, this, &MainWidget::flush);
    connect(_mCreated, &QAction::triggered, this, &MainWidget::add);
    connect(_mDelete, &QAction::triggered, this, &MainWidget::del);
    connect(_mAlter, &QAction::triggered, this, &MainWidget::alter);
    connect(ui->_treeView, &QTreeView::clicked,this,&MainWidget::treeClicked);
    connect(ui->_treeView, &QTreeView::customContextMenuRequested, this,  &MainWidget::showTreeRightMenu);
    connect(_textBrowser, &QTextBrowser::textChanged, this, &MainWidget::autoScroll);
    connect(_dataView, &DataView::getData, this, &MainWidget::initValueListData);
    connect(_dataView, &DataView::commitData, this, &MainWidget::commitValue);

}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::initSet(RedisCluster *redisClient) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    _isCmdSplit = settings.value("cmdSplit", true).toBool();
    _isFormat = settings.value("format", true).toBool();
    _isResp = settings.value("resp", false).toBool();
    _isText = settings.value("text", true).toBool();
    _isJson = settings.value("json", false).toBool();

    _redisClient = redisClient;
    _strConnectName = _redisClient->getConnectName();
    _isClusterMode = _redisClient->getClusterMode();
    if(_isClusterMode) {
        _idbNums = 1;
    } else {
        if(!_redisClient->getDbNum(_idbNums)) {
            _idbNums = 1;
        }
    }
    vClients = _redisClient->getClients();
    for(int i = 0; i < vClients.size(); ++i) {
        vClients[i]._client = nullptr;
    }

    // 线程池至少会存在一个线程
    _threadPool = QThreadPool::globalInstance(); //全局线程池
    _threadPool->setMaxThreadCount(10); //最大线程数
    _threadPool->setExpiryTimeout(10000); //10s

    _idbIndex = 0;
    _iScanKeySeq = 0;
    _iScanValueSeq = 0;
    _vTaskId.clear();
    _cmdRsult.clear();
    _vStandardItemKey.clear();
}

void MainWidget::initKeyListData(int dbIndex)
{
    _SCAN_KEY_LOCK.lockForWrite();

    ++_iScanKeySeq;
    if(dbIndex == -1) {
        _vStandardItemKey.clear();
        _itemKeyModel->clear();
        _standardItemKey = new QStandardItem(_strConnectName);
        _standardItemKey->setIcon(QIcon(ICON_LONGIN));
        _standardItemKey->setEditable(false);
        _itemKeyModel->appendRow(_standardItemKey);
    } else {
        while(_vStandardItemKey[dbIndex]->rowCount()) {
            _vStandardItemKey[dbIndex]->removeRow(0);
        }
    }

    for(int i = 0; i < vClients.size(); ++i) {
        if(_isClusterMode) {
            _taskMsg = new TaskMsg();
            _taskMsg->_taskid = THREAD_SCAN_KEY_TASK;
            _vTaskId.push_back(THREAD_SCAN_KEY_TASK);
            _taskMsg->_sequence = _iScanKeySeq;
            _taskMsg->_host = vClients[i]._host;
            _taskMsg->_port = vClients[i]._port;
            _taskMsg->_passwd = vClients[i]._passwd;
            _taskMsg->_dbIndex = 0;
            _workThread = new WorkThread(_taskMsg);

            connect(_workThread, &WorkThread::sendData, this, &MainWidget::recvData);
            connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
            connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

            _threadPool->start(_workThread);
            _taskMsg = nullptr;
            _workThread = nullptr;
        } else {
            for(int j = 0; j < _idbNums; ++j) {
                if(dbIndex != -1) {
                    if(dbIndex != j)
                        continue;
                }

                if(dbIndex == -1) {
                    _standardSubItem = new QStandardItem(QString("db%1").arg(j));
                    _standardSubItem->setIcon(QIcon(ICON_DB));
                    _standardSubItem->setEditable(false);
                    _standardItemKey->appendRow(_standardSubItem);
                    _vStandardItemKey << _standardSubItem;
                }

                _taskMsg = new TaskMsg();
                _taskMsg->_taskid = THREAD_SCAN_KEY_TASK;
                _vTaskId.push_back(THREAD_SCAN_KEY_TASK);
                _taskMsg->_sequence = _iScanKeySeq;
                _taskMsg->_host = vClients[i]._host;
                _taskMsg->_port = vClients[i]._port;
                _taskMsg->_passwd = vClients[i]._passwd;
                _taskMsg->_dbIndex = j;
                _workThread = new WorkThread(_taskMsg);

                connect(_workThread, &WorkThread::sendData, this, &MainWidget::recvData);
                connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
                connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

                _threadPool->start(_workThread);
                _taskMsg = nullptr;
                _workThread = nullptr;
            }
        }
    }

    _SCAN_KEY_LOCK.unlock();
}

void MainWidget::initValueListData(const QString &key, const QByteArray &type, const int &clientIndex, const int &dbindex) {

    _SCAN_VALUE_LOCK.lockForWrite();

    ++_iScanValueSeq;
    _taskMsg = new TaskMsg();
    _taskMsg->_taskid = THREAD_SCAN_VALUE_TASK;
    _vTaskId.push_back(THREAD_SCAN_VALUE_TASK);
    _taskMsg->_sequence = _iScanValueSeq;
    _taskMsg->_host = vClients[clientIndex]._host;
    _taskMsg->_port = vClients[clientIndex]._port;
    _taskMsg->_passwd = vClients[clientIndex]._passwd;
    _taskMsg->_clientIndex = clientIndex;
    _taskMsg->_dbIndex = dbindex;
    _taskMsg->_key = key;
    _taskMsg->_type = type;
    _workThread = new WorkThread(_taskMsg);

    connect(_workThread, &WorkThread::sendData, this, &MainWidget::recvData);
    connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
    connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

    _threadPool->start(_workThread);
    _taskMsg = nullptr;
    _workThread = nullptr;

    _SCAN_VALUE_LOCK.unlock();
}

void MainWidget::commitValue(QList<CmdMsg> &cmd) {
    if(cmd.size() <= 0)
        return;

    _taskMsg = new TaskMsg();
    _taskMsg->_taskid = THREAD_COMMIT_VALUE_TASK;
    _vTaskId.push_back(THREAD_COMMIT_VALUE_TASK);
    _taskMsg->_host = vClients[cmd[0]._clientIndex]._host;
    _taskMsg->_port = vClients[cmd[0]._clientIndex]._port;
    _taskMsg->_passwd = vClients[cmd[0]._clientIndex]._passwd;
    _taskMsg->_clientIndex = cmd[0]._clientIndex;
    _taskMsg->_dbIndex = cmd[0]._dbIndex;
    _workThread = new WorkThread(cmd,_taskMsg);

    connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
    connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

    _threadPool->start(_workThread);
    _taskMsg = nullptr;
    _workThread = nullptr;
}

void MainWidget::recvData(const TaskMsg taskMsg) {
    if(taskMsg._taskid == THREAD_SCAN_KEY_TASK) {
        _SCAN_KEY_LOCK.lockForRead();
        if(_iScanKeySeq == taskMsg._sequence) {
            QString strKey;
            for(int i = 0; i < taskMsg._list.size(); ++i) {
                strKey = QTextCodec::codecForLocale()->toUnicode(taskMsg._list[i]);
                _standardSubItem = new QStandardItem(strKey);
                _standardSubItem->setIcon(QIcon(ICON_KEY));
                _standardSubItem->setEditable(false);
                if(_isClusterMode) {
                    _standardItemKey->appendRow(_standardSubItem);
                } else {
                    _vStandardItemKey[taskMsg._dbIndex]->appendRow(_standardSubItem);
                }
            }
        }
        _SCAN_KEY_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_SCAN_VALUE_TASK) {
        _SCAN_VALUE_LOCK.lockForRead();
        if(_iScanValueSeq == taskMsg._sequence) {
            _dataView->appendValue(taskMsg._list, taskMsg._type);
        }
        _SCAN_VALUE_LOCK.unlock();
    }
}

void MainWidget::finishWork(const int taskid) {
    _vTaskId.removeOne(taskid);
    if(taskid == THREAD_SCAN_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            if(_isClusterMode) {
                _standardItemKey->sortChildren(0);
            } else {
                for(int i = 0; i < _vStandardItemKey.size(); ++i) {
                    _vStandardItemKey[i]->sortChildren(0);
                }
            }
        }
    } else if(taskid == THREAD_COMMIT_VALUE_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            _dataView->initValueListData();
        }
    }
}

void MainWidget::initKeyView() {
    _vStandardItemKey.clear();
    _itemKeyModel->clear();
    _standardItemKey = new QStandardItem(_strConnectName);
    _standardItemKey->setIcon(QIcon(ICON_LONGIN));
    _standardItemKey->setEditable(false);
    _itemKeyModel->appendRow(_standardItemKey);

    if(!_isClusterMode) {
        for(int i = 0; i < _idbNums; ++i) {
            _standardSubItem = new QStandardItem(QString("db%1").arg(i));
            _standardSubItem->setIcon(QIcon(ICON_DB));
            _standardSubItem->setEditable(false);
            _standardItemKey->appendRow(_standardSubItem);
            _vStandardItemKey << _standardSubItem;
        }
    }
}

void MainWidget::initView()
{
    ui->setupUi(this);

    ui->_horizontalLayout->setStretch(0,4);
    ui->_horizontalLayout->setStretch(1,2);
    ui->_horizontalLayout->setStretch(2,3);
    ui->_horizontalLayout->setStretch(3,4);
    ui->_horizontalLayout->setStretch(4,2);
    ui->_horizontalLayout->setStretch(5,1);
    ui->_horizontalLayout->setStretch(6,2);
    ui->_horizontalLayout->setStretch(7,1);
    ui->_horizontalLayout->setStretch(8,2);
    ui->_horizontalLayout->setStretch(9,1);
    ui->_horizontalLayout->setStretch(10,2);
    ui->_horizontalLayout->setStretch(11,3);
    ui->_horizontalLayout->setStretch(12,2);
    ui->_horizontalLayout->setStretch(13,1);
    ui->_horizontalLayout->setStretch(14,2);
    ui->_horizontalLayout->setStretch(15,1);

    ui->_verticalLayout->setStretchFactor(ui->_plainTextEdit,10);
    ui->_verticalLayout->setStretchFactor(ui->_horizontalLayout,1);
    ui->_verticalLayout->setStretchFactor(ui->_tabWidget,30);

    ui->_mainSplitter->setStretchFactor(0,7);
    ui->_mainSplitter->setStretchFactor(1,9);

    QFont font;
    //font.setFamily(QString::fromLocal8Bit("微软雅黑"));
    font.setPointSize(11);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    _textBrowser = new QTextBrowser();
    _textBrowser->setFont(font);
    _textBrowser->setEnabled(true);
    _dataView = new DataView();
    ui->_checkBoxSplit->setChecked(_isCmdSplit);
    ui->_lineEditsplit->setText(";");
    ui->_lineEditsplit->setFont(font);
    ui->_plainTextEdit->setFont(font);
    ui->_plainTextEdit->setPlaceholderText("enter commands here, press f8 to run or f7 to clear...");
    ui->_tabWidget->addTab(_dataView,"视图");
    ui->_tabWidget->addTab(_textBrowser,"命令");
    ui->_checkBoxFomat->setChecked(_isFormat);
    ui->_radioButtonResp->setChecked(_isResp);
    ui->_radioButtonJson->setChecked(_isJson);
    ui->_radioButtonText->setChecked(_isText);

    _itemKeyModel = new QStandardItemModel(ui->_treeView);
    ui->_treeView->setModel(_itemKeyModel);
    ui->_treeView->setHeaderHidden(true);
    ui->_treeView->setSortingEnabled(true);
    ui->_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    _keyDialog = new KeyDialog();
    _mCount = new QAction("总数统计");
    _mCount->setIcon(QIcon(ICON_COUNT));
    _mRefresh = new QAction("刷新键值");
    _mRefresh->setIcon(QIcon(ICON_FLUSHK));
    _mCreated = new QAction("新建键值");
    _mCreated->setIcon(QIcon(ICON_ADD));
    _mDelete = new QAction("删除键值");
    _mDelete->setIcon(QIcon(ICON_DEL));
    _mAlter = new QAction("修改键值");
    _mAlter->setIcon(QIcon(ICON_ALTER));
    _menu = new QMenu(this);
}

void MainWidget::showTreeRightMenu(const QPoint &pos) {
    _standardSubItem = _itemKeyModel->itemFromIndex(ui->_treeView->indexAt(pos));
    if(!_standardSubItem)
        return;
    _menu->clear();
    if(_isClusterMode) {
        _idbIndex = 0;
        if(_standardSubItem == _standardItemKey) {
            _menu->addAction(_mCount);
            _menu->addAction(_mRefresh);
            _menu->addAction(_mCreated);
        } else {
            _menu->addAction(_mAlter);
            _menu->addAction(_mDelete);
        }
    } else {
        _idbIndex = _vStandardItemKey.indexOf(_standardSubItem);
        if(_idbIndex != -1) {
            _menu->addAction(_mCount);
            _menu->addAction(_mRefresh);
            _menu->addAction(_mCreated);
        } else if(_standardItemKey == _standardSubItem) {
            _menu->addAction(_mCount);
            _menu->addAction(_mRefresh);
            _idbIndex = -1;
        } else {
            _idbIndex = _vStandardItemKey.indexOf(_standardSubItem->parent());
            _menu->addAction(_mAlter);
            _menu->addAction(_mDelete);
        }
    }
    _menu->move(ui->_treeView->cursor().pos());
    _menu->show();
}

void MainWidget::count() {
    QString str;
    if(_isClusterMode) {
        str = QString("统计键值总数为%1").arg(_standardItemKey->rowCount());
        QMessageBox::information(this, "统计", str);
    } else {
        if(_idbIndex == -1) {
            _qLongLong = 0;
            for(int i = 0; i < _idbNums; ++i) {
                _qLongLong += _vStandardItemKey[i]->rowCount();
            }
            str = QString("统计键值总数为%1").arg(_qLongLong);
            QMessageBox::information(this, "统计", str);
        } else {
            str = QString("统计键值总数为%1").arg(_vStandardItemKey[_idbIndex]->rowCount());
            QMessageBox::information(this, "统计", str);
        }
    }
}

void MainWidget::flush() {
    if(_isClusterMode && _idbIndex == 0)
        _idbIndex = -1;
    initKeyListData(_idbIndex);
}

void MainWidget::del() {
    if(QMessageBox::Yes != QMessageBox::question(this, "确认", "确定要删除此键么?"))
        return;

    if(_idbIndex >= 0) {
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            _idbIndex = 0;
            return;
        }
    }

    if(!_redisClient->del(_standardSubItem->text(), _qLongLong)) {
        QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
        _idbIndex = 0;
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, "错误", "切回db0失败");
        }
        return;
    }

    if(_isClusterMode)
        _standardItemKey->removeRow(_standardSubItem->row());
    else
        _vStandardItemKey[_idbIndex]->removeRow(_standardSubItem->row());

    _idbIndex = 0;
    if(!_redisClient->select(_idbIndex)) {
        QMessageBox::critical(this, "错误", "切回db0失败");
    }
}

void MainWidget::alter() {

    if(_idbIndex >= 0) {
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            return;
        }
    }

    QString sTtl;
    if(!_redisClient->pttl(_standardSubItem->text(), _qLongLong)) {
        QMessageBox::critical(this, "错误", "获取键超时时间失败");
        sTtl.clear();
    } else {
        sTtl = QString::number(_qLongLong);
    }

    _keyDialog->init();
    _keyDialog->setKey(_standardSubItem->text());
    _keyDialog->setTtl(sTtl);
    _keyDialog->setFlag(1);
    if(_keyDialog->exec() != QDialog::Accepted) {
        _idbIndex = 0;
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, "错误", "切回db0失败");
        }
        return;
    } else {
        if(_keyDialog->getTtl() != sTtl) {
            if(_keyDialog->getTtl().isEmpty() || _keyDialog->getTtl().toLongLong() < 0) {
                if(sTtl.toLongLong() >= 0) {
                    if(!_redisClient->persist(_standardSubItem->text())) {
                        QMessageBox::critical(this, "错误", "设置键超时时间永久失败");
                    }
                }
            } else {
                if(!_redisClient->pexpire(_standardSubItem->text(),sTtl.toLongLong())) {
                    QMessageBox::critical(this, "错误", "设置超时失败");
                }
            }
        }

        if(_keyDialog->getKey() != _standardSubItem->text()) {
            if(!_redisClient->renamex(_standardSubItem->text(),_keyDialog->getKey())) {
                QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            } else {
                _standardSubItem->setText(_keyDialog->getKey());
            }
        }
    }

    _idbIndex = 0;
    if(!_redisClient->select(_idbIndex)) {
        QMessageBox::critical(this, "错误", "切回db0失败");
    }
}

void MainWidget::add() {
    _keyDialog->init();
    _keyDialog->setFlag();
    if(_keyDialog->exec() != QDialog::Accepted)
        return;

    QString _strType = _keyDialog->getType();
    QString _strKey = _keyDialog->getKey();
    QString _strTtl = _keyDialog->getTtl();
    QList<QByteArray> textList = _keyDialog->getTextList();

    if(_idbIndex >= 0) {
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            return;
        }
    }

    if(_strType == "String") {
        QString _strValue = _keyDialog->getValue();
        if(!_redisClient->set(_strKey,_strValue)) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            _idbIndex = 0;
            if(!_redisClient->select(_idbIndex)) {
                QMessageBox::critical(this, "错误", "切回db0失败");
            }
            return;
        }
        _standardSubItem = new QStandardItem(_strKey);
        _standardSubItem->setIcon(QIcon(ICON_KEY));
        _standardSubItem->setEditable(false);
        if(_isClusterMode) {
            _standardItemKey->appendRow(_standardSubItem);
        } else {
            _vStandardItemKey[_idbIndex]->appendRow(_standardSubItem);
        }
    } else if(_strType == "Hash") {
        for(int i = 0; i < textList.size(); ++++i) {
            if(!_redisClient->hset(_strKey,textList[i],textList[i+1], _qLongLong)) {
                QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                _standardSubItem = new QStandardItem(_strKey);
                _standardSubItem->setIcon(QIcon(ICON_KEY));
                _standardSubItem->setEditable(false);
                if(_isClusterMode) {
                    _standardItemKey->appendRow(_standardSubItem);
                } else {
                    _vStandardItemKey[_idbIndex]->appendRow(_standardSubItem);
                }
            }
        }
    } else if(_strType == "Set") {
        for(int i = 0; i < textList.size(); ++i) {
            if(!_redisClient->sadd(_strKey,textList[i],_qLongLong)) {
                QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                _standardSubItem = new QStandardItem(_strKey);
                _standardSubItem->setIcon(QIcon(ICON_KEY));
                _standardSubItem->setEditable(false);
                if(_isClusterMode) {
                    _standardItemKey->appendRow(_standardSubItem);
                } else {
                    _vStandardItemKey[_idbIndex]->appendRow(_standardSubItem);
                }
            }
        }
    } else if(_strType == "ZSet") {
        for(int i = 0; i < textList.size(); ++++i) {
            if(!_redisClient->zadd(_strKey,textList[i],textList[i+1].toLongLong(), _qLongLong)) {
                QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                _standardSubItem = new QStandardItem(_strKey);
                _standardSubItem->setIcon(QIcon(ICON_KEY));
                _standardSubItem->setEditable(false);
                if(_isClusterMode) {
                    _standardItemKey->appendRow(_standardSubItem);
                } else {
                    _vStandardItemKey[_idbIndex]->appendRow(_standardSubItem);
                }
            }
        }
    } else if(_strType == "List") {
        for(int i = 0; i < textList.size(); ++i) {
            if(!_redisClient->lpush(_strKey,textList[i],_qLongLong)) {
                QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                _standardSubItem = new QStandardItem(_strKey);
                _standardSubItem->setIcon(QIcon(ICON_KEY));
                _standardSubItem->setEditable(false);
                if(_isClusterMode) {
                    _standardItemKey->appendRow(_standardSubItem);
                } else {
                    _vStandardItemKey[_idbIndex]->appendRow(_standardSubItem);
                }
            }
        }
    }

    if(!_strTtl.isEmpty() && _strTtl.toLongLong() >= 0) {
        if(!_redisClient->pexpire(_strKey,_strTtl.toLongLong())) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
        }
    }

    if(_isClusterMode) {
        _standardItemKey->sortChildren(0);
    } else {
        _vStandardItemKey[_idbIndex]->sortChildren(0);
    }

    _idbIndex = 0;
    if(!_redisClient->select(_idbIndex)) {
        QMessageBox::critical(this, "错误", "切回db0失败");
    }
}

void MainWidget::runError(const int taskid, const QString & error) {
    QMessageBox::critical(this, "错误", error);
    Q_UNUSED(taskid)
}

void MainWidget::treeClicked(const QModelIndex &index) {
    int dbIndex = 0;
    _standardSubItem = _itemKeyModel->itemFromIndex(index);
    QString key = _standardSubItem->data(Qt::DisplayRole).toString();
    if(_standardItemKey == _standardSubItem)
        return;
    if(!_isClusterMode) {
        dbIndex = _vStandardItemKey.indexOf(_standardSubItem);
        if(dbIndex != -1)
            return;
        dbIndex = _vStandardItemKey.indexOf(_standardSubItem->parent());
        if(dbIndex == -1)
            return;
    }

    QByteArray byteArray;
    _qLongLong = 0;

    _dataView->clearData();
    _dataView->setKey(key);
    _dataView->setDbIndex(dbIndex);

    if(dbIndex >= 0) {
        if(!_redisClient->select(dbIndex)) {
            QMessageBox::critical(this, "错误", _redisClient->getErrorInfo());
            return;
        }
    }

    if(!_redisClient->type(key, byteArray)) {
        QMessageBox::critical(this, "错误", "获取键类型失败!");
        return;
    }
    _dataView->setType(byteArray);

    if(byteArray == "string") {
        if(!_redisClient->get(key, byteArray)) {
            QMessageBox::critical(this, "错误", "获取键值失败!");
            return;
        }
        _dataView->setValue(QTextCodec::codecForLocale()->toUnicode(byteArray));
    }

    if(!_redisClient->pttl(key, _qLongLong)) {
        QMessageBox::critical(this, "错误", "获取键过期时间失败!");
        _qLongLong = -2;
    }
    _dataView->setTimeMs(_qLongLong);

    if(!_redisClient->encoding(key, byteArray)) {
        QMessageBox::critical(this, "错误", "获取键编码信息失败!");
        byteArray.clear();
    }
    _dataView->setEncode(byteArray);

    if(!_redisClient->refcount(key, _qLongLong)) {
        QMessageBox::critical(this, "错误", "获取键引用计数失败!");
        _qLongLong = -1;
    }
    _dataView->setRefcount(_qLongLong);

    if(!_redisClient->idletime(key, _qLongLong)) {
        QMessageBox::critical(this, "错误", "获取键空闲时间失败!");
        _qLongLong = -1;
    }
    _dataView->setIdleTimeS(_qLongLong);
    _dataView->setIndex(_redisClient->getClientIndex());
    _dataView->initValueListData();
}

void MainWidget::run()
{
    if(_isCmdSplit) {
        _strCmdSplit = ui->_lineEditsplit->text().trimmed();
        if(_strCmdSplit.isEmpty()) {
            QMessageBox::about(this, "错误", "多条命令执行,分隔符不可为空!");
            emit runEnd(true);
            return;
        }
    }

    emit runStart();
    ui->_pushButtonRun->setDisabled(true);

    _strCmd.clear();
    _strCmd = ui->_plainTextEdit->textCursor().selectedText().trimmed();
    if(_strCmd.isEmpty())
        _strCmd = ui->_plainTextEdit->toPlainText().trimmed();
    _cmdRsult.clear();
    _cmdRsult = _redisClient->command(_strCmd, _strCmdSplit);
    for(int i = 0; i < _cmdRsult.size(); ++i) {
        _byteArray.clear();
        if(_isResp) {
            if(_isFormat) {
                _iRet = 0;
                _redisClient->formatToResp(_cmdRsult[i], _byteArray , _iRet);
            } else {
                _byteArray = _cmdRsult[i].replace("\r\n","\\r\\n");
            }
            _textBrowser->append(QTextCodec::codecForLocale()->toUnicode(_byteArray));
        } else if(_isJson) {
            while(!_jsonArray.isEmpty()) {
                _jsonArray.removeAt(0);
            }
            _redisClient->formatToJson(_cmdRsult[i], _jsonArray);
            _jsonDocument.setArray(_jsonArray);
            if(_isFormat) {
                _textBrowser->append(_jsonDocument.toJson(QJsonDocument::JsonFormat::Indented));
            } else {
                _textBrowser->append(_jsonDocument.toJson(QJsonDocument::JsonFormat::Compact));
            }
        } else if(_isText) {
            _iRet = 0;
            _redisClient->formatToText(_cmdRsult[i], _byteArray , _iRet);
            _textBrowser->append(QTextCodec::codecForLocale()->toUnicode(_byteArray));
        } else {
            _textBrowser->append("配置出错，非json、resp、text格式之一");
        }
    }
    emit runEnd(true);
    ui->_pushButtonRun->setDisabled(false);
}

void MainWidget::on__pushButtonRun_clicked()
{
    run();
}

void MainWidget::on__pushButtonClear_clicked()
{
    _textBrowser->clear();
}

void MainWidget::on_CmdClear()
{
    ui->_plainTextEdit->clear();
}

void MainWidget::autoScroll() {
    _textBrowser->moveCursor(QTextCursor::End);
}

void MainWidget::on__checkBoxFomat_stateChanged(int arg1)
{
    _isFormat = arg1;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("format", _isFormat);
}

void MainWidget::on__checkBoxSplit_stateChanged(int arg1)
{
    _isCmdSplit = arg1;
    ui->_lineEditsplit->setEnabled(_isCmdSplit);
    _strCmdSplit.clear();
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("cmdSplit", _isFormat);
}

void MainWidget::on__radioButtonResp_toggled(bool checked)
{
    _isResp = checked;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("resp", _isResp);
}

void MainWidget::on__radioButtonJson_toggled(bool checked)
{
    _isJson = checked;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("json", _isJson);
}

void MainWidget::on__radioButtonText_toggled(bool checked)
{
    _isText = checked;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("text", _isText);
}
