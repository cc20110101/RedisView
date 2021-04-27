/**
* @file      InputDialog.cpp
* @brief     键值设置输入
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/InputDialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("插入"));
    setFixedSize(this->size());
}

InputDialog::~InputDialog()
{
    delete ui;
}

QString InputDialog::getText() {
    return ui->_inputTextEdit->document()->toPlainText().trimmed();
}

QList<QString> InputDialog::getTextList() {
    return _textList;
}

void InputDialog::setTip(const QString & info) {
    ui->_inputTextEdit->setPlaceholderText(info);
}

void InputDialog::setType(const int & type) {
    _type = type;
}

void InputDialog::on__pushButtonY_clicked()
{
    _text = getText();
    if(_text.trimmed().isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("数据为空，请重新输入!"));
        return;
    }

    if(_type == KEY_HASH) {
        _textList.clear();
        PubLib::getList(_text,_textList);
        if(_textList.size() % 2) {
            QMessageBox::critical(this, tr("错误"), tr("数据错误，哈希字段值个数不匹配"));
            return;
        }
    } else if(_type == KEY_SET || _type == KEY_LIST) {
        _textList.clear();
        PubLib::getList(_text,_textList);
    } else if(_type == KEY_ZSET) {
        _textList.clear();
        PubLib::getList(_text,_textList);
        if(_textList.size() % 2) {
            QMessageBox::critical(this, tr("错误"), tr("数据错误，有序集合成员分数个数不匹配"));
            return;
        }

        QChar c;
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

    accept();
}

void InputDialog::clear() {
    _textList.clear();
    ui->_inputTextEdit->clear();
}

void InputDialog::on__pushButtonN_clicked()
{
    reject();
}
