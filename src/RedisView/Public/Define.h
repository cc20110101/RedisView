#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QWidget>
#include <QTextBrowser>
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
#include "RedisLib/RedisCluster.h"

// 定义字符串
#define WindowTitle             "RedisView Community v1.6.1"
#define IniFileName             "conf.ini"

// 图标动画
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


#define THREAD_SCAN_KEY_TASK                       1
#define THREAD_SCAN_VALUE_TASK                     2
#define THREAD_COMMIT_VALUE_TASK                   3
#define THREAD_DEL_KEY_TASK                        4
#define THREAD_BATCH_DEL_KEY_TASK                  5

class Global {
public:
    Global();
    ~Global();

public:
    static QString gEncode;
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
        _name.clear();
        _addr.clear();
        _passwd.clear();
        _encode.clear();
    }

    QString _name;
    QString _addr;
    QString _passwd;
    QString _encode;
};

class TaskMsg {
public:
    TaskMsg() {
        init();
    }

public:
    void init() {
        _port = 0;
        _dbIndex = 0;
        _taskid = 0;
        _sequence = 0;
        _clientIndex = 0;
        _key.clear();
        _type.clear();
        _host.clear();
        _passwd.clear();
        _list.clear();
        _keyPattern.clear();
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

        return *this;
    }

    quint16 _port;
    int _taskid;
    int _sequence;
    int _clientIndex;
    int _dbIndex;
    QString _key;
    QByteArray _type;
    QString _host;
    QString _passwd;
    QString _keyPattern;
    QList<QByteArray> _list;
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
        _type.clear();
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
    double _score;
    qlonglong _valueIndex;
    QByteArray _type;
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
        _type.clear();
        _valuePattern.clear();
    }

    int _dbindex;
    int _clientIndex;
    QByteArray _type;
    QString _key;
    QString _valuePattern;
};

Q_DECLARE_METATYPE(InitValueMsg)
Q_DECLARE_METATYPE(TaskMsg)
Q_DECLARE_METATYPE(CmdMsg)

#endif // DEFINE_H
