/**
* @file      Define.cpp
* @brief     公共定义
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QWidget>
#include <QTextBrowser>
#include <QDomDocument>
#include <QMessageBox>
#include <QTextCodec>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QTreeWidget>
#include <QMessageBox>
#include <QSettings>
#include <QList>
#include <QMap>
#include <QMenu>
#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QDebug>
#include <QToolBar>
#include <QStatusBar>
#include <QtWidgets>
#include <QThreadPool>
#include <QThread>
#include <QMetaType>
#include <QReadLocker>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QTranslator>
#include <QRegExp>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QActionGroup>
#include <QToolButton>
#include <QWidgetAction>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMutex>
#include <QNetworkInterface>
#include "RedisLib/RedisCluster.h"
#include "Public/AesEncrypt.h"
#ifdef __GNUC__
#include <cpuid.h>
#elif defined(_MSC_VER)
#if _MSC_VER >= 1400
#include <intrin.h>
#endif
#else
#error Only supports MSVC or GCC
#endif

// 定义字符串
#define WindowTitle             "RedisView Community v1.7.2"
#define IniFileName             "conf.ini"
#define LogName                 "process.log"
#define OrganizationName        "CC20110101"
#define ApplicationName         "RedisView"
#define ApplicationVersion      "1.7.2"

// 定义常量
#define BATCH_SCAN_NUM                             5000
#define MAX_THREAD_COUNT                           6
#define ORACLE_DB                                  0
#define MYSQL_DB                                   1
#define CORACLE_DB                                 "oracledb"
#define CMYSQL_DB                                  "mysqldb"
#define ORACLE_DRIVE                               "QOCI"
#define MYSQL_DRIVE                                "QMYSQL"
#define SQLITE_DRIVE                               "QSQLITE"
#define WORK_THREAD_MODE0                           0
#define WORK_THREAD_MODE1                           1

// 定义线程任务
#define THREAD_SCAN_KEY_TASK                       1
#define THREAD_SCAN_VALUE_TASK                     2
#define THREAD_COMMIT_VALUE_TASK                   3
#define THREAD_DEL_KEY_TASK                        4
#define THREAD_BATCH_DEL_KEY_TASK                  5
#define THREAD_BATCH_SCAN_KEY_TASK                 6
#define THREAD_BATCH_OIM_KEY_TASK                  7
#define THREAD_BATCH_MIM_KEY_TASK                  8
#define THREAD_BATCH_OEM_KEY_TASK                  9
#define THREAD_BATCH_MEM_KEY_TASK                  10
#define THREAD_BATCH_MDE_KEY_TASK                  11
#define THREAD_BATCH_ODE_KEY_TASK                  12

// 定义操作符
#define OPERATION_ADD                              1
#define OPERATION_DELETE                           2
#define OPERATION_ALTER                            3
#define OPERATION_TIMEOUT                          4
#define OPERATION_RENAME                           5

// 定义键类型
#define KEY_NONE                                   0
#define KEY_STRING                                 1
#define KEY_HASH                                   2
#define KEY_SET                                    3
#define KEY_ZSET                                   4
#define KEY_LIST                                   5

// 定义样式
#define DEEPDARK_THEME             "DeepDarkTheme"
#define DARK_THEME                 "DarkTheme"
#define GRAY_THEME                 "GrayTheme"
#define PINK_THEME                 "PinkTheme"
#define NO_THEME                   "NoTheme"

// 定义样式文件
#define DARK_THEME_FILE            ":/Resources/DarkTheme.qss"
#define GRAY_THEME_FILE            ":/Resources/GrayTheme.qss"
#define PINK_THEME_FILE            ":/Resources/PinkTheme.qss"
#define DEEPDARK_THEME_FILE        ":/Resources/DeepDarkTheme.qss"

// 定义图标动画
#define ICON_TRAY                  ":/Resources/tray.ico"
#define ICON_FLUSHK                ":/Resources/keyflush.ico"
#define ICON_ABOUT                 ":/Resources/about.ico"
#define ICON_EXIT                  ":/Resources/exit.ico"
#define ICON_INFO                  ":/Resources/info.ico"
#define ICON_COUNT                 ":/Resources/count.ico"
#define ICON_TAB                   ":/Resources/tabpage.ico"
#define ICON_SAVE                  ":/Resources/save.ico"
#define ICON_DISPLAY               ":/Resources/display.ico"
#define ICON_HIDE                  ":/Resources/hide.ico"
#define ICON_RUN                   ":/Resources/run.ico"
#define ICON_LONGIN                ":/Resources/login.ico"
#define ICON_ADD                   ":/Resources/add.ico"
#define ICON_DEL                   ":/Resources/delete.ico"
#define ICON_ALTER                 ":/Resources/alter.ico"
#define ICON_TYPE                  ":/Resources/type.ico"
#define ICON_KEY                   ":/Resources/key.ico"
#define ICON_DB                    ":/Resources/db.ico"
#define ICON_DONATE                ":/Resources/donate.ico"
#define ICON_FLUSH                 ":/Resources/flush.ico"
#define ICON_COMMIT                ":/Resources/commit.ico"
#define ICON_IHEAD                 ":/Resources/inhead.ico"
#define ICON_ITAIL                 ":/Resources/intail.ico"
#define ICON_DHEAD                 ":/Resources/delhead.ico"
#define ICON_DTAIL                 ":/Resources/deltail.ico"
#define ICON_INSTRUCTION           ":/Resources/instruction.ico"
#define ICON_LANGUAGE              ":/Resources/language.ico"
#define ICON_EN                    ":/Resources/en.ico"
#define ICON_CN                    ":/Resources/cn.ico"
#define ICON_HISTORY               ":/Resources/history.ico"
#define ICON_SUBSCRIBE             ":/Resources/subcribe.ico"
#define ICON_VIEW                  ":/Resources/view.ico"
#define ICON_RECONNECT             ":/Resources/reconnect.ico"
#define ICON_DELKEY                ":/Resources/delkey.ico"
#define ICON_BATCHOP               ":/Resources/batchop.ico"
#define ICON_ENCODE                ":/Resources/encode.ico"
#define ICON_SELECT                ":/Resources/select.ico"
#define ICON_SORT                  ":/Resources/sort.ico"
#define ICON_CONTRIBUTOR           ":/Resources/contributor.ico"
#define ICON_REDISINFO             ":/Resources/redisinfo.ico"
#define ICON_FEEDBACK              ":/Resources/feedback.ico"
#define ICON_UPDATE                ":/Resources/update.ico"
#define ICON_DETAILS               ":/Resources/details.ico"
#define ICON_ADDDB                 ":/Resources/adddb.ico"
#define GIF_WAIT                   ":/Resources/wait.gif"

extern  QMutex  G_DB_MUTEX;
extern  QMutex  G_SEQUENCE_MUTEX;
extern  QMutex  G_PUBLIC_LIB_MUTEX;

// 定义公共类
class Global {
public:
    Global();
    ~Global();

public:
    static QString gEncode;
    static QString gTheme;
    static QString gConnectName;
    static QTranslator * gTrans;
};

class ClientInfoDialog
{
public:
    ClientInfoDialog() {
        init();
    }

public:
    bool operator == (const ClientInfoDialog& d)
    {
        return this->_name == d._name;
    }

    void init() {
        _indexNums = 0;
        _name.clear();
        _addr.clear();
        _encodeAddr.clear();
        _passwd.clear();
        _encode.clear();
        _keyPattern.clear();
        _valuePattern.clear();
        _encodePasswd.clear();
    }

    int _indexNums;
    QString _name;
    QString _addr;
    QString _passwd;
    QString _encode;
    QString _keyPattern;
    QString _valuePattern;
    QByteArray _encodePasswd;
    QByteArray _encodeAddr;

};

class TaskMsg {
public:
    TaskMsg() {
        init();
    }

public:
    void init() {
        _clusterMode = false;
        _customMode = false;
        _port = 0;
        _dbIndex = 0;
        _taskid = 0;
        _sequence = 0;
        _clientIndex = 0;
        _type = KEY_NONE;
        _key.clear();
        _host.clear();
        _passwd.clear();
        _list.clear();
        _keyPattern.clear();
        _tableName.clear();
        _respResult.init();
    }

    TaskMsg &operator=(const TaskMsg &rhs)
    {
        if (this == &rhs)
            return *this;

        this->_port = rhs._port;
        this->_taskid = rhs._taskid;
        this->_dbIndex = rhs._dbIndex;
        this->_sequence = rhs._sequence;
        this->_clientIndex = rhs._clientIndex;
        this->_key = rhs._key;
        this->_type = rhs._type;
        this->_host = rhs._host;
        this->_passwd = rhs._passwd;
        this->_keyPattern = rhs._keyPattern;
        this->_list.clear();
        this->_list = rhs._list;
        this->_respResult = rhs._respResult;
        this->_tableName = rhs._tableName;
        this->_clusterMode = rhs._clusterMode;
        this->_customMode = rhs._customMode;
        return *this;
    }

    bool _clusterMode;
    bool _customMode;
    quint16 _port;
    int _taskid;
    int _sequence;
    int _clientIndex;
    int _dbIndex;
    int _type;
    QString _key;
    QString _host;
    QString _passwd;
    QString _keyPattern;
    QString _tableName;
    QList<QByteArray> _list;
    RespType _respResult;
};

class CmdMsg {
public:
    CmdMsg() {
        init();
    }

public:
    void init() {
        _operate = 0;
        _valueIndex = 0;
        _dbIndex = 0;
        _clientIndex = 0;
        _score = 0;
        _type = KEY_NONE;
        _key.clear();
        _value.clear();
        _filed.clear();
    }

    CmdMsg &operator=(const CmdMsg &rhs)
    {
        if (this == &rhs)
            return *this;

        this->_type = rhs._type;
        this->_score = rhs._score;
        this->_operate = rhs._operate;
        this->_key = rhs._key;
        this->_value = rhs._value;
        this->_filed = rhs._filed;
        this->_valueIndex = rhs._valueIndex;
        this->_dbIndex = rhs._dbIndex;
        this->_clientIndex = rhs._clientIndex;

        return *this;
    }

    bool operator == (const CmdMsg& msg)
    {
        return  this->_type == msg._type &&
                this->_key == msg._key &&
                this->_filed == msg._filed &&
                this->_valueIndex == msg._valueIndex &&
                this->_score == msg._score;
    }

    bool operator < (const CmdMsg& msg)
    {
        if (this->_type < msg._type) {
            return true;
        } else if(this->_type > msg._type) {
            return false;
        }

        if (this->_key < msg._key) {
            return true;
        } else if(this->_key > msg._key) {
            return false;
        }

        if (this->_filed < msg._filed) {
            return true;
        } else if(this->_filed > msg._filed) {
            return false;
        }

        if (this->_valueIndex < msg._valueIndex) {
            return true;
        } else if(this->_valueIndex > msg._valueIndex) {
            return false;
        }

        if (this->_score < msg._score) {
            return true;
        } else if(this->_score > msg._score) {
            return false;
        }

        return false;
    }

    bool operator <=(const CmdMsg& msg)
    {
        return (*this<msg) || (*this == msg);
    }

    bool operator >(const CmdMsg& msg)
    {
        return !(*this <= msg);
    }

    bool operator >=(const CmdMsg& msg)
    {
        return !(*this < msg);
    }

    int _operate;  //1、2、3增删改
    int _dbIndex;
    int _clientIndex;
    int _type;
    double _score;
    qlonglong _valueIndex;
    QString _key;
    QString _filed;
    QString _value;
};

class InitValueMsg
{
public:
    InitValueMsg() {
        init();
    }

public:
    bool operator == (const InitValueMsg& d)
    {
        return this->_key == d._key &&
                this->_dbindex == d._dbindex &&
                this->_clientIndex == d._clientIndex &&
                this->_valuePattern == d._valuePattern &&
                this->_type == d._type;
    }

    void init() {
        _dbindex = -1;
        _clientIndex = -1;
        _key.clear();
        _type = KEY_NONE;
        _valuePattern.clear();
    }

    int _dbindex;
    int _clientIndex;
    int _type;
    QString _key;
    QString _valuePattern;
};

typedef struct DbCfg {
    QString userName;
    QString password;
    QByteArray encodePasswd;
    QString port;
    QString hostname;
    QByteArray encodeHostname;
    QString database;
    QString tagname;
} DbCfg;

typedef struct ImpExpData
{
    int32_t iState;
    qlonglong lWeight;
    qlonglong lTimeOut;
    QString sKey;
    QString sKeyType;
    QString sFiled;
    QString sValue;
    QString sExpDate;

} ImpExpData;


Q_DECLARE_METATYPE(InitValueMsg)
Q_DECLARE_METATYPE(TaskMsg)
Q_DECLARE_METATYPE(CmdMsg)

#endif // DEFINE_H
