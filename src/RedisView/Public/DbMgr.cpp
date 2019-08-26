/**
* @file      DbMgr.cpp
* @brief     数据库连接
* @author    王长春
* @date      2019-06-28
* @version   001
* @copyright Copyright (c) 2018
*/
#include "DbMgr.h"

DbMgr::DbMgr()
{

}

QString DbMgr::host()
{
    return _host;
}

void DbMgr::setHost(const QString &host)
{
    _host = host;
}

QString DbMgr::user()
{
    return _user;
}

void DbMgr::setUser(const QString &user)
{
    _user = user;
}

QString DbMgr::passwd()
{
    return _passwd;
}

void DbMgr::setPasswd(const QString &passwd)
{
    _passwd = passwd;
}

QString DbMgr::database()
{
    return _database;
}

void DbMgr::setDatabase(const QString &database)
{
    _database = database;
}

QString DbMgr::dbport()
{
    return _dbport;
}

void DbMgr::setDbport(const QString &dbport)
{
    _dbport = dbport;
}

QString DbMgr::dbdrive()
{
    return _dbdrive;
}

void DbMgr::setDbdrive(const QString &dbdrive)
{
    _dbdrive = dbdrive;
}

void DbMgr::close(QString lable) {

    if(lable.isEmpty())
        lable = _lable;

    if(_lable.isEmpty())
        return;

    QMutexLocker locker(&G_DB_MUTEX);

    if(QSqlDatabase::contains(lable)) {
        QSqlDatabase::database(lable).close();
        QSqlDatabase::removeDatabase(lable);
    }
}

bool DbMgr::getDb(QSqlDatabase *db, QString lable) {

    // 官方说MYSQ线程安全,假的
    // Qt动态的加载数据库的plugin,涉及到对本地库文件的管理，出现了竞争
    QMutexLocker locker(&G_DB_MUTEX);
    if(lable.isEmpty())
        lable = _lable;

    if(_lable.isEmpty()) {
        setError(QObject::tr("数据库连接名为空"));
        return false;
    }

    if(QSqlDatabase::contains(lable)) {
        *db = QSqlDatabase::database(lable);
        return true;
    }

    *db = QSqlDatabase::addDatabase(_dbdrive, lable);
    db->setHostName(_host);
    db->setUserName(_user);
    db->setPassword(_passwd);
    db->setPort(_dbport.toInt());
    db->setDatabaseName(_database);
    if(db->open() == false) {
        setError(db->lastError().text().toUtf8());
        return false;
    }
    return true;
}

bool DbMgr::setCfg(int dbtype, QString drive) {
    if(drive.isEmpty()) {
        setError(QObject::tr("错误,数据库驱动为空"));
        return false;
    }
    setDbtype(dbtype);
    setDbdrive(drive);

    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    QString lasttag = settings.value(QString("lastUseTag%1").arg(_dbtype)).toString();
    int size = 0;
    DbCfg dbcfg;

    if(_dbtype == ORACLE_DB)
        size = settings.beginReadArray(CORACLE_DB);
    else if(_dbtype == MYSQL_DB)
        size = settings.beginReadArray(CMYSQL_DB);

    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        dbcfg.userName = settings.value("userName").toString().trimmed();
        dbcfg.encodePasswd = settings.value("passWord").toByteArray();
        dbcfg.password = AesEncrypt::CBC256Decrypt(dbcfg.encodePasswd).trimmed();
        dbcfg.database = settings.value("dataBase").toString().trimmed();
        dbcfg.encodeHostname = settings.value("hostName").toByteArray();
        dbcfg.hostname = AesEncrypt::CBC256Decrypt(dbcfg.encodeHostname).trimmed();
        dbcfg.port = settings.value("dbPort").toString().trimmed();
        dbcfg.tagname = settings.value("tagname").toString().trimmed();
        if(!dbcfg.tagname.isEmpty()) {
            if(dbcfg.tagname == lasttag) {
                size = -1;
                setUser(dbcfg.userName);
                setPasswd(dbcfg.password);
                setDatabase(dbcfg.database);
                setHost(dbcfg.hostname);
                setDbport(dbcfg.port);
                setLable(dbcfg.tagname);
                break;
            }
        }
    }
    settings.endArray();

    if(size != -1) {
        setError(QObject::tr("错误,数据库配置读取失败"));
        return false;
    }
    return true;
}

int DbMgr::getDbtype() const
{
    return _dbtype;
}

void DbMgr::setDbtype(int dbtype)
{
    _dbtype = dbtype;
}

QString DbMgr::lable()
{
    return _lable;
}

void DbMgr::setLable(const QString &lable)
{
    _lable = lable;
}

QString DbMgr::error()
{
    return _error;
}

void DbMgr::setError(const QString &error)
{
    _error = error;
}
