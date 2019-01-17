#include "AppView/LoginDialog.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{

    setWindowTitle(tr("Redis连接"));
    _redisClient = nullptr;

    _labelCreate = new QLabel(tr("<a style='color:#1F90FE;' href=create><u>创建</u>"));
    _labelEdit = new QLabel(tr("<a style='color:#1F90FE;' href=edit><u>编辑</u>"));
    _labelDelte = new QLabel(tr("<a style='color:#1F90FE;' href=delete><u>删除</u>"));

    _treeWidget = new QTreeWidget(this);
    QStringList headers;
    headers.append(tr("名称"));
    headers.append(tr("地址"));
    headers.append(tr("密码"));
    _treeWidget->setHeaderLabels(headers);
    _treeWidget->setWindowFlag(Qt::FramelessWindowHint);
    _treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    _menu = new QMenu();
    _mConnect = new QAction(tr("连接"));
    _mConnect->setIcon(QIcon(ICON_LONGIN));
    _mCreated = new QAction(tr("创建"));
    _mCreated->setIcon(QIcon(ICON_ADD));
    _mEdit = new QAction(tr("编辑"));
    _mEdit->setIcon(QIcon(ICON_ALTER));
    _mDelete = new QAction(tr("删除"));
    _mDelete->setIcon(QIcon(ICON_DEL));
    _menu->addAction(_mConnect);
    _menu->addAction(_mCreated);
    _menu->addAction(_mEdit);
    _menu->addAction(_mDelete);

    _btConnect = new QPushButton(tr("连接"));
    _btCancel = new QPushButton(tr("取消"));

    _gBox = new QGridLayout(this);
    _gBox->addWidget(_labelCreate, 0, 0, 1, 1);
    _gBox->addWidget(_labelEdit, 0, 1, 1,1);
    _gBox->addWidget(_labelDelte, 0, 2, 1, 1);
    _gBox->addWidget(_treeWidget,1,0,6,10);
    _gBox->addWidget(_btConnect,7,8,1,1);
    _gBox->addWidget(_btCancel,7,9,1,1);

    initConnect();

    QRect availableGeometry = QApplication::desktop()->availableGeometry( this);
    resize(availableGeometry.width() * 4 / 7, availableGeometry.height() * 4 / 7);
    move((availableGeometry.width() - width()) / 2,
         (availableGeometry.height() - height()) / 2);

    connect(_treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(treeItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(_treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this,SLOT(showTreeRightMenu(QPoint)));
    connect(_labelCreate, SIGNAL(linkActivated(QString)), this, SLOT(onLink(QString)));
    connect(_labelEdit, SIGNAL(linkActivated(QString)), this, SLOT(onLink(QString)));
    connect(_labelDelte, SIGNAL(linkActivated(QString)), this, SLOT(onLink(QString)));
    connect(_btConnect, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(_btCancel, SIGNAL(clicked()), this, SLOT(onExit()));
    connect(_mConnect, SIGNAL(triggered(bool)),this,SLOT(onConnect()));
    connect(_mCreated, SIGNAL(triggered(bool)),this,SLOT(onCreate()));
    connect(_mEdit, SIGNAL(triggered(bool)),this,SLOT(onEdit()));
    connect(_mDelete, SIGNAL(triggered(bool)),this,SLOT(onDelete()));
}

void LoginDialog::initConnect(int index) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    _vClientInfo.clear();
    _treeWidget->clear();
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        _clientInfo._name = settings.value("name").toString().trimmed();
        _clientInfo._addr = settings.value("addr").toString().trimmed();
        _clientInfo._passwd = settings.value("passwd").toString().trimmed();
        if(_clientInfo._name.isEmpty())
            continue;
        _treeWidgetItem = new QTreeWidgetItem();
        _treeWidgetItem->setText(0,_clientInfo._name);
        _treeWidgetItem->setIcon(0,QIcon(ICON_LONGIN));
        _treeWidgetItem->setText(1,_clientInfo._addr);
        _treeWidgetItem->setText(2,_clientInfo._passwd);
        _treeWidget->addTopLevelItem(_treeWidgetItem);
        _vClientInfo << _clientInfo;
    }
    _treeWidgetItem = nullptr;
    settings.endArray();

    if(index >= 0 && _vClientInfo.size() > index) {
        _treeWidget->setCurrentItem(_treeWidget->topLevelItem(index));
    }
}

void LoginDialog::resizeEvent(QResizeEvent *event) {
    _treeWidget->setColumnWidth(0,_treeWidget->width() / 3);
    _treeWidget->setColumnWidth(1,_treeWidget->width() / 3);
    _treeWidget->setColumnWidth(2,_treeWidget->width() / 3);
    QDialog::resizeEvent(event);
}

