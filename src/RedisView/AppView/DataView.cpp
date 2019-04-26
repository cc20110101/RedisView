#include "AppView/DataView.h"
#include "ui_dataview.h"

DataView::DataView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataView)
{
    ui->setupUi(this);

    _keyLineEdit = new QLineEdit();
    _timeLineEdit = new QLineEdit();
    _typeLineEdit = new QLineEdit();
    _typeLineEdit->setEnabled(false);
    _refcountLineEdit = new QLineEdit();
    _refcountLineEdit->setEnabled(false);
    _encodLineEdit = new QLineEdit();
    _encodLineEdit->setEnabled(false);
    _idletimeLineEdit = new QLineEdit();
    _idletimeLineEdit->setEnabled(false);

    _headHLayout = new QHBoxLayout();
    _headHLayout->addWidget(new QLabel("Key:"),2);
    _headHLayout->addWidget(_keyLineEdit,8);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("Ttl(ms):"),2);
    _headHLayout->addWidget(_timeLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("Type:"),2);
    _headHLayout->addWidget(_typeLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("RefCount:"),2);
    _headHLayout->addWidget(_refcountLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("Encoding:"),2);
    _headHLayout->addWidget(_encodLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("IdleTime(s):"),2);
    _headHLayout->addWidget(_idletimeLineEdit,4);
    _headHLayout->addStretch(6);

    _tableView = new QTableView(this);
    _itemTableModel = new ValueTableModel(this);
    _tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    _tableView->setAlternatingRowColors(false);
    //_tableView->setItemDelegate(new ValueTableDelegate());
    _tableView->setModel(_itemTableModel);
    _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _leftVlayout = new QVBoxLayout();
    _leftVlayout->addWidget(_tableView);

    _bodyAddButton = new QPushButton("Insert");
    _bodyDelButton = new QPushButton("Delete");
    _bodyRefreshButton = new QPushButton("Refresh");
    _bodyOkButton = new QPushButton("Submit");
    _bodyAddHeadButton = new QPushButton("InsertHead");
    _bodyAddTailButton = new QPushButton("InsertTail");
    _bodyDelHeadButton = new QPushButton("DeleteHead");
    _bodyDelTailButton = new QPushButton("DeleteTail");
    _bodyAddHeadButton->setVisible(false);
    _bodyAddTailButton->setVisible(false);
    _bodyDelHeadButton->setVisible(false);
    _bodyDelTailButton->setVisible(false);
    _rightVlayout = new QVBoxLayout();
    _rightVlayout->addWidget(_bodyRefreshButton);
    _rightVlayout->addWidget(_bodyAddHeadButton);
    _rightVlayout->addWidget(_bodyAddTailButton);
    _rightVlayout->addWidget(_bodyDelHeadButton);
    _rightVlayout->addWidget(_bodyDelTailButton);
    _rightVlayout->addWidget(_bodyAddButton);
    _rightVlayout->addWidget(_bodyDelButton);
    _rightVlayout->addWidget(_bodyOkButton);

    _bodyHLayout = new QHBoxLayout();
    _bodyHLayout->addLayout(_leftVlayout,64);
    _bodyHLayout->addStretch(1);
    _bodyHLayout->addLayout(_rightVlayout,4);

    _valueScanPattern = new QLineEdit();
    _valueScanPattern->setPlaceholderText("value scan pattern");
    _valueScanPattern->setText(getValuePattern());

    _searchHLayout = new QHBoxLayout();
    _searchHLayout->addWidget(new QLabel("Init Pattern:"),1);
    _searchHLayout->addWidget(_valueScanPattern,4);
    _searchHLayout->addStretch(16);

    _mainHLayout = new QVBoxLayout();
    _mainHLayout->addLayout(_headHLayout);
    _mainHLayout->addLayout(_bodyHLayout);
    _mainHLayout->addLayout(_searchHLayout);

    ui->_widget->setLayout(_mainHLayout);

    _time = 0;
    _key.clear();
    _type.clear();
    _vCmdMsg.clear();
    _recvEnd = false;
    _inputDialog = new InputDialog(this);
    _countAc = new QAction(QIcon(ICON_COUNT), tr("条数统计"));
    _resetAc = new QAction(QIcon(ICON_FLUSH), tr("刷新重置"));
    _addAc = new QAction(QIcon(ICON_ADD), tr("插入数据"));
    _delAc = new QAction(QIcon(ICON_DEL), tr("删除数据"));
    _commitAc = new QAction(QIcon(ICON_COMMIT), tr("提交操作"));
    _inheadAc = new QAction(QIcon(ICON_IHEAD), tr("链头插入"));
    _intailAc = new QAction(QIcon(ICON_ITAIL), tr("链尾插入"));
    _delheadAc = new QAction(QIcon(ICON_DHEAD), tr("链头删除"));
    _deltailAc = new QAction(QIcon(ICON_DTAIL), tr("链尾删除"));
    _inheadAc->setVisible(false);
    _intailAc->setVisible(false);
    _delheadAc->setVisible(false);
    _deltailAc->setVisible(false);
    _tableView->addAction(_resetAc);
    _tableView->addAction(_countAc);
    _tableView->addAction(_inheadAc);
    _tableView->addAction(_intailAc);
    _tableView->addAction(_delheadAc);
    _tableView->addAction(_deltailAc);
    _tableView->addAction(_addAc);
    _tableView->addAction(_delAc);
    _tableView->addAction(_commitAc);

    connect(_countAc, SIGNAL(triggered()), this, SLOT(count()));
    connect(_resetAc, SIGNAL(triggered()), this, SLOT(flush()));
    connect(_addAc, SIGNAL(triggered()), this, SLOT(add()));
    connect(_delAc, SIGNAL(triggered()), this, SLOT(del()));
    connect(_commitAc, SIGNAL(triggered()), this, SLOT(commit()));
    connect(_inheadAc, SIGNAL(triggered()), this, SLOT(addHead()));
    connect(_intailAc, SIGNAL(triggered()), this, SLOT(addTail()));
    connect(_delheadAc, SIGNAL(triggered()), this, SLOT(delHead()));
    connect(_deltailAc, SIGNAL(triggered()), this, SLOT(delTail()));
    connect(_itemTableModel, &ValueTableModel::itemChanged, this, &DataView::valueChanged);
    connect(_bodyOkButton, &QPushButton::clicked, this, &DataView::commit);
    connect(_bodyDelButton, &QPushButton::clicked, this, &DataView::del);
    connect(_bodyRefreshButton, &QPushButton::clicked, this, &DataView::flush);
    connect(_bodyAddButton, &QPushButton::clicked, this, &DataView::add);
    connect(_bodyAddHeadButton, &QPushButton::clicked, this, &DataView::addHead);
    connect(_bodyAddTailButton, &QPushButton::clicked, this, &DataView::addTail);
    connect(_bodyDelHeadButton, &QPushButton::clicked, this, &DataView::delHead);
    connect(_bodyDelTailButton, &QPushButton::clicked, this, &DataView::delTail);
}

