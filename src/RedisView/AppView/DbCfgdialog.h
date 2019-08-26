/**
* @file      DbCfgdialog.h
* @brief     数据库配置
* @author    王长春
* @date      2019-06-26
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef DBCFGDIALOG_H
#define DBCFGDIALOG_H

#include "Public/Publib.h"

class DbCfgDialog : public QDialog {
    Q_OBJECT
public:
    explicit DbCfgDialog(int dbtype = 0, QWidget *parent = nullptr);
    QLineEdit *_username;
    QLineEdit *_password;
    QLineEdit *_port;
    QLineEdit *_hostname;
    QLineEdit *_database;
    QComboBox *_tagname;

    bool loginDatabase(const QString &host,
                       const QString &user,
                       const QString &passwd,
                       const QString &_database,
                       const QString &dbport);
private:

    QMap<QString, DbCfg> _loginInfo;
    int _dbtype;

signals:

public slots:
    void onOK();
    void onExit();

};

#endif // DBCFGDIALOG_H