void LoginDialog::onLink(const QString &url) {
    int selectIndex = -1;
    if(url == "create") {
        LoginSet loginSet;
        if(loginSet.exec() == QDialog::Accepted) {
            loginSet.getClientInfo(_clientInfo);
            bool isFind = false;
            for(int i = 0; i < _vClientInfo.size(); ++i) {
                if(_clientInfo._name == _vClientInfo[i]._name ) {
                    _vClientInfo[i]._addr = _clientInfo._addr;
                    _vClientInfo[i]._passwd = _clientInfo._passwd;
                    selectIndex = i;
                    isFind = true;
                    break;
                }
            }
            if(!isFind) {
                _vClientInfo << _clientInfo;
                selectIndex = _vClientInfo.size() - 1;
            }
        } else {
            return;
        }
    } else if(url == "edit") {
        _treeWidgetItem = _treeWidget->currentItem();
        if(_treeWidgetItem) {
            _clientInfo._name = _treeWidgetItem->text(0);
            _clientInfo._addr = _treeWidgetItem->text(1);
            _clientInfo._passwd = _treeWidgetItem->text(2);
            LoginSet loginSet;
            loginSet.setClientInfo(_clientInfo);
            if(loginSet.exec() == QDialog::Accepted) {
                ClientInfo clientInfo;
                loginSet.getClientInfo(clientInfo);
                if(_clientInfo._name != clientInfo._name) {
                    _vClientInfo.removeOne(_clientInfo);
                }
                bool isFind = false;
                for(int i = 0; i < _vClientInfo.size(); ++i) {
                    if(clientInfo._name == _vClientInfo[i]._name ) {
                        _vClientInfo[i]._addr = clientInfo._addr;
                        _vClientInfo[i]._passwd = clientInfo._passwd;
                        isFind = true;
                        selectIndex = i;
                        break;
                    }
                }
                if(!isFind) {
                    _vClientInfo << clientInfo;
                    selectIndex = _vClientInfo.size() - 1;
                }
            }
        } else {
            return;
        }
    } else if(url == "delete") {
        _treeWidgetItem = _treeWidget->currentItem();
        if(_treeWidgetItem) {
            if(QMessageBox::Yes != QMessageBox::question(this, tr("确认"), tr("是否要删除此登入信息？")))
                return;
            for(int i = 0; i < _vClientInfo.size(); ++i) {
                if(_treeWidgetItem->text(0) ==
                        _vClientInfo[i]._name ) {
                    _vClientInfo.removeAt(i);
                    break;
                }
            }
            selectIndex = _vClientInfo.size() - 1;
            delete _treeWidgetItem;
            _treeWidgetItem = nullptr;
        } else {
            return;
        }
    } else {
        return;
    }
    saveSet(_vClientInfo);
    initConnect(selectIndex);
}

void LoginDialog::saveSet(QList<ClientInfo> &vClientInfo) {

    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.remove("logins");
    settings.beginWriteArray("logins");
    for(int i =0; i < vClientInfo.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", vClientInfo[i]._name);
        settings.setValue("addr", vClientInfo[i]._addr);
        settings.setValue("passwd", vClientInfo[i]._passwd);
    }
    settings.endArray();
}

RedisCluster * LoginDialog::getClient() {
    return _redisClient;
}

void LoginDialog::showTreeRightMenu(const QPoint &) {
    //_treeWidgetItem = _treeWidget->itemAt(pos);
    //_treeWidget->setCurrentItem(_treeWidgetItem);
    _menu->move(_treeWidget->cursor().pos());
    _menu->show();
}

void LoginDialog::treeItemDoubleClicked(QTreeWidgetItem* ,int) {
    onOK();
}

void LoginDialog::onConnect() {
    onOK();
}

void LoginDialog::onCreate() {
    onLink("create");
}

void LoginDialog::onEdit() {
    onLink("edit");
}

void LoginDialog::onDelete() {
    onLink("delete");
}

void LoginDialog::onOK() {
    _btConnect->setEnabled(false);
    _treeWidgetItem = _treeWidget->currentItem();
    if(_treeWidgetItem) {
        if(!_redisClient)
            _redisClient = new RedisCluster();

        if(_redisClient->openCluster(_treeWidgetItem->text(1),
                                     _treeWidgetItem->text(2))) {
            _redisClient->setConnectName(_treeWidgetItem->text(0));
            accept();
        } else {
            QMessageBox::critical(this, tr("连接错误"), _redisClient->getErrorInfo());
        }
    }
    _btConnect->setEnabled(true);
}

void LoginDialog::onExit() {
    reject();
}


