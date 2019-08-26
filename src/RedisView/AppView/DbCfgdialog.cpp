/**
* @file      DbCfgdialog.cpp
* @brief     数据库配置
* @author    王长春
* @date      2019-06-26
* @version   001
* @copyright Copyright (c) 2018
*/
#include "DbCfgDialog.h"

DbCfgDialog::DbCfgDialog(int dbtype, QWidget *parent) :
    QDialog(parent) {
    setWindowTitle(tr("数据库配置"));
    _dbtype = dbtype;

    QLabel *labelUserName = new QLabel("用  户 ");
    QLabel *labelPassword = new QLabel("密  码 ");
    QLabel *labelPort = new QLabel("端  口 ");
    QLabel *labelDatabase = new QLabel("实例名 ");
    QLabel *labelHostName = new QLabel("主机IP ");
    QLabel *labelTagName = new QLabel("标签名 ");

    _username = new QLineEdit;
    _password = new QLineEdit;
    _port = new QLineEdit;
    _hostname = new QLineEdit;
    _tagname = new QComboBox;
    _tagname->setEditable(true);
    _database = new QLineEdit;
    _password->setEchoMode(QLineEdit::Password);

    QPushButton *OK = new QPushButton("确定");
    QPushButton *Exit = new QPushButton("取消");
    connect(OK, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(Exit, SIGNAL(clicked()), this, SLOT(onExit()));

    QVBoxLayout *vBox = new QVBoxLayout;
    QHBoxLayout *hBox = new QHBoxLayout;
    QGridLayout *grid = new QGridLayout;
    QGridLayout *mainLayout = new QGridLayout;
    vBox->addLayout(grid);
    vBox->addWidget(new QLabel(" "));
    vBox->addLayout(hBox);

    mainLayout->addWidget(new QLabel("   "), 0, 0);
    mainLayout->addLayout(vBox, 0, 1);
    mainLayout->addWidget(new QLabel("   "), 0, 2);
    setLayout(mainLayout);

    grid->addWidget(labelUserName, 0, 0);
    grid->addWidget(labelPassword, 1, 0);
    grid->addWidget(labelPort, 2, 0);
    grid->addWidget(labelDatabase, 3, 0);
    grid->addWidget(labelHostName, 4, 0);
    grid->addWidget(labelTagName, 5, 0);

    grid->addWidget(_username, 0, 1);
    grid->addWidget(_password, 1, 1);
    grid->addWidget(_port, 2, 1);
    grid->addWidget(_database, 3, 1);
    grid->addWidget(_hostname, 4, 1);
    grid->addWidget(_tagname, 5, 1);

    hBox->addWidget(OK);
    hBox->addWidget(Exit);

    if(_dbtype == ORACLE_DB) {
        labelDatabase->setText("实例名 ");
    } else if(_dbtype == MYSQL_DB) {
        labelDatabase->setText("数据库 ");
    }

    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    QString lasttag = settings.value(QString("lastUseTag%1").arg(_dbtype)).toString();
    int lastIndex = 0;
    int size = 0;

    if(_dbtype == ORACLE_DB)
        size = settings.beginReadArray(CORACLE_DB);
    else if(_dbtype == MYSQL_DB)
        size = settings.beginReadArray(CMYSQL_DB);

    // 读取配置初始化
    _loginInfo.clear();
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        DbCfg login;
        login.userName = settings.value("userName").toString().trimmed();
        login.encodePasswd = settings.value("passWord").toByteArray();
        login.password = AesEncrypt::CBC256Decrypt(login.encodePasswd).trimmed();
        login.database = settings.value("dataBase").toString().trimmed();
        login.encodeHostname = settings.value("hostName").toByteArray();
        login.hostname = AesEncrypt::CBC256Decrypt(login.encodeHostname).trimmed();
        login.port = settings.value("dbPort").toString().trimmed();
        login.tagname = settings.value("tagname").toString().trimmed();
        if(!login.tagname.isEmpty()) {
            if(login.tagname == lasttag)
                lastIndex = i;
            _loginInfo.insert(login.tagname, login);
            _tagname->addItem(login.tagname);
        }
    }
    settings.endArray();

    connect(_tagname, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged),
            [ = ](int index) {
        _tagname->setCurrentIndex(index);
        QString text = _tagname->currentText();
        if(!text.isEmpty()) {
            _username->setText(_loginInfo[text].userName);
            _password->setText(_loginInfo[text].password);
            _port->setText(_loginInfo[text].port);
            _database->setText(_loginInfo[text].database);
            _hostname->setText(_loginInfo[text].hostname);
            _tagname->setCurrentText(_loginInfo[text].tagname);
        }
    }
    );
    // 发信号触发一次曹函数来初始化
    emit _tagname->currentIndexChanged(lastIndex);
}

void DbCfgDialog::onOK() {

    QString strUsername = _username->text().trimmed();
    QString strPassword = _password->text().trimmed();
    QString strHostname = _hostname->text().trimmed();
    QString strDatabase = _database->text().trimmed();
    QString strDbPort = _port->text().trimmed();
    QString strTagname = _tagname->currentText().trimmed();

    if(strUsername.length() == 0) {
        QMessageBox::critical(this, "错误", "用户名不能为空!  ");
        _username->setFocus();
        return;
    }
    if(strPassword.length() == 0) {
        QMessageBox::critical(this, "错误", "密码不能为空!  ");
        _password->setFocus();
        return;
    }
    if(strDatabase.length() == 0) {
        QMessageBox::critical(this, "错误", "数据库名不能为空!  ");
        _database->setFocus();
        return;
    }
    if(strHostname.length() == 0) {
        QMessageBox::critical(this, "错误", "数据库主机不能为空!  ");
        _hostname->setFocus();
        return;
    }
    if(strDbPort.length() == 0) {
        QMessageBox::critical(this, "错误", "数据库端口不能为空!  ");
        _port->setFocus();
        return;
    }
    if(strTagname.length() == 0) {
        QMessageBox::critical(this, "错误", "数据库说明标签不能为空!  ");
        _tagname->setFocus();
        return;
    }

    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    DbCfg login;
    login.userName = strUsername;
    login.password = strPassword;
    login.database = strDatabase;
    login.hostname = strHostname;
    login.port = strDbPort;
    login.tagname = strTagname;
    _loginInfo.insert(login.tagname, login);

    settings.setValue(QString("lastUseTag%1").arg(_dbtype), login.tagname);
    if(_dbtype == ORACLE_DB) {
        settings.remove(CORACLE_DB);
        settings.beginWriteArray(CORACLE_DB);
    } else if(_dbtype == MYSQL_DB) {
        settings.remove(CMYSQL_DB);
        settings.beginWriteArray(CMYSQL_DB);
    }
    int i = 0;
    foreach(DbCfg value, _loginInfo) {
        settings.setArrayIndex(i);
        i++;
        settings.setValue("userName", value.userName);
        value.encodePasswd = AesEncrypt::CBC256Crypt(value.password);
        settings.setValue("passWord", value.encodePasswd);
        settings.setValue("dataBase", value.database);
        value.encodeHostname = AesEncrypt::CBC256Crypt(value.hostname);
        settings.setValue("hostName", value.encodeHostname);
        settings.setValue("dbPort", value.port);
        settings.setValue("tagname", value.tagname);
    }
    settings.endArray();
    accept();
}

// exit login
void DbCfgDialog::onExit() {
    reject();
}