DataView::~DataView()
{
    delete ui;
}

void DataView::flush() {
    initValueListData();
}

void DataView::count() {
    QString str = QString(tr("统计值总数为")) + QString::number(_itemTableModel->rowCount());
    QMessageBox::information(this, tr("统计"), str);
}

void DataView::commit() {

    if(_type != "string" &&
            _type != "hash" &&
            _type != "set" &&
            _type != "zset" &&
            _type != "list")
        return;

    QString str = _timeLineEdit->text().trimmed();
    if(_time != str.toLongLong()) {
        _cmdMsg.init();
        _cmdMsg._dbIndex = _dbIndex;
        _cmdMsg._clientIndex = _clientIndex;
        _cmdMsg._key = _key;
        _cmdMsg._type = _type;
        _cmdMsg._value = str;
        _cmdMsg._valueIndex = _time;
        _cmdMsg._operate = 4;
        _vCmdMsg << _cmdMsg;
    }

    str = _keyLineEdit->text().trimmed();
    if(str.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("新KEY不可为空！"));
        return;
    }

    if(_key != str) {
        _cmdMsg.init();
        _cmdMsg._dbIndex = _dbIndex;
        _cmdMsg._clientIndex = _clientIndex;
        _cmdMsg._key = _key;
        _cmdMsg._type = _type;
        _cmdMsg._value = str;
        _cmdMsg._operate = 5;
        _vCmdMsg << _cmdMsg;
    }

    if(_vCmdMsg.size() <= 0)
        return;
    emit commitData(_vCmdMsg);
    _vCmdMsg.clear();
}

