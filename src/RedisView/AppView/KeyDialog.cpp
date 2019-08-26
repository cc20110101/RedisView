/**
* @file      KeyDialog.cpp
* @brief     键设置输入
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/KeyDialog.h"

KeyDialog::KeyDialog(QWidget *parent) :
    QDialog(parent) {

    _labelKey = new QLabel("KEY:");
    _labelTtl = new QLabel("TTL(ms):");
    _labelType = new QLabel("TYPE:");

    _editKey = new QLineEdit();
    _editTtl = new QLineEdit();
    _combType = new QComboBox();

    _textEdit = new QTextEdit();
    _buttonOk= new QPushButton(tr("确定"));
    _buttonNo = new QPushButton(tr("取消"));

    _grid = new QGridLayout(this);
    _grid->addWidget(_labelKey,0,0,1,1);
    _grid->addWidget(_editKey,0,1,1,9);
    _grid->addWidget(_labelTtl,0,11,1,1);
    _grid->addWidget(_editTtl,0,12,1,5);
    _grid->addWidget(_labelType,1,0,1,1);
    _grid->addWidget(_combType,1,1,1,9);
    _grid->addWidget(_textEdit,2,0,3,17);
    _grid->addWidget(_buttonOk,5,13,1,2);
    _grid->addWidget(_buttonNo,5,15,1,2);

    _combType->addItem("String");
    _combType->addItem("Hash");
    _combType->addItem("Set");
    _combType->addItem("ZSet");
    _combType->addItem("List");
    _editTtl->setPlaceholderText(tr("空不修改，负为永久"));

    connect(_buttonOk, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(_buttonNo, SIGNAL(clicked()), this, SLOT(onExit()));
    connect(_combType, static_cast<void(QComboBox::*)(const QString &)>
            (&QComboBox::currentTextChanged),
            [ = ](const QString & str) {
        _strType = str;
        if(_strType == "String") {
            _textEdit->setPlaceholderText("value");
        } else if(_strType == "Hash") {
            _textEdit->setPlaceholderText("filed1 value1 filed2 value2...");
        } else if(_strType == "Set") {
            _textEdit->setPlaceholderText("member1 member2...");
        } else if(_strType == "ZSet") {
            _textEdit->setPlaceholderText("member1 score1 member2 score2 ...");
        } else if(_strType == "List") {
            _textEdit->setPlaceholderText("member1 member2...");
        }
    });
    emit _combType->currentTextChanged("String");
}

void KeyDialog::init() {
    _editKey->clear();
    _editTtl->clear();
    _textEdit->clear();
}

QString KeyDialog::getType() const
{
    return _strType;
}

QString KeyDialog::getKey() const
{
    return _strKey;
}

QString KeyDialog::getTtl() const
{
    return _strTtl;
}

QString KeyDialog::getValue() const
{
    return _strValue;
}

QList<QString> KeyDialog::getTextList() const
{
    return _textList;
}

void KeyDialog::setKey(const QString &strKey)
{
    _strKey = strKey;
}

void KeyDialog::setFlag(int flag)
{
    _flag = flag;
    switch(_flag) {
    case 0:
        _combType->setVisible(true);
        _textEdit->setVisible(true);
        _labelType->setVisible(true);
        break;
    case 1:
        _combType->setVisible(false);
        _textEdit->setVisible(false);
        _labelType->setVisible(false);
        _editKey->setText(_strKey);
        _editTtl->setText(_strTtl);
        break;
    default:
        break;
    }
}

void KeyDialog::setTtl(const QString &strTtl)
{
    _strTtl = strTtl;
}

void KeyDialog::setType(const QString &strType)
{
    _strType = strType;
}

void KeyDialog::onOK() {
    _strKey = _editKey->text().trimmed();
    if(_strKey.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("键不可为空"));
        return;
    }

    _strTtl = _editTtl->text().trimmed();
    QChar c;
    _strbTtl = _strTtl.toLocal8Bit();
    for(int i = 0; i < _strbTtl.size(); ++i) {
        c = _strbTtl.at(i);
        if(!c.isDigit()) {
            if(c != '-') {
                QMessageBox::critical(this, tr("错误"), tr("数据错误，键超时时间不是有效数值"));
                return;
            }
        }
    }

    if(!_flag) {
        _strValue = _textEdit->document()->toPlainText().trimmed();
        if(_strValue.isEmpty()) {
            QMessageBox::critical(this, tr("错误"), tr("值不可为空"));
            return;
        }

        _strType = _combType->currentText();
        if(_strType == "Hash") {
            _textList.clear();
            PubLib::getList(_strValue,_textList);
            if(_textList.size() % 2) {
                QMessageBox::critical(this, tr("错误"), tr("数据错误，哈希字段值个数不匹配"));
                return;
            }
        } else if(_strType == "Set" || _strType == "List") {
            _textList.clear();
            PubLib::getList(_strValue,_textList);
        } else if(_strType == "ZSet") {
            _textList.clear();
            PubLib::getList(_strValue,_textList);
            if(_textList.size() % 2) {
                QMessageBox::critical(this, tr("错误"), tr("数据错误，有序集合成员分数个数不匹配"));
                return;
            }

            for(int i =0 ; i < _textList.size(); ++++i) {
                for(int j = 0; j < _textList[i+1].size(); ++j) {
                    c = _textList[i+1].at(j);
                    if(!c.isDigit()) {
                        QMessageBox::critical(this, tr("错误"), tr("数据错误，分数不是有效数值"));
                        return;
                    }
                }
            }
        }
    }

    accept();
}

void KeyDialog::onExit() {
    reject();
}
