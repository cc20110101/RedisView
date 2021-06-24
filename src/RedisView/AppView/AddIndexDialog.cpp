/**
* @file      AddIndexDialog.cpp
* @brief     添加数据库索引数对话框
* @author    王长春
* @date      2021-06-21
* @version   001
* @copyright Copyright (c) 2021
*/
#include "AppView/AddIndexDialog.h"

AddIndexDialog::AddIndexDialog(QWidget *parent) :
    QDialog(parent) {

    setWindowTitle(tr("添加索引数"));

    _labelDbNum = new QLabel(tr("索引数: "));
    _editDbNum = new QLineEdit();
    _buttonOk= new QPushButton(tr("确定"));
    _buttonNo = new QPushButton(tr("取消"));

    _grid = new QGridLayout(this);
    _grid->addWidget(_labelDbNum,0,0,1,1);
    _grid->addWidget(_editDbNum,0,1,1,9);
    _grid->addWidget(_buttonOk,2,6,1,2);
    _grid->addWidget(_buttonNo,2,8,1,2);

    connect(_buttonOk, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(_buttonNo, SIGNAL(clicked()), this, SLOT(onExit()));
}

void AddIndexDialog::init() {
    _editDbNum->clear();
}

int AddIndexDialog::dbNum() const
{
    return _dbNum;
}

void AddIndexDialog::onOK() {
    _dbNum = _editDbNum->text().trimmed().toInt();
    accept();
}

void AddIndexDialog::onExit() {
    reject();
}
