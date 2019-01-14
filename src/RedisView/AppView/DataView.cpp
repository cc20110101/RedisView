#include "AppView/DataView.h"
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
    _headHLayout->addWidget(new QLabel("Key:"),2);
    _headHLayout->addWidget(_keyLineEdit,8);
    _headHLayout->addStretch(1);
    _headHLayout->addWidget(new QLabel("LiveTime(ms):"),2);
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
    _valuePattern = PubLib::getConfig("initValuePattern");
    _valueScanPattern->setText(_valuePattern);

    _syntaxComb = new QComboBox();
    _syntaxComb->addItem("Regular expression", QRegExp::RegExp);
    _syntaxComb->addItem("Wildcard", QRegExp::Wildcard);
    _syntaxComb->addItem("Fixed string", QRegExp::FixedString);
    _columnComb = new QComboBox();
    _searchLineEdit = new QLineEdit();
    _searchButton = new QPushButton("Search");
    _filterCaseSensitivityCheckBox = new QCheckBox("Case sensitive");
    _filterCaseSensitivityCheckBox->setChecked(false);
    _searchHLayout = new QHBoxLayout();
    _searchHLayout->addWidget(_valueScanPattern,4);
    _searchHLayout->addStretch(1);
    _searchHLayout->addWidget(new QLabel("Column:"),1);
    _searchHLayout->addWidget(_columnComb,2);
    _searchHLayout->addStretch(1);
    _searchHLayout->addWidget(new QLabel("Syntax:"),1);
    _searchHLayout->addWidget(_syntaxComb,2);
    _searchHLayout->addStretch(1);
    _searchHLayout->addWidget(_filterCaseSensitivityCheckBox,2);
    _searchHLayout->addStretch(1);
    _searchHLayout->addWidget(new QLabel("Pattern:"),1);
    _searchHLayout->addWidget(_searchLineEdit,4);
    _searchHLayout->addStretch(1);
    _searchHLayout->addWidget(_searchButton,1);
    _searchHLayout->addStretch(3);

    _mainHLayout = new QVBoxLayout();
    _mainHLayout->addLayout(_headHLayout);
    _mainHLayout->addLayout(_bodyHLayout);
    _mainHLayout->addLayout(_searchHLayout);

    ui->_widget->setLayout(_mainHLayout);

    _time = 0;
    _key.clear();
    _type.clear();
    _vCmdMsg.clear();
    _valueProxyModel = nullptr;
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
    connect(_searchButton, &QPushButton::clicked, this, &DataView::search);
    connect(_columnComb, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &DataView::filterColumnChanged);
    connect(_filterCaseSensitivityCheckBox, &QAbstractButton::toggled,this, &DataView::filterRegExpChanged);
    connect(_syntaxComb, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &DataView::filterRegExpChanged);
}

DataView::~DataView()
{
    delete ui;
}

void DataView::flush() {
    initValueListData();
}

