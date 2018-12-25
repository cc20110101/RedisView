#include "DataView.h"
#include "ui_dataview.h"

DataView::DataView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataView)
{
    ui->setupUi(this);

    _keyLineEdit = new QLineEdit();
    //_keyLineEdit->setReadOnly(true);
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
    _headHLayout->addWidget(new QLabel("KEY:"),2);
    _headHLayout->addWidget(_keyLineEdit,8);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("TTL(ms):"),2);
    _headHLayout->addWidget(_timeLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("TYPE:"),2);
    _headHLayout->addWidget(_typeLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("REFCOUNT:"),2);
    _headHLayout->addWidget(_refcountLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("ENCODING:"),2);
    _headHLayout->addWidget(_encodLineEdit,4);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("IDLETIME(s):"),2);
    _headHLayout->addWidget(_idletimeLineEdit,4);
    _headHLayout->addStretch(6);

    _tableView = new QTableView();
    _itemValueModel = new QStandardItemModel(_tableView);
    _tableView->setModel(_itemValueModel);
    _tableView->setSortingEnabled(true);
    _tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    _leftVlayout = new QVBoxLayout();
    _leftVlayout->addWidget(_tableView);

    _bodyAddButton = new QPushButton("Insert");
    _bodyDelButton = new QPushButton("Delete");
    _bodyRefreshButton = new QPushButton("Reset");
    _bodyOkButton = new QPushButton("Commit");
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
    _bodyHLayout->addLayout(_leftVlayout);
    _bodyHLayout->addLayout(_rightVlayout);

    _mainHLayout = new QVBoxLayout();
    _mainHLayout->addLayout(_headHLayout);
    _mainHLayout->addLayout(_bodyHLayout);

    ui->_widget->setLayout(_mainHLayout);

    _time = 0;
    _key.clear();
    _type.clear();
    _vCmdMsg.clear();
    _inputDialog = new InputDialog(this);
    _resetAc = new QAction(QIcon(ICON_FLUSH), "刷新重置");
    _addAc = new QAction(QIcon(ICON_ADD), "插入数据");
    _delAc = new QAction(QIcon(ICON_DEL), "删除数据");
    _commitAc = new QAction(QIcon(ICON_COMMIT), "提交操作");
    _inheadAc = new QAction(QIcon(ICON_IHEAD), "链头插入");
    _intailAc = new QAction(QIcon(ICON_ITAIL), "链尾插入");
    _delheadAc = new QAction(QIcon(ICON_DHEAD), "链头删除");
    _deltailAc = new QAction(QIcon(ICON_DTAIL), "链尾删除");
    _inheadAc->setVisible(false);
    _intailAc->setVisible(false);
    _delheadAc->setVisible(false);
    _deltailAc->setVisible(false);
    _tableView->addAction(_resetAc);
    _tableView->addAction(_inheadAc);
    _tableView->addAction(_intailAc);
    _tableView->addAction(_delheadAc);
    _tableView->addAction(_deltailAc);
    _tableView->addAction(_addAc);
    _tableView->addAction(_delAc);
    _tableView->addAction(_commitAc);

    connect(_resetAc, SIGNAL(triggered()), this, SLOT(flush()));
    connect(_addAc, SIGNAL(triggered()), this, SLOT(add()));
    connect(_delAc, SIGNAL(triggered()), this, SLOT(del()));
    connect(_commitAc, SIGNAL(triggered()), this, SLOT(commit()));
    connect(_itemValueModel, &QStandardItemModel::itemChanged, this, &DataView::valueChanged);
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
        QMessageBox::critical(this, "错误", "新KEY不可为空！");
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
        QList<QStandardItem *> vItemData;
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            vItemData.clear();
            _standardItemValue = new QStandardItem(QString(valueList[i]));
            vItemData << _standardItemValue;
            _itemValueModel->insertRow(0, vItemData);

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

        _listIndex = _itemValueModel->rowCount();
        QList<QStandardItem *> vItemData;
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            vItemData.clear();
            _standardItemValue = new QStandardItem(QString(valueList[i]));
            vItemData << _standardItemValue;
            _itemValueModel->insertRow(_listIndex,vItemData);

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
    if(_itemValueModel->rowCount() <= 0) {
        QMessageBox::critical(this, "错误", "无数据行需要删除！");
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
    _itemValueModel->removeRow(0);
}