void DataView::addHead() {
    if(_type == "list") {
        _inputDialog->clear();
        _inputDialog->setTip("member1 member2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;
        QList<QString> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _tableItemValue = new ValueTableItem(valueList[i]);
            _itemTableModel->insertRow(0, _tableItemValue);

            _cmdMsg.init();
            _cmdMsg._dbIndex = _dbIndex;
            _cmdMsg._clientIndex = _clientIndex;
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._valueIndex = -1;
            _cmdMsg._value = valueList[i];
            _cmdMsg._operate = 1;
            _vCmdMsg << _cmdMsg;
        }
    }
}

void DataView::addTail() {
    if(_type == "list") {
        _inputDialog->clear();
        _inputDialog->setTip("member1 member2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;

        _listIndex = _itemTableModel->rowCount();
        QList<QString> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _tableItemValue = new ValueTableItem(QString(valueList[i]));
            _itemTableModel->insertRow(_listIndex,_tableItemValue);

            _cmdMsg.init();
            _cmdMsg._dbIndex = _dbIndex;
            _cmdMsg._clientIndex = _clientIndex;
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._valueIndex = 1;
            _cmdMsg._value = valueList[i];
            _cmdMsg._operate = 1;
            _vCmdMsg << _cmdMsg;
        }
    }
}

void DataView::delHead() {
    if(_itemTableModel->rowCount() <= 0) {
        QMessageBox::critical(this, tr("错误"), tr("无数据行需要删除！"));
        return;
    }

    _cmdMsg.init();
    _cmdMsg._dbIndex = _dbIndex;
    _cmdMsg._clientIndex = _clientIndex;
    if(_type == "list") {
        _cmdMsg._key = _key;
        _cmdMsg._type = _type;
        _cmdMsg._valueIndex = -1;
        _cmdMsg._operate = 2;
    }
    _vCmdMsg << _cmdMsg;
    _itemTableModel->removeRow(0);
}

void DataView::delTail() {
    _listIndex = _itemTableModel->rowCount();
    if(_listIndex <= 0) {
        QMessageBox::critical(this, tr("错误"), tr("无数据行需要删除！"));
        return;
    }

    _cmdMsg.init();
    _cmdMsg._dbIndex = _dbIndex;
    _cmdMsg._clientIndex = _clientIndex;
    if(_type == "list") {
        _cmdMsg._key = _key;
        _cmdMsg._type = _type;
        _cmdMsg._valueIndex = 1;
        _cmdMsg._operate = 2;
    }
    _vCmdMsg << _cmdMsg;
    _itemTableModel->removeRow(_listIndex - 1);
}

