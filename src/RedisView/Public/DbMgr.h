/**
* @file      DbMgr.h
* @brief     数据库连接
* @author    王长春
* @date      2019-06-28
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef DBMGR_H
#define DBMGR_H

#include "Public/Define.h"

class DbMgr
{

public:
    DbMgr();

    QString host();
    void setHost(const QString &host);

    QString user();
    void setUser(const QString &user);

    QString passwd();
    void setPasswd(const QString &passwd);

    QString database();
    void setDatabase(const QString &database);

    QString dbport();
    void setDbport(const QString &dbport);

    QString dbdrive();
    void setDbdrive(const QString &dbdrive);

    QString error();
    void setError(const QString &error);

    QString lable();
    void setLable(const QString &lable);

    bool getDb(QSqlDatabase *db, QString lable = "");

    int getDbtype() const;
    void setDbtype(int dbtype);

    bool setCfg(int dbtype,QString drive);

    void close(QString lable = "");


private:
    int _dbtype;
    QString _lable;
    QString _host;
    QString _user;
    QString _passwd;
    QString _database;
    QString _dbport;
    QString _dbdrive;
    QString _error;

};

#endif // DBMGR_H
