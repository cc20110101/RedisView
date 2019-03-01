/**
* @file      redisdefine.h
* @brief     REDIS定义类
* @author    wangcc3
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef REDISDEFINE_H
#define REDISDEFINE_H

#include <QString>
#include <QTime>
#include <QTimer>
#include <QObject>
#include <QVariant>
#include <QTextCodec>
#include <QStringList>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QHostAddress>
#include <QTextStream>
#include <QByteArray>
#include <QTcpSocket>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QList>

class RedisClient;

class RespType {
public:

    RespType() {
        init();
    }

    void init() {
        _integerValue = 0;
        _arrayLength = 0;
        _formatLength = 0;
        _formatType = '\0';
        _arrayValue.clear();
        _stringValue.clear();
    }

    // 不重载实际也可以用=
    RespType &operator=(const RespType &rhs)
    {
        // 若是本对象本身,则直接返回
        if (this == &rhs)
            return *this;

        // 复制等号右边的成员到左边的对象中
        this->_formatType = rhs._formatType;
        this->_arrayLength = rhs._arrayLength;
        this->_formatLength = rhs._formatLength;
        this->_integerValue = rhs._integerValue;
        this->_stringValue = rhs._stringValue;
        this->_arrayValue.clear();
        this->_arrayValue = rhs._arrayValue;

        // 把等号左边的对象再次传出,支持连等
        return *this;
    }

    char _formatType; // RESP类型
    int _arrayLength; // RESP数组元素数
    int _formatLength; // 大字符串类型长度
    qlonglong _integerValue; // 整数值
    QByteArray _stringValue; // 字符串值
    QList<RespType> _arrayValue; // RESP数组值
};

Q_DECLARE_METATYPE(RespType)

class ClusterClient
{
public:

    ClusterClient() {
        init();
    }

    void init() {
        _master = false;
        _port = 0;
        _startSlot = 0;
        _endSlot = 0;
        _slotNum = 0;
        _client = nullptr;
        _host.clear();
        _passwd.clear();
        _nodeId.clear();
        _masterId.clear();
    }

    ClusterClient &operator=(const ClusterClient &rhs)
    {
        if (this == &rhs)
            return *this;

        this->_master = rhs._master;
        this->_port = rhs._port;
        this->_startSlot = rhs._startSlot;
        this->_endSlot = rhs._endSlot;
        this->_slotNum = rhs._slotNum;
        this->_client = rhs._client;
        this->_host = rhs._host;
        this->_passwd = rhs._passwd;
        this->_nodeId = rhs._nodeId;
        this->_masterId = rhs._masterId;

        return *this;
    }

    bool _master;
    quint16 _port;
    unsigned int _startSlot;
    unsigned int _endSlot;
    unsigned int _slotNum;
    RedisClient * _client;
    QString _host;
    QString _passwd;
    QString _nodeId;
    QString _masterId;
};

class ClusterCmdRoute
{
public:

    ClusterCmdRoute() {
        init();
    }

    void init() {
        _key.clear();
        _value.clear();
        _cmd.clear();
    }

    ClusterCmdRoute &operator=(const ClusterCmdRoute &rhs)
    {
        if (this == &rhs)
            return *this;

        this->_cmd = rhs._cmd;
        this->_key = rhs._key;
        this->_value = rhs._value;

        return *this;
    }

    QByteArray _cmd;
    QByteArray _key;
    QByteArray _value;
};

#endif // REDISDEFINE_H