void DataView::delTail() {
    _listIndex = _itemValueModel->rowCount();
    if(_listIndex <= 0) {
        QMessageBox::critical(this, "错误", "无数据行需要删除！");
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
    _itemValueModel->removeRow(_listIndex - 1);
}

void DataView::add() {
    if(_type == "hash") {
        _inputDialog->clear();
        _inputDialog->setTip("filed1 value1 filed2 value2 ...");
        _inputDialog->setType(_type);
        if(_inputDialog->exec() != QDialog::Accepted)
            return;
        QList<QStandardItem *> vItemData;
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++++i) {
            vItemData.clear();
            _standardItemValue = new QStandardItem(QString(valueList[i]));
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            vItemData << _standardItemValue;
            _standardItemValue = new QStandardItem(QString(valueList[i+1]));
            vItemData << _standardItemValue;
            _itemValueModel->insertRow(0,vItemData);

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
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _standardItemValue = new QStandardItem(QString(valueList[i]));
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            _itemValueModel->insertRow(0,_standardItemValue);

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
        QList<QStandardItem *> vItemData;
        double dscore;
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++++i) {
            vItemData.clear();
            dscore = 0;
            _standardItemValue = new QStandardItem(QString(valueList[i]));
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            vItemData << _standardItemValue;
            dscore = valueList[i+1].toDouble();
            _standardItemValue = new QStandardItem();
            _standardItemValue->setData(dscore, Qt::DisplayRole);
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            vItemData << _standardItemValue;
            _itemValueModel->insertRow(0,vItemData);

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

    if(_itemValueModel->rowCount() <= 0) {
        QMessageBox::critical(this, "错误", "无数据行需要删除！");
        return;
    }

    // 通过_view去获取被选中的部分的数据model
    QItemSelectionModel *selectModel = _tableView->selectionModel();
    // 通过选中的数据结构，获取这些格子的ModelIndex
    QModelIndexList selectList =  selectModel->selectedIndexes();
    QList<QStandardItem *> delRow;

    if(selectList.size() <= 0) {
        QMessageBox::critical(this, "提示", "请先选择要删除的行！");
        return;
    }
    // 遍历这些格子，获取格子所在行，因为可能存在相同的行，所以要去重
    for(int i = 0; i < selectList.size(); ++i) {
        QModelIndex index = selectList.at(i);
        _standardSubItem = _itemValueModel->item(index.row(),0);
        if(delRow.contains(_standardSubItem))
            continue;
        delRow << _standardSubItem;
    }

    while(delRow.size() > 0) {
        _standardSubItem = delRow.at(0);

        _cmdMsg.init();
        _cmdMsg._dbIndex = _dbIndex;
        _cmdMsg._clientIndex = _clientIndex;
        if(_type == "hash") {
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._filed = _standardSubItem->text();
            _cmdMsg._operate = 2;
            if(_vCmdMsg.contains(_cmdMsg)) {
                _vCmdMsg.removeAll(_cmdMsg);
            }
        } else if(_type == "set" || _type == "zset") {
            _cmdMsg._key = _key;
            _cmdMsg._type = _type;
            _cmdMsg._value = _standardSubItem->text();
            _cmdMsg._operate = 2;
        }
        _vCmdMsg << _cmdMsg;

        delRow.removeAll(_standardSubItem);
        _itemValueModel->removeRow(_standardSubItem->row());
    }
}

void DataView::valueChanged(QStandardItem *item) {
    _cmdMsg.init();
    _cmdMsg._dbIndex = _dbIndex;
    _cmdMsg._clientIndex = _clientIndex;
    _cmdMsg._operate = 3;
    _cmdMsg._type = _type;
    _cmdMsg._key = _key;
    if(_type == "string") {
        _cmdMsg._value = item->text();
        if(_vCmdMsg.contains(_cmdMsg)) {
            _vCmdMsg.removeAll(_cmdMsg);
        }
    } else if(_type == "hash") {
        _standardSubItem = _itemValueModel->item(item->row(),0);
        if(_standardSubItem)
            _cmdMsg._filed = _standardSubItem->text();
        else
            return;
        _cmdMsg._value = item->text();
        if(_vCmdMsg.contains(_cmdMsg)) {
            _vCmdMsg.removeAll(_cmdMsg);
        }
    } else if(_type == "list") {
        _cmdMsg._valueIndex = item->row();
        _cmdMsg._value = item->text();
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
    _itemValueModel->clear();
    _cmdMsg.init();
    _vCmdMsg.clear();
    _listIndex = 0;
}

void DataView::initValueListData() {
    _key = _keyLineEdit->text().trimmed();
    if(_key.isEmpty())
        return;

    _listIndex = 0;
    _vCmdMsg.clear();
    _itemValueModel->clear();
    _tableView->setSortingEnabled(true);
    if(_type == "string") {
        _itemValueModel->setColumnCount(1);
        _itemValueModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        emit getData(_key,_type,_clientIndex,_dbIndex);
    } else if(_type == "hash") {
        _itemValueModel->setColumnCount(2);
        _itemValueModel->setHeaderData(0,Qt::Horizontal, "FIELD");
        _itemValueModel->setHeaderData(1,Qt::Horizontal, "VALUE");
        emit getData(_key,_type,_clientIndex,_dbIndex);
    } else if(_type == "list") {
        _listIndex = 0;
        _tableView->setSortingEnabled(false);
        _itemValueModel->setColumnCount(1);
        _itemValueModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        emit getData(_key,_type,_clientIndex,_dbIndex);
    } else if(_type == "set") {
        _itemValueModel->setColumnCount(1);
        _itemValueModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        emit getData(_key,_type,_clientIndex,_dbIndex);
    } else if(_type == "zset") {
        _itemValueModel->setColumnCount(2);
        _itemValueModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        _itemValueModel->setHeaderData(1,Qt::Horizontal, "SCORE");
        emit getData(_key,_type,_clientIndex,_dbIndex);
    }
}

void DataView::appendValue(const QList<QByteArray> & vList, const QByteArray flag) {
    if(flag == "hash") {
        for(int i = 0; i < vList.size(); ++++i) {
            _vItemData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _standardItemValue = new QStandardItem(_value);
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            _vItemData << _standardItemValue;
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i+1]);
            _standardItemValue = new QStandardItem(_value);
            _vItemData << _standardItemValue;
            _itemValueModel->appendRow(_vItemData);
        }
    } else if(flag == "zset") {
        for(int i = 0; i < vList.size(); ++++i) {
            _vItemData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _standardItemValue = new QStandardItem(_value);
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            _vItemData << _standardItemValue;
            _standardItemValue = new QStandardItem();
            _standardItemValue->setData(vList[i+1].toDouble(), Qt::DisplayRole);
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            _vItemData << _standardItemValue;
            _itemValueModel->appendRow(_vItemData);
        }
    } else if(flag == "set") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _standardItemValue = new QStandardItem(_value);
            _standardItemValue->setFlags(_standardItemValue->flags() & (~Qt::ItemIsEditable));
            _itemValueModel->appendRow(_standardItemValue);
        }
    } else if(flag == "string") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _standardItemValue = new QStandardItem(_value);
            _itemValueModel->appendRow(_standardItemValue);
            break;
        }
    } else if(flag == "list") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _standardItemValue = new QStandardItem(_value);
            _itemValueModel->appendRow(_standardItemValue);
        }
    }
    _tableView->resizeColumnsToContents();
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