void DataView::add() {
    if(_type == "hash") {
        _inputDialog->clear();
        _inputDialog->setTip("filed1 value1 filed2 value2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;
        QList<QString> valueList = _inputDialog->getTextList();
        QVector<QVariant> data;
        for(int i =0; i < valueList.size(); ++++i) {
            data.clear();
            data.push_back(QVariant(valueList[i]));
            data.push_back(QVariant(valueList[i+1]));
            _tableItemValue = new ValueTableItem(data);
            _itemTableModel->insertRow(0,_tableItemValue);

            _cmdMsg.init();
            _cmdMsg._dbIndex = _dbIndex;
            _cmdMsg._clientIndex = _clientIndex;
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._filed = valueList[i];
            _cmdMsg._value = valueList[i+1];
            _cmdMsg._operate = 1;
            if(_vCmdMsg.contains(_cmdMsg)) {
                _vCmdMsg.removeAll(_cmdMsg);
            }
            _vCmdMsg << _cmdMsg;
        }
    } else if(_type == "set") {
        _inputDialog->clear();
        _inputDialog->setTip("member1 member2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;
        QList<QString> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _tableItemValue = new ValueTableItem(valueList[i]);
            _itemTableModel->insertRow(0,_tableItemValue);

            _cmdMsg.init();
            _cmdMsg._dbIndex = _dbIndex;
            _cmdMsg._clientIndex = _clientIndex;
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._value = valueList[i];
            _cmdMsg._operate = 1;
            _vCmdMsg << _cmdMsg;
        }
    } else if(_type == "zset") {
        _inputDialog->clear();
        _inputDialog->setTip("member1 score1 member2 score2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;

        double dscore;
        QVector<QVariant> data;
        QList<QString> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++++i) {
            data.clear();
            dscore = valueList[i+1].toDouble();
            data.push_back(QVariant(valueList[i]));
            data.push_back(QVariant(dscore));
            _tableItemValue = new ValueTableItem(data);
            _itemTableModel->insertRow(0,_tableItemValue);

            _cmdMsg.init();
            _cmdMsg._dbIndex = _dbIndex;
            _cmdMsg._clientIndex = _clientIndex;
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._value = valueList[i];
            _cmdMsg._score = dscore;
            _cmdMsg._operate = 1;
            _vCmdMsg << _cmdMsg;
        }
    }
}

void DataView::del() {

    if(_type != "string" &&
            _type != "hash" &&
            _type != "set" &&
            _type != "zset" &&
            _type != "list")
        return;

    if(_itemTableModel->rowCount() <= 0) {
        QMessageBox::critical(this, tr("错误"), tr("无数据行需要删除！"));
        return;
    }

    // 通过_view去获取被选中的部分的数据model
    QItemSelectionModel *selectModel = _tableView->selectionModel();
    // 通过选中的数据结构，获取这些格子的ModelIndex
    QModelIndexList selectList = selectModel->selectedIndexes();
    QVector<ValueTableItem *> delRow;

    if(selectList.size() <= 0) {
        QMessageBox::critical(this, tr("提示"), tr("请先选择要删除的行！"));
        return;
    }
    // 遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    for(int i = 0; i < selectList.size(); ++i) {
        QModelIndex index = selectList.at(i);
        _tableSubItem = _itemTableModel->item(index.row());
        if(delRow.contains(_tableSubItem))
            continue;
        delRow << _tableSubItem;
    }

    while(delRow.size() > 0) {
        _tableSubItem = delRow.at(0);

        _cmdMsg.init();
        _cmdMsg._dbIndex = _dbIndex;
        _cmdMsg._clientIndex = _clientIndex;
        if(_type == "hash") {
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._filed = _tableSubItem->text();
            _cmdMsg._operate = 2;
            if(_vCmdMsg.contains(_cmdMsg)) {
                _vCmdMsg.removeAll(_cmdMsg);
            }
        } else if(_type == "set" || _type == "zset") {
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._value = _tableSubItem->text();
            _cmdMsg._operate = 2;
        }
        _vCmdMsg << _cmdMsg;

        delRow.removeAll(_tableSubItem);
        _itemTableModel->removeItem(_tableSubItem);
    }
}

