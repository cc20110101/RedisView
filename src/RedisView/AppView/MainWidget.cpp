#include "AppView/MainWidget.h"

MainWidget::MainWidget(RedisCluster *redisClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    initSet(redisClient);
    initView();
    initKeyView();
    initSlot();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::reOpenClient() {
    _idbIndex = 0;
    vClients.clear();
    vMasterClients.clear();
    vClients = _redisClient->getClients(false);
    vMasterClients = _redisClient->getClients(true);
    for(int i = 0; i < vMasterClients.size(); ++i) {
        vMasterClients[i]._client = nullptr;
    }

    ui->_ipComboBox->clear();
    _isClusterMode = _redisClient->getClusterMode();
    _isReplicationMode = _redisClient->getReplicationMode();
    if(_isClusterMode) {
        _idbNums = 1;
        QString clientInfo;
        ui->_ipComboBox->addItem("Cluster mode");
        for(int j = 0; j < vClients.size(); ++j) {
            clientInfo = QString("%1:%2:%3")
                    .arg(vClients[j]._host)
                    .arg(vClients[j]._port)
                    .arg(vClients[j]._master ? "Master" : "Slave");
            ui->_ipComboBox->addItem(clientInfo);
        }
    } else if(_isReplicationMode) {
        if(!_redisClient->getDbNum(_idbNums)) {
            _idbNums = 1;
        }
        QString clientInfo;
        ui->_ipComboBox->addItem("Replication mode");
        for(int j = 0; j < vClients.size(); ++j) {
            clientInfo = QString("%1:%2:%3")
                    .arg(vClients[j]._host)
                    .arg(vClients[j]._port)
                    .arg(vClients[j]._master ? "Master" : "Slave");
            ui->_ipComboBox->addItem(clientInfo);
        }
    } else {
        if(!_redisClient->getDbNum(_idbNums)) {
            _idbNums = 1;
        }
        ui->_ipComboBox->addItem("Singleton mode");
    }

    initKeyView();
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

    _redisSendClient = nullptr;
    _redisRecvClient = nullptr;
    _redisClient = redisClient;
    _strConnectName = _redisClient->getConnectName();
    _isClusterMode = _redisClient->getClusterMode();
    _isReplicationMode =  _redisClient->getReplicationMode();
    if(_isClusterMode) {
        _idbNums = 1;
    } else {
        if(!_redisClient->getDbNum(_idbNums)) {
            _idbNums = 1;
        }
    }
    vClients = _redisClient->getClients(false);
    vMasterClients = _redisClient->getClients(true);
    for(int i = 0; i < vMasterClients.size(); ++i) {
        vMasterClients[i]._client = nullptr;
    }

    // 线程池至少会存在一个线程
    _threadPool = QThreadPool::globalInstance(); //全局线程池
    _threadPool->setMaxThreadCount(10); //最大线程数
    _threadPool->setExpiryTimeout(10000); //10s

    _tabIndex = -1;
    _idbIndex = 0;
    _iScanKeySeq = 0;
    _iScanValueSeq = 0;
    _displayViewTab = true;
    _displayCmdTab = false;
    _displayMsgTab = false;
    _tabPage.clear();
    _vTaskId.clear();
    _cmdRsult.clear();
    _vTreeItemKey.clear();
}

void MainWidget::initSlot() {
    connect(_closeView, &QAction::triggered, this, &MainWidget::closeView);
    connect(_closeCmd, &QAction::triggered, this, &MainWidget::closeCmd);
    connect(_closeMsg, &QAction::triggered, this, &MainWidget::closeMsg);
    connect(_mKeySort, &QAction::triggered, this, &MainWidget::keySort);
    connect(_mCount, &QAction::triggered, this, &MainWidget::count);
    connect(_mRefresh, &QAction::triggered, this, &MainWidget::flush);
    connect(_mCreated, &QAction::triggered, this, &MainWidget::add);
    connect(_mDelete, &QAction::triggered, this, &MainWidget::del);
    connect(_mAlter, &QAction::triggered, this, &MainWidget::alter);
    connect(ui->_treeView, &QTreeView::clicked,this,&MainWidget::treeClicked);
    connect(_tabBar, &QTabWidget::customContextMenuRequested, this, &MainWidget::showTabRightMenu);
    connect(ui->_treeView, &QTreeView::customContextMenuRequested, this, &MainWidget::showTreeRightMenu);
    connect(ui->_textBrowser, &QTextBrowser::textChanged, this, &MainWidget::autoScroll);
    connect(_dataView, &DataView::getData, this, &MainWidget::initValueListData);
    connect(_dataView, &DataView::commitData, this, &MainWidget::commitValue);
}

void MainWidget::initKeyListData(int dbIndex)
{
    if(_vTaskId.indexOf(THREAD_DEL_KEY_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台删键任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    if(_vTaskId.indexOf(THREAD_SCAN_VALUE_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台扫描键值任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    _SCAN_KEY_LOCK.lockForWrite();
    runWait(true);
    ++_iScanKeySeq;
    if(dbIndex == -1) {
        _vTreeItemKey.clear();
        _itemKeyModel->clear();
        _treeItemKey = new KeyDbTreeItem(_strConnectName, _itemKeyModel->getRootItem());
        _treeItemKey->setIconId(1);
        _itemKeyModel->insertRow(_treeItemKey);
    } else {
        _itemKeyModel->removeChild(_vTreeItemKey[dbIndex]);
    }

    for(int i = 0; i < vMasterClients.size(); ++i) {
        if(_isClusterMode) {
            _taskMsg = new TaskMsg();
            _taskMsg->_taskid = THREAD_SCAN_KEY_TASK;
            _vTaskId.push_back(THREAD_SCAN_KEY_TASK);
            _taskMsg->_sequence = _iScanKeySeq;
            _taskMsg->_host = vMasterClients[i]._host;
            _taskMsg->_port = vMasterClients[i]._port;
            _taskMsg->_passwd = vMasterClients[i]._passwd;
            _taskMsg->_keyPattern = ui->_refreshEdit->text();
            _taskMsg->_dbIndex = 0;
            _workThread = new WorkThread(_taskMsg);

            connect(_workThread, &WorkThread::sendData, this, &MainWidget::recvData);
            connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
            connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

            _threadPool->start(_workThread);
            _taskMsg = nullptr;
            _workThread = nullptr;
        } else {
            if(_isReplicationMode) {
                if(!vMasterClients[i]._master)
                    continue;
            }
            for(int j = 0; j < _idbNums; ++j) {
                if(dbIndex != -1) {
                    if(dbIndex != j)
                        continue;
                }

                if(dbIndex == -1) {
                    _subTreeItem = new KeyDbTreeItem(QString("db%1").arg(j),_treeItemKey);
                    _subTreeItem->setIconId(2);
                    _itemKeyModel->insertRow(_subTreeItem);
                    _vTreeItemKey << _subTreeItem;
                }

                _taskMsg = new TaskMsg();
                _taskMsg->_taskid = THREAD_SCAN_KEY_TASK;
                _vTaskId.push_back(THREAD_SCAN_KEY_TASK);
                _taskMsg->_sequence = _iScanKeySeq;
                _taskMsg->_host = vMasterClients[i]._host;
                _taskMsg->_port = vMasterClients[i]._port;
                _taskMsg->_passwd = vMasterClients[i]._passwd;
                _taskMsg->_keyPattern = ui->_refreshEdit->text();
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

void MainWidget::initValueListData(const InitValueMsg &initValueMsg) {

    if(_vTaskId.indexOf(THREAD_DEL_KEY_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台删键任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    if(_vTaskId.indexOf(THREAD_SCAN_KEY_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台扫描键任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    _SCAN_VALUE_LOCK.lockForWrite();

    ++_iScanValueSeq;
    runWait(true);
    _taskMsg = new TaskMsg();
    _taskMsg->_taskid = THREAD_SCAN_VALUE_TASK;
    _vTaskId.push_back(THREAD_SCAN_VALUE_TASK);
    _taskMsg->_sequence = _iScanValueSeq;
    _taskMsg->_host = vMasterClients[initValueMsg._clientIndex]._host;
    _taskMsg->_port = vMasterClients[initValueMsg._clientIndex]._port;
    _taskMsg->_passwd = vMasterClients[initValueMsg._clientIndex]._passwd;
    _taskMsg->_clientIndex = initValueMsg._clientIndex;
    _taskMsg->_dbIndex = initValueMsg._dbindex;
    _taskMsg->_key = initValueMsg._key;
    _taskMsg->_type = initValueMsg._type;
    _taskMsg->_keyPattern = initValueMsg._valuePattern;
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
    runWait(true);
    _haveError = false;
    _taskMsg = new TaskMsg();
    _taskMsg->_taskid = THREAD_COMMIT_VALUE_TASK;
    _vTaskId.push_back(THREAD_COMMIT_VALUE_TASK);
    _taskMsg->_host = vMasterClients[cmd[0]._clientIndex]._host;
    _taskMsg->_port = vMasterClients[cmd[0]._clientIndex]._port;
    _taskMsg->_passwd = vMasterClients[cmd[0]._clientIndex]._passwd;
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
            for(int i = 0; i < taskMsg._respResult._arrayValue[1]._arrayValue.size(); ++i) {
                strKey = QTextCodec::codecForLocale()->toUnicode(taskMsg._respResult._arrayValue[1]._arrayValue[i]._stringValue);
                if(_isClusterMode) {
                    _subTreeItem = new KeyTreeItem(strKey,_treeItemKey);
                } else {
                    _subTreeItem = new KeyTreeItem(strKey,_vTreeItemKey[taskMsg._dbIndex]);
                }
                _itemKeyModel->insertRow(_subTreeItem);
            }
        }
        _SCAN_KEY_LOCK.unlock();
    } else if(taskMsg._taskid == THREAD_SCAN_VALUE_TASK) {
        _SCAN_VALUE_LOCK.lockForRead();
        if(_iScanValueSeq == taskMsg._sequence) {
            _dataView->appendValue(taskMsg, taskMsg._type);
        }
        _SCAN_VALUE_LOCK.unlock();
    }
}

void MainWidget::finishWork(const int taskid) {
    _vTaskId.removeOne(taskid);
    if(taskid == THREAD_SCAN_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            runWait(false);
        }
    } else if(taskid == THREAD_COMMIT_VALUE_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            runWait(false);
            if(_haveError) {
                _dataView->initValueListData();
            }
        }
    } else if(taskid == THREAD_DEL_KEY_TASK) {
        if(_vTaskId.indexOf(taskid) == -1) {
            runWait(false);
            if(_haveError)
                flush();
        }
    } else if(taskid == THREAD_SCAN_VALUE_TASK) {
        _dataView->setRecvEnd(true);
        runWait(false);
    }
}

void MainWidget::initKeyView() {
    _vTreeItemKey.clear();
    _itemKeyModel->clear();
    _treeItemKey = new KeyDbTreeItem(_strConnectName, _itemKeyModel->getRootItem());
    _treeItemKey->setIconId(1);
    _itemKeyModel->insertRow(_treeItemKey);

    if(!_isClusterMode) {
        for(int i = 0; i < _idbNums; ++i) {
            _subTreeItem = new KeyDbTreeItem(QString("db%1").arg(i),_treeItemKey);
            _subTreeItem->setIconId(2);
            _itemKeyModel->insertRow(_subTreeItem);
            _vTreeItemKey << _subTreeItem;
        }
    }
}

void MainWidget::initView()
{
    ui->setupUi(this);

    ui->_horizontalLayout->setStretch(0,1);
    ui->_horizontalLayout->setStretch(1,2);
    ui->_horizontalLayout->setStretch(2,2); // space
    ui->_horizontalLayout->setStretch(3,2);
    ui->_horizontalLayout->setStretch(4,1);
    ui->_horizontalLayout->setStretch(5,2);
    ui->_horizontalLayout->setStretch(6,1);
    ui->_horizontalLayout->setStretch(7,2);
    ui->_horizontalLayout->setStretch(8,1);
    ui->_horizontalLayout->setStretch(9,2);
    ui->_horizontalLayout->setStretch(10,2); // space
    ui->_horizontalLayout->setStretch(11,6);
    ui->_horizontalLayout->setStretch(12,1);
    ui->_horizontalLayout->setStretch(13,2);
    ui->_horizontalLayout->setStretch(14,1);
    ui->_horizontalLayout->setStretch(15,2);

    ui->_verticalLayout->setStretchFactor(ui->_plainTextEdit,10);
    ui->_verticalLayout->setStretchFactor(ui->_horizontalLayout,1);

    ui->_commandSplitter->setStretchFactor(0,11);
    ui->_commandSplitter->setStretchFactor(1,30);

    ui->_mainSplitter->setStretchFactor(0,1);
    ui->_mainSplitter->setStretchFactor(1,1);

    ui->_msgSplitter->setStretchFactor(0,12);
    ui->_msgSplitter->setStretchFactor(1,5);
    ui->_subHorizontalLayout->setStretch(0,1);
    ui->_subHorizontalLayout->setStretch(1,3);
    ui->_subHorizontalLayout->setStretch(2,1);
    ui->_subHorizontalLayout->setStretch(3,2);
    ui->_subHorizontalLayout->setStretch(4,1);
    ui->_subHorizontalLayout->setStretch(5,3);
    ui->_subHorizontalLayout->setStretch(6,1);
    ui->_subHorizontalLayout->setStretch(7,2);
    ui->_subHorizontalLayout->setStretch(8,1);
    ui->_subHorizontalLayout->setStretch(9,1);
    ui->_subHorizontalLayout->setStretch(10,1);
    ui->_subHorizontalLayout->setStretch(11,1);
    ui->_channelHorizontalLayout->setStretch(0,1);
    ui->_channelHorizontalLayout->setStretch(1,3);
    ui->_channelHorizontalLayout->setStretch(2,10);

    QFont font;
    //font.setFamily(QString::fromLocal8Bit("微软雅黑"));
    font.setPointSize(11);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    ui->_textBrowser->setFont(font);
    ui->_subTextBrowser->setFont(font);
    ui->_publishPlainTextEdit->setFont(font);
    ui->_textBrowser->setEnabled(true);
    _dataView = new DataView();
    ui->_checkBoxSplit->setChecked(_isCmdSplit);
    ui->_lineEditsplit->setText(";");
    ui->_lineEditsplit->setFont(font);
    ui->_plainTextEdit->setFont(font);
    ui->_plainTextEdit->setPlaceholderText("enter commands here, press f8 to run or f7 to clear...");
    ui->_viewLayout->addWidget(_dataView);
    ui->_tabWidget->setCurrentIndex(1);
    ui->_checkBoxFomat->setChecked(_isFormat);
    ui->_radioButtonResp->setChecked(_isResp);
    ui->_radioButtonJson->setChecked(_isJson);
    ui->_radioButtonText->setChecked(_isText);

    _itemKeyModel = new KeyTreeModel(ui->_treeView);
    ui->_treeView->setModel(_itemKeyModel);
    ui->_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->_treeView->setHeaderHidden(true);
    ui->_treeView->setSortingEnabled(true);
    //ui->_treeView->setFrameStyle(QFrame::NoFrame);
    ui->_treeView->setRootIsDecorated(true);
    ui->_treeView->setAlternatingRowColors(false);
    ui->_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->_refreshEdit->setPlaceholderText(tr("键初始化模式"));
    ui->_refreshEdit->setText(getKeyPattern());
    ui->_refreshEdit->setEnabled(true);

    for(int i = 0; i < ui->_tabWidget->count(); ++i)
        _tabPage.push_back(ui->_tabWidget->widget(i));

    for(int j = _tabPage.size() - 1; j > 0; --j)
        ui->_tabWidget->removeTab(j);

    if(_isClusterMode) {
        QString clientInfo;
        ui->_ipComboBox->addItem("Cluster mode");
        for(int k = 0; k < vClients.size(); ++k) {
            clientInfo = QString("%1:%2:%3")
                    .arg(vClients[k]._host)
                    .arg(vClients[k]._port)
                    .arg(vClients[k]._master ? "Master" : "Slave");
            ui->_ipComboBox->addItem(clientInfo);
        }
    } else if(_isReplicationMode) {
        QString clientInfo;
        ui->_ipComboBox->addItem("Replication mode");
        for(int j = 0; j < vClients.size(); ++j) {
            clientInfo = QString("%1:%2:%3")
                    .arg(vClients[j]._host)
                    .arg(vClients[j]._port)
                    .arg(vClients[j]._master ? "Master" : "Slave");
            ui->_ipComboBox->addItem(clientInfo);
        }
    } else {
        ui->_ipComboBox->addItem("Singleton mode");
    }

    _tabBar = ui->_tabWidget->tabBar();
    _tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    _keyDialog = new KeyDialog();
    _mKeySort = new QAction(tr("键值排序"));
    _mKeySort->setIcon(QIcon(ICON_SORT));
    _mCount = new QAction(tr("总数统计"));
    _mCount->setIcon(QIcon(ICON_COUNT));
    _mRefresh = new QAction(tr("刷新键值"));
    _mRefresh->setIcon(QIcon(ICON_FLUSHK));
    _mCreated = new QAction(tr("新建键值"));
    _mCreated->setIcon(QIcon(ICON_ADD));
    _mDelete = new QAction(tr("删除键值"));
    _mDelete->setIcon(QIcon(ICON_DEL));
    _mAlter = new QAction(tr("修改键值"));
    _mAlter->setIcon(QIcon(ICON_ALTER));
    _closeView = new QAction(tr("关闭视图"));
    _closeView->setIcon(QIcon(ICON_TAB));
    _closeCmd = new QAction(tr("关闭命令"));
    _closeCmd->setIcon(QIcon(ICON_TAB));
    _closeMsg = new QAction(tr("关闭订阅"));
    _closeMsg->setIcon(QIcon(ICON_TAB));
    _treeMenu = new QMenu(this);
    _tabMenu = new QMenu(this);
    _movie = new QMovie(GIF_WAIT);
    _waitLabel = new QLabel(this);
    _waitLabel->setFixedSize(50,50);
    _waitLabel->setContentsMargins(0,0,0,0);
    QRect rect = geometry();
    _waitLabel->move(rect.x() + rect.width()/2 - _waitLabel->width() /2,
                     rect.y() + rect.height()/4 - _waitLabel->height());
    _waitLabel->setMovie(_movie);
    runWait();
}

void MainWidget::showTabRightMenu(const QPoint &pos) {
    _tabIndex = _tabBar->tabAt(pos);
    if(_tabIndex == -1)
        return;

    _tabMenu->clear();
    QWidget *tabPage = ui->_tabWidget->widget(_tabIndex);
    int index = _tabPage.indexOf(tabPage);
    if(index == 0) {
        _tabMenu->addAction(_closeView);
    } else if(index == 1) {
        _tabMenu->addAction(_closeCmd);
    } else if(index == 2){
        _tabMenu->addAction(_closeMsg);
    } else {
        return;
    }
    _tabMenu->move(_tabBar->cursor().pos());
    _tabMenu->show();
}

void MainWidget::closeView() {
    _displayViewTab = false;
    ui->_tabWidget->removeTab(_tabIndex);
}

void MainWidget::closeCmd() {
    _displayCmdTab = false;
    ui->_tabWidget->removeTab(_tabIndex);
}

void MainWidget::closeMsg() {
    _displayMsgTab = false;
    ui->_tabWidget->removeTab(_tabIndex);
}

void MainWidget::openView() {
    if(!_displayViewTab) {
        _displayViewTab = true;
        ui->_tabWidget->addTab(_tabPage[0],tr("视图"));
        ui->_tabWidget->setCurrentIndex(ui->_tabWidget->count() - 1);
    } else {
        _tabIndex = ui->_tabWidget->indexOf(_tabPage[0]);
        ui->_tabWidget->setCurrentIndex(_tabIndex);
    }
}

void MainWidget::openMsg() {
    if(!_displayMsgTab) {
        _displayMsgTab = true;
        ui->_tabWidget->addTab(_tabPage[2],tr("消息"));
        ui->_tabWidget->setCurrentIndex(ui->_tabWidget->count() - 1);
    } else {
        _tabIndex = ui->_tabWidget->indexOf(_tabPage[2]);
        ui->_tabWidget->setCurrentIndex(_tabIndex);
    }
}

void MainWidget::showTreeRightMenu(const QPoint &pos) {
    QModelIndex proxIndex = ui->_treeView->indexAt(pos);
    _subTreeItem = _itemKeyModel->itemFromIndex(proxIndex);
    if(!_subTreeItem)
        return;
    _treeMenu->clear();
    if(_isClusterMode) {
        _idbIndex = 0;
        if(_subTreeItem == _treeItemKey) {
            _treeMenu->addAction(_mCount);
            _treeMenu->addAction(_mRefresh);
            _treeMenu->addAction(_mKeySort);
            _treeMenu->addAction(_mCreated);
        } else {
            _treeMenu->addAction(_mAlter);
            _treeMenu->addAction(_mDelete);
        }
    } else {
        _idbIndex = _vTreeItemKey.indexOf(_subTreeItem);
        if(_idbIndex != -1) {
            _treeMenu->addAction(_mCount);
            _treeMenu->addAction(_mRefresh);
            _treeMenu->addAction(_mKeySort);
            _treeMenu->addAction(_mCreated);
        } else if(_treeItemKey == _subTreeItem) {
            _treeMenu->addAction(_mCount);
            _treeMenu->addAction(_mRefresh);
            _treeMenu->addAction(_mKeySort);
            _idbIndex = -1;
        } else {
            _idbIndex = _vTreeItemKey.indexOf(_subTreeItem->parent());
            _treeMenu->addAction(_mAlter);
            _treeMenu->addAction(_mDelete);
        }
    }
    _treeMenu->move(ui->_treeView->cursor().pos());
    _treeMenu->show();
}

void MainWidget::keySort() {
    runWait(true);
    if(_isClusterMode) {
        _itemKeyModel->sortItem(_treeItemKey, _vSortMap.value(0,Qt::AscendingOrder));
        if(_vSortMap.value(0,Qt::AscendingOrder) == Qt::AscendingOrder) {
            _vSortMap[0] = Qt::DescendingOrder;
        } else {
            _vSortMap[0] = Qt::AscendingOrder;
        }
    } else {
        if(_idbIndex == -1) {
            for(int i = 0; i < _vTreeItemKey.size(); ++i) {
                _itemKeyModel->sortItem(_vTreeItemKey[i], _vSortMap.value(i,Qt::AscendingOrder));
                if(_vSortMap.value(i,Qt::AscendingOrder) == Qt::AscendingOrder) {
                    _vSortMap[i] = Qt::DescendingOrder;
                } else {
                    _vSortMap[i] = Qt::AscendingOrder;
                }
            }
        } else {
            _itemKeyModel->sortItem(_vTreeItemKey[_idbIndex], _vSortMap.value(_idbIndex,Qt::AscendingOrder));
            if(_vSortMap.value(_idbIndex,Qt::AscendingOrder) == Qt::AscendingOrder) {
                _vSortMap[_idbIndex] = Qt::DescendingOrder;
            } else {
                _vSortMap[_idbIndex] = Qt::AscendingOrder;
            }
        }
    }
    runWait(false);
}

void MainWidget::count() {
    QString str;
    QModelIndex sindex;
    if(_isClusterMode) {
        sindex = _itemKeyModel->indexFromItem(_treeItemKey);
        str = QString(tr("统计键值总数为")) + QString::number(_itemKeyModel->rowCount(sindex));
        QMessageBox::information(this, tr("统计"), str);
    } else {
        if(_idbIndex == -1) {
            _qLongLong = 0;
            for(int i = 0; i < _idbNums; ++i) {
                sindex = _itemKeyModel->indexFromItem(_vTreeItemKey[i]);
                _qLongLong += _itemKeyModel->rowCount(sindex);
            }
            str = tr("统计键值总数为") + QString::number(_qLongLong);
            QMessageBox::information(this, tr("统计"), str);
        } else {
            sindex = _itemKeyModel->indexFromItem(_vTreeItemKey[_idbIndex]);
            str = tr("统计键值总数为") + QString::number(_itemKeyModel->rowCount(sindex));
            QMessageBox::information(this, tr("统计"), str);
        }
    }
}

void MainWidget::flush() {
    if(_isClusterMode && _idbIndex == 0)
        _idbIndex = -1;
    initKeyListData(_idbIndex);
    setKeyPattern(ui->_refreshEdit->text());
}

void MainWidget::del() {

    if(_vTaskId.size() > 0) {
        QMessageBox::critical(this, tr("提示"), tr("后台任务未结束，请稍后进行删键操作!"));
        return;
    }

    if(QMessageBox::Yes != QMessageBox::question(this, tr("确认"), tr("确定要删除选中键么?")))
        return;

    // 通过_view去获取被选中的部分的数据model
    QItemSelectionModel *selectModel = ui->_treeView->selectionModel();
    // 通过选中的数据结构，获取这些格子的ModelIndex
    QModelIndexList selectList = selectModel->selectedIndexes();
    QVector<KeyTreeItem *> delRow;

    if(selectList.size() <= 0) {
        QMessageBox::critical(this, tr("提示"), tr("请先选择要删除的键！"));
        return;
    }
    // 遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    for(int i = 0; i < selectList.size(); ++i) {
        QModelIndex index = selectList.at(i);
        _subTreeItem = _itemKeyModel->itemFromIndex(index);

        if(_isClusterMode) {
            if(_subTreeItem == _treeItemKey)
                continue;
        } else {
            if(_subTreeItem->parent() == _treeItemKey ||
                    _subTreeItem == _treeItemKey)
                continue;
        }

        if(delRow.contains(_subTreeItem))
            continue;
        delRow << _subTreeItem;
    }

    if(delRow.size() <= 0) {
        QMessageBox::critical(this, tr("提示"), tr("没有可删除的键！"));
        return;
    }

    if(vMasterClients.size() <= 0) {
        QMessageBox::critical(this, tr("提示"), tr("客户端连接信息异常!"));
        return;
    }

    _haveError = false;
    _vCmdMsg.clear();
    runWait(true);
    while(delRow.size() > 0) {
        _subTreeItem = delRow.at(0);
        delRow.removeAll(_subTreeItem);
        if(_isClusterMode) {
            _cmdMsg.init();
            _cmdMsg._dbIndex = -1;
            _cmdMsg._clientIndex = -1;
            _cmdMsg._operate = 2;
            _cmdMsg._key = _subTreeItem->text();
            _itemKeyModel->removeChild(_subTreeItem->parent(), _subTreeItem->childNumber());
        } else {
            _cmdMsg.init();
            _cmdMsg._dbIndex = _vTreeItemKey.indexOf(_subTreeItem->parent());
            _cmdMsg._clientIndex = -1;
            _cmdMsg._operate = 2;
            _cmdMsg._key = _subTreeItem->text();
            _itemKeyModel->removeChild(_vTreeItemKey[_cmdMsg._dbIndex],_subTreeItem->childNumber());
        }

        if(_cmdMsg._key == _dataView->getKey())
            _dataView->clearData();

        _vCmdMsg << _cmdMsg;

        if(_vCmdMsg.size() > 5000) {
            _taskMsg = new TaskMsg();
            _taskMsg->_taskid = THREAD_DEL_KEY_TASK;
            _vTaskId.push_back(THREAD_DEL_KEY_TASK);
            _taskMsg->_host = vMasterClients[0]._host;
            _taskMsg->_port = vMasterClients[0]._port;
            _taskMsg->_passwd = vMasterClients[0]._passwd;
            _taskMsg->_clientIndex = _isClusterMode;
            _workThread = new WorkThread(_vCmdMsg,_taskMsg);

            connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
            connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

            _threadPool->start(_workThread);
            _taskMsg = nullptr;
            _workThread = nullptr;
            _vCmdMsg.clear();
        }
    }

    if(_vCmdMsg.size() > 0) {
        _taskMsg = new TaskMsg();
        _taskMsg->_taskid = THREAD_DEL_KEY_TASK;
        _vTaskId.push_back(THREAD_DEL_KEY_TASK);
        _taskMsg->_host = vMasterClients[0]._host;
        _taskMsg->_port = vMasterClients[0]._port;
        _taskMsg->_passwd = vMasterClients[0]._passwd;
        _taskMsg->_clientIndex = _isClusterMode;
        _workThread = new WorkThread(_vCmdMsg,_taskMsg);

        connect(_workThread, &WorkThread::finishWork, this, &MainWidget::finishWork);
        connect(_workThread, &WorkThread::runError, this, &MainWidget::runError);

        _threadPool->start(_workThread);
        _taskMsg = nullptr;
        _workThread = nullptr;
        _vCmdMsg.clear();
    }
}

void MainWidget::alter() {

    if(_idbIndex >= 0) {
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
            return;
        }
    }

    QString sTtl;
    if(!_redisClient->pttl(_subTreeItem->text(), _qLongLong)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键超时时间失败"));
        sTtl.clear();
    } else {
        sTtl = QString::number(_qLongLong);
    }

    _keyDialog->init();
    _keyDialog->setKey(_subTreeItem->text());
    _keyDialog->setTtl(sTtl);
    _keyDialog->setFlag(1);
    if(_keyDialog->exec() != QDialog::Accepted) {
        _idbIndex = 0;
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, tr("错误"), tr("切回db0失败"));
        }
        return;
    } else {
        if(_keyDialog->getTtl() != sTtl) {
            if(_keyDialog->getTtl().isEmpty() || _keyDialog->getTtl().toLongLong() < 0) {
                if(sTtl.toLongLong() >= 0) {
                    if(!_redisClient->persist(_subTreeItem->text())) {
                        QMessageBox::critical(this, tr("错误"), tr("设置键超时时间永久失败"));
                    }
                }
            } else {
                if(!_redisClient->pexpire(_subTreeItem->text(),_keyDialog->getTtl().toLongLong())) {
                    QMessageBox::critical(this, tr("错误"), tr("设置超时失败"));
                }
            }
        }

        if(_keyDialog->getKey() != _subTreeItem->text()) {
            if(!_redisClient->renamex(_subTreeItem->text(),_keyDialog->getKey())) {
                QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
            } else {
                _itemKeyModel->setText(_subTreeItem, _keyDialog->getKey());
            }
        }
    }

    _idbIndex = 0;
    if(!_redisClient->select(_idbIndex)) {
        QMessageBox::critical(this, tr("错误"), tr("切回db0失败"));
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
    QList<QString> textList = _keyDialog->getTextList();

    if(_idbIndex >= 0) {
        if(!_redisClient->select(_idbIndex)) {
            QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
            return;
        }
    }

    if(_strType == "String") {
        QString _strValue = _keyDialog->getValue();
        if(!_redisClient->set(_strKey,_strValue)) {
            QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
            _idbIndex = 0;
            if(!_redisClient->select(_idbIndex)) {
                QMessageBox::critical(this, tr("错误"), tr("切回db0失败"));
            }
            return;
        }
        if(_isClusterMode) {
            _subTreeItem = new KeyTreeItem(_strKey,_treeItemKey);
        } else {
            _subTreeItem = new KeyTreeItem(_strKey,_vTreeItemKey[_idbIndex]);
        }
        _itemKeyModel->insertRow(_subTreeItem);
    } else if(_strType == "Hash") {
        for(int i = 0; i < textList.size(); ++++i) {
            if(!_redisClient->hset(_strKey,textList[i],textList[i+1], _qLongLong)) {
                QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                if(_isClusterMode) {
                    _subTreeItem = new KeyTreeItem(_strKey,_subTreeItem);
                } else {
                    _subTreeItem = new KeyTreeItem(_strKey,_vTreeItemKey[_idbIndex]);
                }
                _itemKeyModel->insertRow(_subTreeItem);
            }
        }
    } else if(_strType == "Set") {
        for(int i = 0; i < textList.size(); ++i) {
            if(!_redisClient->sadd(_strKey,textList[i],_qLongLong)) {
                QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                if(_isClusterMode) {
                    _subTreeItem = new KeyTreeItem(_strKey,_treeItemKey);
                } else {
                    _subTreeItem = new KeyTreeItem(_strKey,_vTreeItemKey[_idbIndex]);
                }
                _itemKeyModel->insertRow(_subTreeItem);
            }
        }
    } else if(_strType == "ZSet") {
        for(int i = 0; i < textList.size(); ++++i) {
            if(!_redisClient->zadd(_strKey,textList[i],textList[i+1].toLongLong(), _qLongLong)) {
                QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                if(_isClusterMode) {
                    _subTreeItem = new KeyTreeItem(_strKey,_treeItemKey);
                } else {
                    _subTreeItem = new KeyTreeItem(_strKey,_vTreeItemKey[_idbIndex]);
                }
                _itemKeyModel->insertRow(_subTreeItem);
            }
        }
    } else if(_strType == "List") {
        for(int i = 0; i < textList.size(); ++i) {
            if(!_redisClient->lpush(_strKey,textList[i],_qLongLong)) {
                QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
                continue;
            }
            if(!i) {
                if(_isClusterMode) {
                    _subTreeItem = new KeyTreeItem(_strKey,_treeItemKey);
                } else {
                    _subTreeItem = new KeyTreeItem(_strKey,_vTreeItemKey[_idbIndex]);
                }
                _itemKeyModel->insertRow(_subTreeItem);
            }
        }
    }

    if(!_strTtl.isEmpty() && _strTtl.toLongLong() >= 0) {
        if(!_redisClient->pexpire(_strKey,_strTtl.toLongLong())) {
            QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
        }
    }

    if(_isClusterMode) {
        _itemKeyModel->sortItem(_treeItemKey);
    } else {
        _itemKeyModel->sortItem(_vTreeItemKey[_idbIndex]);
    }

    _idbIndex = 0;
    if(!_redisClient->select(_idbIndex)) {
        QMessageBox::critical(this, tr("错误"), tr("切回db0失败"));
    }
}

void MainWidget::runError(const int taskid, const QString & error) {
    QMessageBox::critical(this, tr("错误"), error);
    if(taskid == THREAD_COMMIT_VALUE_TASK ||
            taskid == THREAD_DEL_KEY_TASK)
        _haveError = true;
}

void MainWidget::treeClicked(const QModelIndex &index) {

    if(!_displayViewTab)
        return;

    if(_vTaskId.indexOf(THREAD_DEL_KEY_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台删键任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    if(_vTaskId.indexOf(THREAD_SCAN_KEY_TASK) != -1) {
        QMessageBox::critical(this, tr("提示"), tr("后台扫描键任务未结束，请稍后进行键刷新操作!"));
        return;
    }

    int dbIndex = 0;
    _subTreeItem = _itemKeyModel->itemFromIndex(index);
    QString key = _subTreeItem->text();
    if(_treeItemKey == _subTreeItem)
        return;
    if(!_isClusterMode) {
        dbIndex = _vTreeItemKey.indexOf(_subTreeItem);
        if(dbIndex != -1)
            return;
        dbIndex = _vTreeItemKey.indexOf(_subTreeItem->parent());
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
            QMessageBox::critical(this, tr("错误"), _redisClient->getErrorInfo());
            return;
        }
    }

    if(!_redisClient->type(key, byteArray)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键类型失败!"));
        return;
    }
    _dataView->setType(byteArray);

    if(byteArray == "string") {
        if(!_redisClient->get(key, byteArray)) {
            QMessageBox::critical(this, tr("错误"), tr("获取键值失败!"));
            return;
        }
        _dataView->setValue(QTextCodec::codecForLocale()->toUnicode(byteArray));
    }

    if(!_redisClient->pttl(key, _qLongLong)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键过期时间失败!"));
        _qLongLong = -2;
    }
    _dataView->setTimeMs(_qLongLong);

    if(!_redisClient->encoding(key, byteArray)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键编码信息失败!"));
        byteArray.clear();
    }
    _dataView->setEncode(byteArray);

    if(!_redisClient->refcount(key, _qLongLong)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键引用计数失败!"));
        _qLongLong = -1;
    }
    _dataView->setRefcount(_qLongLong);

    if(!_redisClient->idletime(key, _qLongLong)) {
        QMessageBox::critical(this, tr("错误"), tr("获取键空闲时间失败!"));
        _qLongLong = -1;
    }
    _dataView->setIdleTimeS(_qLongLong);
    _dataView->setIndex(_redisClient->getClientIndex());
    _dataView->initValueListData();
}

void MainWidget::run()
{
    if(!_displayCmdTab) {
        _displayCmdTab = true;
        ui->_tabWidget->addTab(_tabPage[1],tr("命令"));
        ui->_tabWidget->setCurrentIndex(ui->_tabWidget->count() - 1);
        emit runEnd(true);
        return;
    } else {
        _tabIndex = ui->_tabWidget->indexOf(_tabPage[1]);
        ui->_tabWidget->setCurrentIndex(_tabIndex);
    }

    if(_isCmdSplit) {
        _strCmdSplit = ui->_lineEditsplit->text().trimmed();
        if(_strCmdSplit.isEmpty()) {
            QMessageBox::about(this, tr("错误"), tr("多条命令执行,分隔符不可为空!"));
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
    _cmdRsult = _redisClient->command(_strCmd, _strCmdSplit, ui->_ipComboBox->currentIndex() - 1);
    for(int i = 0; i < _cmdRsult.size(); ++i) {
        _byteArray.clear();
        if(_isResp) {
            if(_isFormat) {
                _iRet = 0;
                _redisClient->formatToResp(_cmdRsult[i], _byteArray , _iRet);
            } else {
                _byteArray = _cmdRsult[i].replace("\r\n","\\r\\n");
            }
            ui->_textBrowser->append(QTextCodec::codecForLocale()->toUnicode(_byteArray));
        } else if(_isJson) {
            while(!_jsonArray.isEmpty()) {
                _jsonArray.removeAt(0);
            }
            _redisClient->formatToJson(_cmdRsult[i], _jsonArray);
            _jsonDocument.setArray(_jsonArray);
            if(_isFormat) {
                ui->_textBrowser->append(_jsonDocument.toJson(QJsonDocument::JsonFormat::Indented));
            } else {
                ui->_textBrowser->append(_jsonDocument.toJson(QJsonDocument::JsonFormat::Compact));
            }
        } else if(_isText) {
            _iRet = 0;
            _redisClient->formatToText(_cmdRsult[i], _byteArray , _iRet);
            ui->_textBrowser->append(QTextCodec::codecForLocale()->toUnicode(_byteArray));
        } else {
            ui->_textBrowser->append(tr("配置出错，非json、resp、text格式之一"));
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
    ui->_textBrowser->clear();
}

void MainWidget::on_CmdClear()
{
    ui->_plainTextEdit->clear();
}

void MainWidget::autoScroll() {
    ui->_textBrowser->moveCursor(QTextCursor::End);
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

void MainWidget::on__refreshButton_clicked()
{
    if(_isReplicationMode)
        _idbIndex = -1;
    flush();
}

void MainWidget::on__publishButton_clicked()
{
    QString chanel = ui->_channelLineEdit->text().trimmed();
    if(chanel.isEmpty()) {
        QMessageBox::about(this, tr("错误"), tr("发送频道不可为空!"));
        return;
    }

    QString msg = ui->_publishPlainTextEdit->toPlainText().trimmed();
    if(msg.isEmpty()) {
        QMessageBox::about(this, tr("错误"), tr("发送消息不可为空!"));
        return;
    }

    if(!_redisSendClient) {
        if(vMasterClients.size() <= 0) {
            QMessageBox::about(this, tr("错误"), tr("客户端连接信息为空!"));
            return;
        } else {
            _redisSendClient = new RedisClient();
            _redisSendClient->open(vMasterClients[vMasterClients.size() - 1]._host,
                    vMasterClients[vMasterClients.size() - 1]._port);
            if(!vMasterClients[vMasterClients.size() - 1]._passwd.isEmpty()) {
                if(!_redisSendClient->auth(vMasterClients[vMasterClients.size() - 1]._passwd)) {
                    QMessageBox::about(this, tr("错误"), _redisSendClient->getErrorInfo());
                    return;
                }
            }
            if(!_redisSendClient->isOpen()) {
                QMessageBox::about(this, tr("错误"), tr("客户端连接失败!"));
                return;
            }
        }
    }

    qlonglong qllRet = 0;
    if(!_redisSendClient->publish(chanel,msg,qllRet)) {
        QMessageBox::about(this, tr("错误"), _redisSendClient->getErrorInfo());
        return;
    }

    QString sendMsg = QString("Send to %1 [%2]:%3").arg(chanel)
            .arg(qllRet).arg(msg);
    ui->_subTextBrowser->append(sendMsg);
    ui->_publishPlainTextEdit->clear();
}

void MainWidget::on__subscribeButton_clicked()
{
    QString chanel = ui->_subPatternLineEdit->text().trimmed();
    if(chanel.isEmpty()) {
        QMessageBox::about(this, tr("错误"), tr("订阅频道不可为空!"));
        return;
    }

    if(!_redisRecvClient) {
        if(vMasterClients.size() <= 0) {
            QMessageBox::about(this, tr("错误"), tr("客户端连接信息为空!"));
            return;
        } else {
            _redisRecvClient = new RedisClient();
            _redisRecvClient->open(vMasterClients[vMasterClients.size() - 1]._host,
                    vMasterClients[vMasterClients.size() - 1]._port);
            if(!vMasterClients[vMasterClients.size() - 1]._passwd.isEmpty()) {
                if(!_redisRecvClient->auth(vMasterClients[vMasterClients.size() - 1]._passwd)) {
                    QMessageBox::about(this, tr("错误"), _redisRecvClient->getErrorInfo());
                    return;
                }
            }
            if(!_redisRecvClient->isOpen()) {
                QMessageBox::about(this, tr("错误"), tr("客户端连接失败!"));
                return;
            }
        }
        _redisRecvClient->setSubModel(true);
        connect(_redisRecvClient, &RedisClient::sigMessage, this, &MainWidget::recvMessage);
    }
    _redisRecvClient->psubscribe(chanel);
}

void MainWidget::on__unSubcribeButton_clicked()
{
    if(_redisRecvClient) {
        _redisRecvClient->punsubscribe(ui->_unsubPatternLineEdit->text().trimmed());
    } else {
        QMessageBox::about(this, tr("错误"), tr("未执行订阅,无需取消订阅!"));
        return;
    }
}

void MainWidget::recvMessage(const RespType &msg) {
    QString rescMsg;
    if(msg._arrayValue.size() == 4 && msg._arrayValue[0]._stringValue == "pmessage") {
        rescMsg = QString("Receive %1 channel of %2 pattern msg:%3")
                .arg(QTextCodec::codecForLocale()->toUnicode(msg._arrayValue[2]._stringValue))
                .arg(QTextCodec::codecForLocale()->toUnicode(msg._arrayValue[1]._stringValue))
                .arg(QTextCodec::codecForLocale()->toUnicode(msg._arrayValue[3]._stringValue));
    } else if(msg._arrayValue.size() == 3 && msg._arrayValue[0]._stringValue == "punsubscribe") {
        rescMsg = QString("Punsubscribe %1 channel pattern [%2] success...")
                .arg(QTextCodec::codecForLocale()->toUnicode(msg._arrayValue[1]._stringValue))
                .arg(msg._arrayValue[2]._integerValue);
    } else if(msg._arrayValue.size() == 3 && msg._arrayValue[0]._stringValue == "psubscribe") {
        rescMsg = QString("Psubscribe %1 [%2] success...")
                .arg(QTextCodec::codecForLocale()->toUnicode(msg._arrayValue[1]._stringValue))
                .arg(msg._arrayValue[2]._integerValue);
    }

    if(!rescMsg.isEmpty())
        ui->_subTextBrowser->append(rescMsg);
}

void MainWidget::on__msgClearButton_clicked()
{
    ui->_subTextBrowser->clear();
}

void MainWidget::on__msgInfoButton_clicked()
{
    if(!_redisSendClient) {
        if(vMasterClients.size() <= 0) {
            QMessageBox::about(this, tr("错误"), tr("客户端连接信息为空!"));
            return;
        } else {
            _redisSendClient = new RedisClient();
            _redisSendClient->open(vMasterClients[vMasterClients.size() - 1]._host,
                    vMasterClients[vMasterClients.size() - 1]._port);
            if(!vMasterClients[vMasterClients.size() - 1]._passwd.isEmpty()) {
                if(!_redisSendClient->auth(vMasterClients[vMasterClients.size() - 1]._passwd)) {
                    QMessageBox::about(this, tr("错误"), _redisSendClient->getErrorInfo());
                    return;
                }
            }
            if(!_redisSendClient->isOpen()) {
                QMessageBox::about(this, tr("错误"), tr("客户端连接失败!"));
                return;
            }
        }
    }

    PubsubDialog pubsubDialog(_redisSendClient);
    pubsubDialog.exec();
}

int MainWidget::getTaskSize() {
    return _vTaskId.size();
}

void MainWidget::resizeEvent(QResizeEvent *) {
    QRect rect = geometry();
    _waitLabel->move(rect.x() + rect.width()/2 - _waitLabel->width() /2,
                     rect.y() + rect.height()/4 - _waitLabel->height());
}

void MainWidget::runWait(bool isRun) {
    if(isRun) {
        setEnabled(false);
        _waitLabel->raise();
        _waitLabel->setVisible(true);
        _movie->start();
    } else {
        setEnabled(true);
        _movie->stop();
        _waitLabel->lower();
        _waitLabel->setVisible(false);
    }
}

QString MainWidget::getKeyPattern() {
    _keyPattern.clear();
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        if(_redisClient->getConnectName() == clientInfo._name) {
            _keyPattern = settings.value("keypattern","").toString();
            break;
        }
    }
    settings.endArray();
    return _keyPattern;
}

void MainWidget::setKeyPattern(QString keyPattern) {
    if(_keyPattern == keyPattern)
        return;
    QList<ClientInfoDialog> vClientInfo;
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        clientInfo._addr = settings.value("addr").toString().trimmed();
        clientInfo._passwd = settings.value("passwd").toString().trimmed();
        clientInfo._encode = settings.value("encode","GB18030").toString().trimmed();
        clientInfo._keyPattern = settings.value("keypattern","").toString();
        clientInfo._valuePattern = settings.value("valuepattern","").toString();
        vClientInfo << clientInfo;
    }
    settings.endArray();
    settings.remove("logins");
    settings.beginWriteArray("logins");
    for(int j =0; j < vClientInfo.size(); ++j) {
        settings.setArrayIndex(j);
        settings.setValue("name", vClientInfo[j]._name);
        settings.setValue("addr", vClientInfo[j]._addr);
        settings.setValue("passwd", vClientInfo[j]._passwd);
        settings.setValue("encode", vClientInfo[j]._encode);
        if(_redisClient->getConnectName() == vClientInfo[j]._name)
            settings.setValue("keypattern", keyPattern);
        else
            settings.setValue("keypattern", vClientInfo[j]._keyPattern);
        settings.setValue("valuepattern", vClientInfo[j]._valuePattern);
    }
    settings.endArray();
}