void DataView::count() {
    QString str;
    if(_valueProxyModel)
        str = QString(tr("统计值总数为")) + QString::number(_valueProxyModel->rowCount());
    else
        str = QString(tr("统计值总数为0"));
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
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _tableItemValue = new ValueTableItem(QString(valueList[i]));
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
        QList<QByteArray> valueList = _inputDialog->getTextList();
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
        QList<QByteArray> valueList = _inputDialog->getTextList();
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
        QList<QByteArray> valueList = _inputDialog->getTextList();
        for(int i =0; i < valueList.size(); ++i) {
            _tableItemValue = new ValueTableItem(QString(valueList[i]));
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
        QList<QByteArray> valueList = _inputDialog->getTextList();
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

    if(!_valueProxyModel)
        return;

    if(_valueProxyModel->rowCount() <= 0) {
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
        QModelIndex index = _valueProxyModel->mapToSource(selectList.at(i));
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
}

void DataView::initValueListData() {
    _key = _keyLineEdit->text().trimmed();
    if(_key.isEmpty())
        return;

    _listIndex = 0;
    _vCmdMsg.clear();
    _itemTableModel->clear();
    _columnComb->clear();
    _initValueMsg.init();

    if(_valuePattern != _valueScanPattern->text()) {
        _valuePattern = _valueScanPattern->text();
        PubLib::setConfig("initValuePattern", _valuePattern);
    }

    _initValueMsg._key = _key;
    _initValueMsg._type = _type;
    _initValueMsg._dbindex = _dbIndex;
    _initValueMsg._clientIndex = _clientIndex;
    _initValueMsg._valuePattern = _valuePattern;

    if(_valueProxyModel) {
        delete _valueProxyModel;
        _valueProxyModel = nullptr;
    }
    _valueProxyModel = new ValueTableProxyModel;
    _valueProxyModel->setSourceModel(_itemTableModel);
    _valueProxyModel->setFilterRole(Qt::DisplayRole);
    _valueProxyModel->setDynamicSortFilter(false);

    _tableView->setSortingEnabled(true);
    if(_type == "string") {
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) | Qt::ItemIsEditable);
        _columnComb->addItem("VALUE");
        emit getData(_initValueMsg);
    } else if(_type == "hash") {
        _itemTableModel->setColumnCount(2);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "FIELD");
        _itemTableModel->setHeaderData(1,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        _itemTableModel->setFlags(1,_itemTableModel->flags(1) | Qt::ItemIsEditable);
        _columnComb->addItem("FIELD");
        _columnComb->addItem("VALUE");
        emit getData(_initValueMsg);
    } else if(_type == "list") {
        _listIndex = 0;
        _tableView->setSortingEnabled(false);
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "VALUE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) | Qt::ItemIsEditable);
        _columnComb->addItem("VALUE");
        emit getData(_initValueMsg);
    } else if(_type == "set") {
        _itemTableModel->setColumnCount(1);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        _columnComb->addItem("MEMBER");
        emit getData(_initValueMsg);
    } else if(_type == "zset") {
        _itemTableModel->setColumnCount(2);
        _itemTableModel->setHeaderData(0,Qt::Horizontal, "MEMBER");
        _itemTableModel->setHeaderData(1,Qt::Horizontal, "SCORE");
        _itemTableModel->setFlags(0,_itemTableModel->flags(0) & (~Qt::ItemIsEditable));
        _itemTableModel->setFlags(1,_itemTableModel->flags(1) & (~Qt::ItemIsEditable));
        _columnComb->addItem("MEMBER");
        _columnComb->addItem("SCORE");
        emit getData(_initValueMsg);
    }
    _columnComb->setCurrentIndex(0);
    filterColumnChanged();
    _tableView->setModel(_valueProxyModel);
}

void DataView::appendValue(const QList<QByteArray> & vList, const QByteArray flag) {
    if(flag == "hash") {
        for(int i = 0; i < vList.size(); ++++i) {
            vRowData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            vRowData.push_back(QVariant(_value));
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i+1]);
            vRowData.push_back(QVariant(_value));
            _tableItemValue = new ValueTableItem(vRowData);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "zset") {
        for(int i = 0; i < vList.size(); ++++i) {
            vRowData.clear();
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            vRowData.push_back(QVariant(_value));
            vRowData.push_back(QVariant(vList[i+1].toDouble()));
            _tableItemValue = new ValueTableItem(vRowData);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "set") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
        }
    } else if(flag == "string") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
            break;
        }
    } else if(flag == "list") {
        for(int i = 0; i < vList.size(); ++i) {
            _value = QTextCodec::codecForLocale()->toUnicode(vList[i]);
            _tableItemValue = new ValueTableItem(_value);
            _itemTableModel->insertRow(_tableItemValue);
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

void DataView::search() {
    if(_columnComb->count() == 0)
        return;
    filterRegExpChanged();
}

void DataView::filterRegExpChanged()
{
    if(!_valueProxyModel)
        return;

    if(_columnComb->count() == 0)
        return;

    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(_syntaxComb->itemData(_syntaxComb->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            _filterCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegExp regExp(_searchLineEdit->text(), caseSensitivity, syntax);
    _valueProxyModel->setFilterRegExp(regExp);
}

void DataView::filterColumnChanged()
{
    if(_columnComb->count() == 0)
        return;

    if(_valueProxyModel)
        _valueProxyModel->setFilterKeyColumn(_columnComb->currentIndex());
}