void DataView::valueChanged(ValueTableItem *item, int column) {
    _cmdMsg.init();
    _cmdMsg._dbIndex = _dbIndex;
    _cmdMsg._clientIndex = _clientIndex;
    _cmdMsg._operate = 3;
    _cmdMsg._type = _type;
    _cmdMsg._key = _key;
    if(_type == "string") {
        _cmdMsg._value = item->text(column);
        if(_vCmdMsg.contains(_cmdMsg)) {
            _vCmdMsg.removeAll(_cmdMsg);
        }
    } else if(_type == "hash") {
        if(item)
            _cmdMsg._filed = item->text();
        else
            return;
        _cmdMsg._value = item->text(column);
        if(_vCmdMsg.contains(_cmdMsg)) {
            _vCmdMsg.removeAll(_cmdMsg);
        }
    } else if(_type == "list") {
        _cmdMsg._valueIndex = _itemTableModel->childNumber(item);
        _cmdMsg._value = item->text(column);
    } else {
        return;
    }

    _vCmdMsg << _cmdMsg;
}

void DataView::clearData() {
    _keyLineEdit->clear();
    _timeLineEdit->clear();
    _typeLineEdit->clear();
    _refcountLineEdit->clear();
    _encodLineEdit->clear();
    _idletimeLineEdit->clear();
    _itemTableModel->clear();
    _cmdMsg.init();
    _vCmdMsg.clear();
    _listIndex = 0;
    _recvEnd = false;
}

void DataView::initValueListData() {
    _key = _keyLineEdit->text().trimmed();
    if(_key.isEmpty())
        return;

    _listIndex = 0;
    _vCmdMsg.clear();
    _itemTableModel->clear();
    _initValueMsg.init();
    setValuePattern(_valueScanPattern->text());

    _initValueMsg._key = _key;
    _initValueMsg._type = _type;
    _initValueMsg._dbindex = _dbIndex;
    _initValueMsg._clientIndex = _clientIndex;
    _initValueMsg._valuePattern = _valuePattern;

    _tableView->setSortingEnabled(true);
    if(_type == "string") {
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) | Qt::ItemIsEditable);
        emit getData(_initValueMsg);
    } else if(_type == "hash") {
        _itemTableModel->setColumnCount(2);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "FIELD");
        _itemTableModel->setHeaderData(1,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        _itemTableModel->setFlags(1,_itemTableModel->flags(1) | Qt::ItemIsEditable);
        emit getData(_initValueMsg);
    } else if(_type == "list") {
        _listIndex = 0;
        _tableView->setSortingEnabled(false);
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) | Qt::ItemIsEditable);
        emit getData(_initValueMsg);
    } else if(_type == "set") {
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        emit getData(_initValueMsg);
    } else if(_type == "zset") {
        _itemTableModel->setColumnCount(2);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        _itemTableModel->setHeaderData(1,Qt::Horizontal, "SCORE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        _itemTableModel->setFlags(1,_itemTableModel->flags(1) & (~Qt::ItemIsEditable));
        emit getData(_initValueMsg);
    }
}

void DataView::appendValue(const TaskMsg & taskMsg, const QByteArray flag) {
    if(flag == "hash") {
        for(int i = 0; i < taskMsg._respResult._arrayValue[1]._arrayValue.size(); ++++i) {
            vRowData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._respResult._arrayValue[1]._arrayValue[i]._stringValue);
            vRowData.push_back(QVariant(_value));
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._respResult._arrayValue[1]._arrayValue[i+1]._stringValue);
            vRowData.push_back(QVariant(_value));
            _tableItemValue = new ValueTableItem(vRowData);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "zset") {
        for(int i = 0; i < taskMsg._respResult._arrayValue[1]._arrayValue.size(); ++++i) {
            vRowData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._respResult._arrayValue[1]._arrayValue[i]._stringValue);
            vRowData.push_back(QVariant(_value));
            vRowData.push_back(QVariant(taskMsg._respResult._arrayValue[1]._arrayValue[i+1]._stringValue.toDouble()));
            _tableItemValue = new ValueTableItem(vRowData);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "set") {
        for(int i = 0; i < taskMsg._respResult._arrayValue[1]._arrayValue.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._respResult._arrayValue[1]._arrayValue[i]._stringValue);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "string") {
        for(int i = 0; i < taskMsg._list.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._list[i]);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
            break;
        }
    } else if(flag == "list") {
        for(int i = 0; i < taskMsg._list.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(taskMsg._list[i]);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
        }
    }
}

