/**
* @file      LoginSet.cpp
* @brief     登入信息设置
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/LoginSet.h"

LoginSet::LoginSet(QWidget *parent) :
    QDialog(parent) {

    setWindowTitle(tr("连接设置"));
    _clientInfo._addr.clear();
    _clientInfo._name.clear();
    _clientInfo._passwd.clear();

    _lb_name = new QLabel(tr("名称:"));
    _le_name = new QLineEdit();

    _lb_addr = new QLabel(tr("地址:"));
    _le_addr = new QLineEdit();
    _le_addr->setPlaceholderText("ip1:port1,ip2:port2...");

    _lb_passwd = new QLabel(tr("密码:"));
    _le_passwd = new QLineEdit();
    _le_passwd->setEchoMode(QLineEdit::Password);

    QPushButton *OK = new QPushButton(tr("确定"));
    QPushButton *Exit = new QPushButton(tr("取消"));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(_lb_name,0,0,1,1);
    mainLayout->addWidget(_le_name,0,1,1,10);

    mainLayout->addWidget(_lb_addr,1,0,1,1);
    mainLayout->addWidget(_le_addr,1,1,1,10);

    mainLayout->addWidget(_lb_passwd,2,0,1,1);
    mainLayout->addWidget(_le_passwd,2,1,1,10);

    mainLayout->addWidget(OK,3,9,1,1);
    mainLayout->addWidget(Exit,3,10,1,1);

    setLayout(mainLayout);

    QRect availableGeometry = QApplication::desktop()->availableGeometry( this);
    resize(availableGeometry.width() * 2 / 5, availableGeometry.height() * 1 / 4);
    move((availableGeometry.width() - width()) / 2,
         (availableGeometry.height() - height()) / 2);

    connect(OK, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(Exit, SIGNAL(clicked()), this, SLOT(onExit()));
}

void LoginSet::getClientInfo(ClientInfoDialog & clientInfo)
{
    clientInfo._addr = _clientInfo._addr;
    clientInfo._name = _clientInfo._name;
    clientInfo._passwd = _clientInfo._passwd;
}

void LoginSet::setClientInfo(ClientInfoDialog & clientInfo)
{
    _clientInfo._addr = clientInfo._addr;
    _clientInfo._name = clientInfo._name;
    _clientInfo._passwd = clientInfo._passwd;
    _le_name->setText(_clientInfo._name);
    _le_addr->setText(_clientInfo._addr);
    _le_passwd->setText(_clientInfo._passwd);
}

void LoginSet::onOK() {

    _clientInfo._name = _le_name->text().trimmed();
    if(_clientInfo._name.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("数据库连接标签名称不可为空!"));
        _le_name->setFocus();
        return;
    }

    _clientInfo._addr = _le_addr->text().trimmed();
    if(_clientInfo._addr.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("数据库连接串不可为空!"));
        _le_addr->setFocus();
        return;
    }
    _clientInfo._passwd = _le_passwd->text().trimmed();

    accept();
}

void LoginSet::onExit() {
    reject();
}