void DataView::setDbIndex(const int &index) {
    _dbIndex = index;
}

void DataView::setIndex(const int &index) {
    _clientIndex = index;
    if(_clientIndex < 0)
        _clientIndex = 0;
}

void DataView::setKey(const QString &key) {
    _key = key;
    _keyLineEdit->setText(key);
}

QString DataView::getKey() {
    return _key;
}

void DataView::setValue(const QString &value) {
    _value = value;
}

void DataView::setType(const QByteArray &type) {
    _type = type;
    _bodyAddHeadButton->setVisible(false);
    _bodyAddTailButton->setVisible(false);
    _bodyDelHeadButton->setVisible(false);
    _bodyDelTailButton->setVisible(false);
    _inheadAc->setVisible(false);
    _intailAc->setVisible(false);
    _delheadAc->setVisible(false);
    _deltailAc->setVisible(false);
    _addAc->setVisible(true);
    _delAc->setVisible(true);
    if(_type == "string") {
        _addAc->setVisible(false);
        _delAc->setVisible(false);
        _typeLineEdit->setText("String");
        _bodyAddButton->setVisible(false);
        _bodyDelButton->setVisible(false);
    } else if(_type == "hash") {
        _typeLineEdit->setText("Hash");
        _bodyAddButton->setVisible(true);
        _bodyDelButton->setVisible(true);
    } else if(_type == "list") {
        _addAc->setVisible(false);
        _delAc->setVisible(false);
        _inheadAc->setVisible(true);
        _intailAc->setVisible(true);
        _delheadAc->setVisible(true);
        _deltailAc->setVisible(true);
        _bodyAddButton->setVisible(false);
        _bodyDelButton->setVisible(false);
        _bodyAddHeadButton->setVisible(true);
        _bodyAddTailButton->setVisible(true);
        _bodyDelHeadButton->setVisible(true);
        _bodyDelTailButton->setVisible(true);
        _typeLineEdit->setText("List");
    } else if(_type == "set") {
        _bodyAddButton->setVisible(true);
        _bodyDelButton->setVisible(true);
        _typeLineEdit->setText("Set");
    } else if(_type == "zset") {
        _bodyAddButton->setVisible(true);
        _bodyDelButton->setVisible(true);
        _typeLineEdit->setText("Zset");
    } else
        _typeLineEdit->setText("None");
}

void DataView::setTimeMs(const qlonglong &timeMs) {
    _time = timeMs;
    _timeLineEdit->setText(QString::number(timeMs));
}

void DataView::setEncode(const QByteArray &encode) {
    _encodLineEdit->setText(QString(encode));
}

void DataView::setRefcount(const qlonglong &count) {
    _refcountLineEdit->setText(QString::number(count));
}

void DataView::setIdleTimeS(const qlonglong &times) {
    _idletimeLineEdit->setText(QString::number(times));
}

void DataView::setRecvEnd(bool recvEnd)
{
    _recvEnd = recvEnd;
    //_tableView->resizeRowsToContents();
    //_tableView->resizeColumnsToContents();
    //_tableView->setColumnWidth(0,_tableView->width());

}

QString DataView::getValuePattern() {
    _valuePattern.clear();
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        if(Global::gConnectName == clientInfo._name) {
            _valuePattern = settings.value("valuepattern","").toString();
            break;
        }
    }
    settings.endArray();
    return _valuePattern;
}

void DataView::setValuePattern(QString valuePattern) {
    if(_valuePattern == valuePattern)
        return;
    _valuePattern = valuePattern;
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
        settings.setValue("keypattern", vClientInfo[j]._keyPattern);
        if(Global::gConnectName == vClientInfo[j]._name)
            settings.setValue("valuepattern", valuePattern);
        else
            settings.setValue("valuepattern", vClientInfo[j]._valuePattern);
    }
    settings.endArray();
}

