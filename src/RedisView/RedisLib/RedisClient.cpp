/**
* @file      RedisClient.cpp
* @brief     REDIS客户端类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#include "RedisLib/RedisClient.h"

RedisClient::RedisClient(QObject *parent)
    : QObject(parent) {
    initClient();
}

RedisClient::RedisClient(const QString &hostAddress, int port, QObject *parent)
    : QObject(parent) {
    initClient();
    setHostAddress(hostAddress);
    setPort(port);
}

void RedisClient::initClient() {
    _iTimeOut = 3000; //默认3秒
    _qTimer.setInterval(_iTimeOut);
    _qTimer.setSingleShot(true);
    _bTimeout = false;
    _bSubModel = false;
    _sErrorInfo.clear();

    connect(&_qTimer, &QTimer::timeout, [this]()
    {
        if(_iTimeOut > 0) {
            _bTimeout = true;
            _qEventloop.quit();
        }
    });

    _redisTransMgr = new RedisTransMgr(this);
    connect(_redisTransMgr, &RedisTransMgr::sigReply, [this](const QByteArray &msg, const int &length)
    {
        _msg = msg;
        _msgLength = length;

        if(_bSubModel) {
            _respMsg.init();
            if(parseResp(_msg, _respMsg)) {
                emit sigMessage(_respMsg);
            }
        } else {
            _qEventloop.quit();
        }
    });

    connect(_redisTransMgr, &RedisTransMgr::sigError, [this](const QString &sError)
    {
        _sErrorInfo = sError;
        emit sigError(sError);
    });

    connect(_redisTransMgr, &RedisTransMgr::sigConnected, [this]()
    {
        emit sigConnected();
    });

    connect(_redisTransMgr, &RedisTransMgr::sigDisconnected, [this]()
    {
        emit sigDisconnected();
    });
}

QString RedisClient::getHostAddress() {
    return _redisTransMgr->getHostAddress();
}

void RedisClient::setHostAddress(const QString &hostAddress) {
    Q_ASSERT(_redisTransMgr != NULL);
    _redisTransMgr->setHostAddress(hostAddress);
}

void RedisClient::setPort(int port) {
    Q_ASSERT(_redisTransMgr != NULL);
    _redisTransMgr->setPort(port);
}

int RedisClient::getPort() {
    return _redisTransMgr->getPort();
}

bool RedisClient::isOpen() {
    return _redisTransMgr->isOpen();
}

bool RedisClient::open(int timeOut) {
    return _redisTransMgr->open(timeOut);
}

bool RedisClient::open(const QString &hostAddress,quint16 port,int timeOut) {
    return _redisTransMgr->open(hostAddress, port, timeOut);
}

void RedisClient::close(){
    _redisTransMgr->close();
}

/**
 * 设置执行命令等待结果超时时间
 * @param[in]    iTimeOutMs 毫秒超时时间，0阻塞
 * @return
 * @see
 * @note
 */
void RedisClient::setTimeOutMs(int iTimeOutMs) {
    _iTimeOut = iTimeOutMs;
    _qTimer.setInterval(_iTimeOut);
}

/**
 * 获取执行命令等待结果超时时间
 * @return    毫秒超时时间
 * @see
 * @note
 */
int RedisClient::getTimeOutMs() {
    return _iTimeOut;
}

/**
 * 发送REDIS命令到服务端
 * @param[in]    str 命令
 * @return       服务端RESP协议返回值
 * @see
 * @note
 */
QByteArray RedisClient::command(const QString & str) {
    if(str.trimmed().isEmpty()) {
        return "-str is empty\r\n";
    }

    _msg.clear();
    _sErrorInfo.clear();
    _sendData.clear();
    _bTimeout = false;
    packRespCmd(str,_sendData);
    if(_sendData.isEmpty()) {
        return "-pack resp cmd is empty\r\n";
    }

    _redisTransMgr->write(_sendData);

    if(!_qTimer.isActive())
        _qTimer.start();
    _qEventloop.exec();
    if(_qTimer.isActive())
        _qTimer.stop();

    if (_bTimeout) {
        _sErrorInfo = "-run " + str + " timeout\r\n";
        return QByteArray(_sErrorInfo.toUtf8());
    }

    return _msg;
}

/**
 * 发送REDIS命令到服务端
 * @param[in]    list 命令
 * @return       服务端RESP协议返回值
 * @see
 * @note
 */
QByteArray RedisClient::command(const QList<QString> &list) {
    if(list.isEmpty()) {
        return "-cmd list is empty\r\n";
    }

    _msg.clear();
    _sErrorInfo.clear();
    _sendData.clear();
    _bTimeout = false;
    packRespCmd(list,_sendData);

    if(_sendData.isEmpty()) {
        return "-pack resp cmd is empty\r\n";
    }

    _redisTransMgr->write(_sendData);

    if(!_qTimer.isActive())
        _qTimer.start();
    _qEventloop.exec();
    if(_qTimer.isActive())
        _qTimer.stop();

    if (_bTimeout) {
        return "-run cmd timeout\r\n";
    }

    return _msg;
}

/**
 * 发送REDIS命令到服务端
 * @param[in]    list 命令
 * @return       执行是否成功
 * @see
 * @note
 */
bool RedisClient::command(const QList<QString> &list, QByteArray &result) {
    result = command(list);
    if(result[0] == '-') {
        if(parseRespError(result,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        return false;
    }
    return true;
}

/**
 * 解析命令为REDIS协议格式
 * @param[in]    str 命令
 * @return       命令RESP协议格式
 * @see
 * @note
 */
QByteArray RedisClient::getRespCmd(const QString &str) {
    _sendData.clear();
    packRespCmd(str,_sendData);
    return _sendData;
}

/**
 * 判断是否是集群模式
 * @param[out]    value 是否集群模式
 * @return        成功是ture，否false
 * @see
 * @note
 */
bool RedisClient::isCluster(bool &value) { 
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("info Cluster");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        QByteArray sValue;
        if(parseRespBulkString(_cmdResult,sValue, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                value = sValue.contains("cluster_enabled:1");
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取复制集参数信息
 * @param[out]    infoMap复制集信息
 * @return        成功是ture，否false
 * @see
 * @note
 */
bool RedisClient::getReplicationInfo(QMap<QString,QString> &infoMap) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("info Replication");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        QString sValue;
        if(parseRespBulkString(_cmdResult,sValue, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                int index = -1;
                QStringList infoList = sValue.split("\r\n",QString::SkipEmptyParts);
                for(int i = 0; i < infoList.size(); ++i) {
                    index = infoList[i].indexOf(":");
                    if(index == -1) {
                        infoMap[infoList[i]] = "nil";
                        continue;
                    }
                    infoMap[infoList[i].mid(0, index)] = infoList[i].mid(index + 1);
                }
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取Redis信息
 * @param[out]    infoMap信息
 * @return        成功是ture，否false
 * @see
 * @note
 */
bool RedisClient::getRedisInfo(QMap<QString,QString> &infoMap) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("info");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        QString sValue;
        if(parseRespBulkString(_cmdResult,sValue, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                int index = -1;
                QStringList infoList = sValue.split("\r\n",QString::SkipEmptyParts);
                for(int i = 0; i < infoList.size(); ++i) {
                    index = infoList[i].indexOf(":");
                    if(index == -1) {
                        infoMap[infoList[i]] = "nil";
                        continue;
                    }
                    infoMap[infoList[i].mid(0, index)] = infoList[i].mid(index + 1);
                }
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 判断是否是集群模式下主节点
 * @param[out]    value 是否集群主
 * @return       是ture，否false
 * @see
 * @note
 */
bool RedisClient::isMaster(bool &value) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("info Replication");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        QByteArray sValue;
        if(parseRespBulkString(_cmdResult,sValue, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                value = sValue.contains("role:master");
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取集群槽信息
 * @param[out]    value 集群槽信息
 * @return       是ture，否false
 * @see
 * @note
 */
bool RedisClient::getClusterSlots(RespType &value) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("CLUSTER SLOTS");

    _cmdResult = command(_cmd);
    if(parseResp(_cmdResult,value)) {
        if(value._formatType == '-') {
            _sErrorInfo = value._stringValue;
            _bRet = false;
        } else {
            _bRet = true;
        }
    } else {
        _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    }

    return _bRet;
}

/**
 * 获取集群模式节点信息
 * @param[out]    value 集群节点信息
 * @return       是ture，否false
 * @see
 * @note
 */
bool RedisClient::getClusterNodes(QByteArray & value) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("CLUSTER NODES");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet <= 0) {
                _bRet = false;
                _sErrorInfo = "value is nil or empty";
            } else {
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将信息message发送到指定的频道channel
 * @param[in]    channel 频道
 * @param[in]    message 消息
 * @param[out]   llRet   接收到信息 message 的订阅者数量
 * @return       true成功,false失败
 * @see
 * @note
 */
bool RedisClient::publish(const QString & channel, const QString & message, qlonglong & llRet) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("PUBLISH");
    _vCmdList.push_back(channel);
    _vCmdList.push_back(message);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 是一个查看订阅与发布系统状态的内省命令
 * @param[in]     subcommand 子命令
 * @param[in]     argument   子命令参数
 * @param[out]    value      返回值
 * @return        true成功,false失败
 * @see
 * @note
 */
bool RedisClient::pubsub(const QString &subcommand, const QString &argument, RespType &value) {
    if(subcommand.trimmed().isEmpty()) {
        _sErrorInfo = "subcommand is empty";
        return false;
    }

    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("PUBSUB");
    _vCmdList.push_back(subcommand);
    if(!argument.isEmpty())
        _vCmdList.push_back(argument);

    _cmdResult = command(_vCmdList);
    if(parseResp(_cmdResult,value)) {
        if(value._formatType == '-') {
            _sErrorInfo = value._stringValue;
            _bRet = false;
        } else {
            _bRet = true;
        }
    } else {
        _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    }

    return _bRet;
}

/**
 * 订阅一个或多个频道
 * @param[in]    pattern 一个或多个频道
 * @return
 * @see
 * @note
 */
void RedisClient::subscribe(const QString & channel) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("SUBSCRIBE");
    _vCmdList.push_back(channel);

    _sendData.clear();
    packRespCmd(_vCmdList,_sendData);
    _redisTransMgr->write(_sendData);
}

/**
 * 指示客户端退订给定的频道,如果没有频道被指定，也即是，一个无参数的UNSUBSCRIBE调用被执行，
 * 那么客户端使用SUBSCRIBE命令订阅的所有频道都会被退订。在这种情况下，命令会返回一个信息，
 * 告知客户端所有被退订的频道
 * @param[in]    channel 一个或多个频道
 * @return
 * @see
 * @note
 */
void RedisClient::unsubscribe(const QString & channel) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("UNSUBSCRIBE");
    _vCmdList.push_back(channel);

    _sendData.clear();
    packRespCmd(_vCmdList,_sendData);
    _redisTransMgr->write(_sendData);
}

/**
 * 订阅一个或多个符合给定模式的频道。每个模式以*作为匹配符，比如it*匹配所有以it开头的频道
 * @param[in]    pattern 一个或多个模式
 * @return
 * @see
 * @note
 */
void RedisClient::psubscribe(const QString & pattern) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("PSUBSCRIBE");
    _vCmdList.push_back(pattern);

    _sendData.clear();
    packRespCmd(_vCmdList,_sendData);
    _redisTransMgr->write(_sendData);
}

/**
 * 指示客户端退订所有给定模式，如果没有模式被指定，也即是，一个无参数的PUNSUBSCRIBE调用被执行，
 * 那么客户端使用 PSUBSCRIBE 命令订阅的所有模式都会被退订。在这种情况下，命令会返回一个信息，
 * 告知客户端所有被退订的模式
 * @param[in]     pattern 一个或多个模式
 * @return
 * @see
 * @note
 */
void RedisClient::punsubscribe(const QString & pattern) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("PUNSUBSCRIBE");
    _vCmdList.push_back(pattern);

    _sendData.clear();
    packRespCmd(_vCmdList,_sendData);
    _redisTransMgr->write(_sendData);
}

/**
 * 返回上次错误信息
 * @return    上次错误信息
 * @see
 * @note
 */
QString RedisClient::getErrorInfo() {
    return _sErrorInfo;
}

/**
 * 如果AUTH命令给定的密码password和配置文件中的密码相符的话，服务器会返回OK并开始接受命令输入
 * @param[in]    password 认证密码
 * @return       成功ture，失败false
 * @see
 * @note
 */
bool RedisClient::auth(const QString & password) {
    _sErrorInfo.clear();
    if(password.isEmpty()) {
        _sErrorInfo = "password is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("AUTH");
    _vCmdList.push_back(password);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 如果key已经存在并且是一个字符串，APPEND命令将value追加到key原来的值的末尾
 * 如果key不存在，APPEND就简单地将给定key设为value，就像执行SET key value一样
 * @param[in]    key 一个或多个键
 * @param[in]    value 追加的值
 * @return       追加value之后，key中字符串的长度
 * @see
 * @note
 */
qlonglong RedisClient::append(const QString & key, const QString & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("APPEND");
    _vCmdList.push_back("key");
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _llRet = 0;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _llRet = 0;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _llRet;
}

/**
 * 返回当前数据库的key的数量
 * @param[in]    key 一个或多个键
 * @param[out]   llRet 键数量
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::dbsize(qlonglong & llRet) {
    _sErrorInfo.clear();

    _cmd.clear();
    _cmd.append("DBSIZE");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 删除给定的一个key，不存在的key会被忽略
 * @param[in]    key 一个键
 * @param[out]   llRet 删除数量
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::del(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("DEL");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将key中储存的数字值减一
 * 如果key不存在，那么key的值会先被初始化为0，然后再执行DECR操作
 * 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误
 * @param[in]    key 键
 * @param[out]   llRet 减后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::decr(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("DECR");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将key所储存的值减去interval
 * 如果key不存在，那么key的值会先被初始化为0，然后再执行DECRBY命令
 * 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误
 * @param[in]    key 键
 * @param[in]    interval 减值
 * @param[out]   llRet 减后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::decrby(const QString & key, int interval, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("DECRBY");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(interval));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将key中储存的数字值增一
 * 如果key不存在，那么key的值会先被初始化为0，然后再执行INCR操作
 * 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误
 * @param[in]    key 键
 * @param[out]   llRet 加后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::incr(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("INCR");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将key所储存的值加上增量increment
 * 如果key不存在，那么key的值会先被初始化为0，然后再执行INCRBY命令
 * 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误
 * @param[in]    key 键
 * @param[in]    interval 加值
 * @param[out]   llRet 加后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::incrby(const QString & key, int interval, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("INCRBY");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(interval));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为哈希表key中的域field的值加上增量increment
 * 增量也可以为负数，相当于对给定域进行减法操作
 * 如果key不存在，一个新的哈希表被创建并执行HINCRBY命令
 * 如果域field不存在，那么在执行命令前，域的值被初始化为0
 * 对一个储存字符串值的域field执行HINCRBY命令将造成一个错误。
 * @param[in]    key 键
 * @param[in]    field 字段
 * @param[in]    increment 加值
 * @param[out]   llRet 加后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::hincrby(const QString & key, const QString & field, int increment, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HINCRBY");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);
    _vCmdList.push_back(QString::number(increment));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为哈希表key中的域field加上浮点数增量increment
 * 如果哈希表中没有域field，那么HINCRBYFLOAT会先将域field的值设为0，然后再执行加法操作。
 * 如果键key不存在，那么HINCRBYFLOAT会先创建一个哈希表，再创建域field，最后再执行加法操作。
 * @param[in]    key 键
 * @param[in]    field 字段
 * @param[in]    increment 加值
 * @param[out]   dRet 加后值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::hincrbyfloat(const QString & key, const QString & field, double increment, double & dRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HINCRBYFLOAT");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);
    _vCmdList.push_back(QString::number(increment));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,_sValue, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                dRet = _sValue.toDouble();
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 检查给定key是否存在
 * @param[in]    key 键
 * @return       存在true,不存在false
 * @see
 * @note
 */
bool RedisClient::exists(const QString & key) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("EXISTS");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet)
            _bRet = true;
        else
            _bRet = false;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 设置字符串key的键值
 * @param[in]    key 键
 * @param[in]    value 值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::set(const QString & key, const QString & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SET");
    _vCmdList.push_back(key);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回key所关联的字符串值。如果key不存在那么返回特殊值nil
 * @param[in]    key 键
 * @param[out]   value 值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::get(const QString & key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("GET");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为key设置生存时间，命令接受的时间参数秒
 * 当key不存在或没办法设置生存时间，返回0
 * @param[in]    key 键
 * @param[in]    seconds 生存秒
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::expire(const QString & key, uint seconds) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("EXPIRE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(seconds));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet)
            _bRet = true;
        else
            _bRet = false;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为key设置生存时间，命令接受的时间参数毫秒
 * 当key不存在或没办法设置生存时间，返回0
 * @param[in]    key 键
 * @param[in]    mseconds 生存豪秒
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::pexpire(const QString & key, uint mseconds) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("PEXPIRE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(mseconds));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet)
            _bRet = true;
        else
            _bRet = false;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为key设置生存时间，毫秒为单位设置key的过期unix时间戳
 * 当key不存在或没办法设置生存时间，返回0
 * @param[in]    key 键
 * @param[in]    mstimestamp UNIX毫秒时间戳
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::pexpireat(const QString & key, uint mstimestamp) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("PEXPIREAT");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(mstimestamp));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet)
            _bRet = true;
        else
            _bRet = false;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 为key设置生存时间，命令接受的时间参数是UNIX时间戳，如果生存时间设置成功，返回1
 * 当key不存在或没办法设置生存时间，返回0
 * @param[in]    key 键
 * @param[in]    timestamp UNIX秒时间戳
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::expireat(const QString & key, uint timestamp) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("EXPIREAT");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(timestamp));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet)
            _bRet = true;
        else
            _bRet = false;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 以秒为单位返回key的剩余生存时间
 * @param[in]    key 键
 * @param[out]   seconds 当key不存在时，为-2。当key存在但没有设置剩余生存时间时，
 *              值-1，否则值为key的剩余生存时间
 * @return      设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::ttl(const QString & key, qlonglong & seconds) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("TTL");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,seconds)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 以毫秒为单位返回key的剩余生存时间
 * @param[in]    key 键
 * @param[out]   mseconds 当key不存在时，为-2。当key存在但没有设置剩余生存时间时，
 *              值-1，否则值为key的剩余生存时间
 * @return      设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::pttl(const QString & key, qlonglong & mseconds) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("PTTL");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,mseconds)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else
            _bRet = true;
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

int RedisClient::GetDataType(QByteArray &type) {
    if ("string" == type)
        return 1;
    else if ("list" == type)
        return 2;
    else if ("set" == type)
        return 3;
    else if ("zset" == type)
        return 4;
    else if ("hash" == type)
        return 5;
    else if ("none" == type)
        return 0;
    else
        return 0;
}

/**
 * 返回key所储存的值的类型
 * @param[in]    key 键
 * @param[out]   value 键类型，none，string，list，set，zset，hash
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::type(const QString & key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("TYPE");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回key所储存的值的类型
 * @param[in]    key 键
 * @param[out]   value 键类型，none 0，string 1，list 2，set 3，zset 4，hash 5
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::type(const QString& key, int & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    if(type(key,_sValue)) {
        value = GetDataType(_sValue);
        _bRet = true;
    } else {
        _bRet = 0;
    }
    return _bRet;
}

/**
 * 序列化给定key，并返回被序列化的值
 * @param[in]    key 字符串键
 * @param[out]   value 序列化后值
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::dump(const QString & key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("DUMP");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回key中字符串值的子字符串，字符串的截取范围由start和stop两个偏移量决定(包括start和stop在内)。
 * 负数偏移量表示从字符串最后开始计数， -1表示最后一个字符， -2表示倒数第二个，以此类推。
 * @param[in]    key 字符串键
 * @param[in]    start 字符串开始位置
 * @param[in]    stop 字符串结束位置
 * @param[out]   value 子字符串值
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::getrange(const QString & key, int start, int stop, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("GETRANGE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(start));
    _vCmdList.push_back(QString::number(stop));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将哈希表key中的域field的值设为value 。
 * 如果key不存在，一个新的哈希表被创建并进行HSET操作。
 * 如果域field已经存在于哈希表中，旧值将被覆盖。
 * @param[in]    key 哈希键
 * @param[in]    field 哈希字段
 * @param[in]    value 哈希字段值
 * @param[out]   llRet 如果field是哈希表中的一个新建域，并且值设置成功，值为1。
 *               如果哈希表中域field已经存在且旧值已被新值覆盖，值为0。
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::hset(const QString & key, const QString & field, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HSET");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将哈希表key中的域field的值设置为value，当且仅当域field不存在。
 * 若域field 已经存在，该操作无效。
 * 如果 key 不存在，一个新哈希表被创建并执行 HSETNX 命令。
 * @param[in]    key 哈希键
 * @param[in]    field 哈希字段
 * @param[in]    value 哈希字段值
 * @return       设置成功rue,设置失败false
 * @see
 * @note
 */
bool RedisClient::hsetnx(const QString & key, const QString & field, const QString & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HSETNX");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(!parseRespInteger(_cmdResult,_llRet)) {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        } else if(_llRet > 0) {
            _bRet = true;
        } else {
            _sErrorInfo = "hash filed have exist";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 删除哈希表key中的一个或多个指定域，不存在的域将被忽略
 * @param[in]    key 哈希键
 * @param[in]    field 哈希字段
 * @param[out]   llRet 被成功移除的域的数量，不包括被忽略的域
 * @return       存在rue,不存在false
 * @see
 * @note
 */
bool RedisClient::hdel(const QString & key, const QString & field, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HDEL");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 查看哈希表key中，给定域field是否存在
 * @param[in]    key 哈希键
 * @param[in]    field 哈希字段
 * @return       存在rue,不存在false
 * @see
 * @note
 */
bool RedisClient::hexists(const QString & key, const QString & field) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HEXISTS");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet > 0)
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表key中给定域field的值。
 * @param[in]    key 哈希键
 * @param[in]    field 哈希字段
 * @param[in]    value 哈希字段值
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hget(const QString & key, const QString & field, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HGET");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表key中域的数量。当 key 不存在时，返回 0 。
 * @param[in]    key 哈希键值
 * @param[out]   llRet 哈希域长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hlen(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HLEN");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 同时将多个field-value(域-值)对设置到哈希表key中。
 * 此命令会覆盖哈希表中已存在的域。
 * 如果key不存在，一个空哈希表被创建并执行HMSET操作。
 * @param[in]    key 哈希键值
 * @param[in]    map 插入哈希字段与值
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hmset(const QString & key, QMap<QString, QVariant> & map) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HMSET");
    _vCmdList.push_back(key);
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext())
    {
        i.next();
        _vCmdList.push_back(i.key());
        _vCmdList.push_back(i.value().toString());
    }

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回列表key中，下标为index的元素
 * 下标index参数start和stop都以0为底，也就是说，以0表示列表的第一个元素，以1表示列表的第二个元素，以此类推。
 * 也可以使用负数下标，以-1表示列表的最后一个元素，-2表示列表的倒数第二个元素，以此类推。
 * 如果 key 不是列表类型，返回一个错误。
 * @param[in]    key 链表键值
 * @param[in]    index 链表索引值
 * @param[out]    value 链表键值
 * @return       成功true,失败false
 * @see
 * @note         如果index参数的值不在列表的区间范围内(out of range)，返回 nil 。
 */
bool RedisClient::lindex(const QString & key, qlonglong index, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LINDEX");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(index));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将一个或多个值value插入到列表key的表头
 * @param[in]    key 链表键值
 * @param[in]    value 链表添加一个或多个键值
 * @param[out]   llRet 链表长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::lpush(const QString & key, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LPUSH");
    _vCmdList.push_back(key);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将一个或多个值value插入到列表key的表尾,如果key不存在，一个空列表会被创建并执行RPUSH操作
 * @param[in]    key 链表键值
 * @param[in]    value 链表添加一个或多个键值
 * @param[out]   llRet 链表长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::rpush(const QString & key, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RPUSH");
    _vCmdList.push_back(key);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将值value插入到列表key的表尾，当且仅当key存在并且是一个列表,
 * 当key不存在时，RPUSHX 命令什么也不做
 * @param[in]    key 链表键值
 * @param[in]    value 链表添加一个或多个键值
 * @param[out]   llRet 链表长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::rpushx(const QString & key, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RPUSHX");
    _vCmdList.push_back(key);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 当且仅当key存在并且是一个列表时,将值 value插入到列表 key的表头。
 * 和 LPUSH命令相反，当 key不存在时LPUSHX命令什么也不做。
 * @param[in]    key 链表键值
 * @param[in]    value 链表添加键值
 * @param[out]   llRet 链表长度
 * @return       成功true,失败false
 * @see
 * @note         key不存在时不做操作
 */
bool RedisClient::lpushx(const QString & key, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LPUSHX");
    _vCmdList.push_back(key);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 链表压入
 * @param[in]    key 链表键值
 * @param[out]   llRet 链表长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::llen(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LLEN");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 链表插入数据
 * @param[in]    key 链表键值
 * @param[in]    position 链表位置，0是BEFORE，1是AFTER
 * @param[in]    pivot 链表查找元素值
 * @param[in]    value 链表要插入元素值
 * @param[out]   llRet 没有pivot为-1，没有key或空链表为0，其他为列表长度
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::linsert(const QString & key, const int position, const QString & pivot, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LINSERT");
    _vCmdList.push_back(key);
    if(position)
        _vCmdList.push_back("AFTER");
    else
        _vCmdList.push_back("BEFORE");
    _vCmdList.push_back(pivot);
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除并返回列表key的头元素，当key不存在时，返回nil
 * @param[in]    key 链表键值
 * @param[out]   value 链表返回元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::lpop(const QString & key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LPOP");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取引用计数
 * @param[in]    key     键
 * @param[out]   value   结果
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::refcount(const QString& key, qlonglong & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("OBJECT");
    _vCmdList.push_back("REFCOUNT");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取空闲时间
 * @param[in]    key     键
 * @param[out]   value   空闲秒
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::idletime(const QString& key, qlonglong & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("OBJECT");
    _vCmdList.push_back("IDLETIME");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 获取编码
 * @param[in]    key     键
 * @param[out]   value   结果
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::encoding(const QString& key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("OBJECT");
    _vCmdList.push_back("ENCODING");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 它是LPOP命令的阻塞版本，当给定列表内没有任何元素可供弹出的时候，
 * 连接将被BLPOP命令阻塞，直到等待超时或发现可弹出元素为止
 * @param[in]    key 一个或多个链表键值，多个空格隔开
 * @param[out]   outkey 链表返回元素的key
 * @param[out]   value 链表返回元素
 * @param[in]    timeout 以秒为单位的超时参数，设为0表示阻塞时间可以无限期延长
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::blpop(const QString & key, QString & outkey, QByteArray & value, int timeout) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("BLPOP");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(timeout));

    unsigned int iOldTimeOut = getTimeOutMs();
    setTimeOutMs(0);
    _cmdResult = command(_vCmdList);
    setTimeOutMs(iOldTimeOut);

    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        _vListResult.clear();
        if(parseRespSingelArray(_cmdResult,_vListResult, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else if(_iRet >= 2){
                outkey = _vListResult[0];
                value = _vListResult[1];
                _bRet = true;
            } else {
                _bRet = false;
                _sErrorInfo = "unkown exception";
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 它是RPOP命令的阻塞版本，当给定列表内没有任何元素可供弹出的时候，
 * 连接将被BLPOP命令阻塞，直到等待超时或发现可弹出元素为止
 * @param[in]    key 一个或多个链表键值，多个空格隔开
 * @param[out]   outkey 链表返回元素的key
 * @param[out]   value 链表返回元素
 * @param[in]    timeout 以秒为单位的超时参数，设为0表示阻塞时间可以无限期延长
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::brpop(const QString & key, QString & outkey, QByteArray & value, int timeout) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("BRPOP");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(timeout));

    unsigned int iOldTimeOut = getTimeOutMs();
    setTimeOutMs(0);
    _cmdResult = command(_vCmdList);
    setTimeOutMs(iOldTimeOut);

    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        _vListResult.clear();
        if(parseRespSingelArray(_cmdResult,_vListResult, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else if(_iRet >= 2){
                outkey = _vListResult[0];
                value = _vListResult[1];
                _bRet = true;
            } else {
                _bRet = false;
                _sErrorInfo = "unkown exception";
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 根据参数count的值，移除列表中与参数value相等的元素
 * @param[in]    key 链表键值
 * @param[in]    count count>0:从表头开始向表尾搜索，移除与value相等的元素，数量为count
 *               count<0:从表尾开始向表头搜索，移除与value相等的元素，数量为count的绝对值
 *               count=0:移除表中所有与value相等的值
 * @param[in]    value 链表移除元素值
 * @param[out]   llRet 被移除元素的数量，不存在的key被视作空表，所以当key不存在时，为0
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::lrem(const QString & key, int count, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LREM");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(count));
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将列表key下标为index的元素的值设置为value
 * 当index参数超出范围，或对一个空列表(key不存在)进行LSET时，返回一个错误
 * @param[in]    key 链表键值
 * @param[in]    index 链表索引
 * @param[in]    value 链表值
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::lset(const QString & key, int index, const QString & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LSET");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(index));
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 让列表只保留指定区间内的元素，不在指定区间之内的元素都将被删除，保留包含前后索引
 * 下标(index)参数start和stop都以0为底，也就是说，以0表示列表的第一个元素，
 * 以1表示列表的第二个元素，以此类推。
 * 也可以使用负数下标，以-1表示列表的最后一个元素，-2表示列表的倒数第二个元素，以此类推。
 * @param[in]    key 链表键值
 * @param[in]    start 链表开始索引
 * @param[in]    stop 链表结束索引
 * @return       成功true,失败false
 * @see
 * @note        如果start下标比列表的最大下标end(LLEN list减去1)还要大，或者start>stop，
 *              LTRIM 返回一个空列表(因为LTRIM已经将整个列表清空)
 *              如果stop下标比end下标还要大，Redis将stop的值设置为end
 *
 */
bool RedisClient::ltrim(const QString & key, int start, int stop) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LTRIM");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(start));
    _vCmdList.push_back(QString::number(stop));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将当前数据库的key移动到给定的数据库db当中
 * 如果当前数据库(源数据库)和给定数据库(目标数据库)有相同名字的给定key，
 * 或者key不存在于当前数据库，那么MOVE没有任何效果
 * @param[in]    key 键值
 * @param[in]    database 目的数据库
 * @return       成功true,失败false
 * @see
 * @note
 *
 */
bool RedisClient::move(const QString & key, int database) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("MOVE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(database));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet)
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 同时设置一个或多个key-value对
 * 如果某个给定key已经存在，那么MSET会用新值覆盖原来的旧值
 * @param[in]    map 键值对
 * @return       成功true,失败false
 * @see
 * @note         MSET是一个原子性(atomic)操作，所有给定key都会在同一时间内被设置，
 *               某些给定key被更新而另一些给定key没有改变的情况，不可能发生
 *
 */
bool RedisClient::mset(QMap<QString, QVariant> & map) {
    _sErrorInfo.clear();
    _vCmdList.clear();
    _vCmdList.push_back("MSET");
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext())
    {
        i.next();
        _vCmdList.push_back(i.key());
        _vCmdList.push_back(i.value().toString());
    }

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除给定 key 的生存时间,如果key不存在或key没有设置生存时间，返回0
 * @param[in]    key 键
 * @return       设置成功true,设置失败false
 * @see
 * @note
 *
 */
bool RedisClient::persist(const QString & key) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("PERSIST");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet)
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将key改名为newkey，当key和newkey相同，或者key不存在时，返回一个错误。
 * 当newkey已经存在时，RENAME命令将覆盖旧值
 * @param[in]    key 键
 * @param[in]    newkey 新键
 * @return       设置成功true,设置失败false
 * @see
 * @note
 *
 */
bool RedisClient::rename(const QString & key, const QString & newkey) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }
    if(newkey.trimmed().isEmpty()) {
        _sErrorInfo = "newkey is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RENAME");
    _vCmdList.push_back(key);
    _vCmdList.push_back(newkey);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 当且仅当newkey不存在时，将key改名为newkey
 * 当key不存在时，返回一个错误
 * @param[in]    key 键
 * @param[in]    newkey 新键
 * @return       设置成功true,设置失败false
 * @see
 * @note
 *
 */
bool RedisClient::renamex(const QString & key, const QString & newkey) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }
    if(newkey.trimmed().isEmpty()) {
        _sErrorInfo = "newkey is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RENAMENX");
    _vCmdList.push_back(key);
    _vCmdList.push_back(newkey);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet)
                _bRet = true;
            else {
                _sErrorInfo = "new key have exist";
                _bRet = false;
            }
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除并返回列表key的尾元素，当key不存在时，返回nil
 * @param[in]    key 链表键值
 * @param[out]   value 链表返回元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::rpop(const QString & key, QByteArray & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RPOP");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 命令RPOPLPUSH在一个原子时间内，执行以下两个动作：
 * 将列表source中的最后一个元素(尾元素)弹出，并返回给客户端。
 * 将source弹出的元素插入到列表destination，作为destination列表的的头元素。
 * @param[in]    source 源链表
 * @param[in]    destination 目的链表
 * @param[out]   value 链表返回元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::rpoplpush(const QString & source, const QString & destination, QByteArray & value) {
    _sErrorInfo.clear();
    if(source.trimmed().isEmpty()) {
        _sErrorInfo = "source is empty";
        return false;
    }
    if(destination.trimmed().isEmpty()) {
        _sErrorInfo = "destination is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("RPOPLPUSH");
    _vCmdList.push_back(source);
    _vCmdList.push_back(destination);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * BRPOPLPUSH是RPOPLPUSH的阻塞版本,当列表source为空时，
 * BRPOPLPUSH命令将阻塞连接，直到等待超时，或有另一个客户端对
 * source执行LPUSH或RPUSH命令为止
 * 命令RPOPLPUSH在一个原子时间内，执行以下两个动作：
 * 将列表source中的最后一个元素(尾元素)弹出，并返回给客户端。
 * 将source弹出的元素插入到列表destination，作为destination列表的的头元素。
 * @param[in]    source 源链表
 * @param[in]    destination 目的链表
 * @param[in]    timeout 以秒为单位，0阻塞
 * @param[out]   value 链表返回元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::brpoplpush(const QString & source, const QString & destination,
                             QByteArray & value, int timeout) {
    _sErrorInfo.clear();
    if(source.trimmed().isEmpty()) {
        _sErrorInfo = "source is empty";
        return false;
    }
    if(destination.trimmed().isEmpty()) {
        _sErrorInfo = "destination is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("BRPOPLPUSH");
    _vCmdList.push_back(source);
    _vCmdList.push_back(destination);
    _vCmdList.push_back(QString::number(timeout));

    unsigned int iOldTimeOut = getTimeOutMs();
    setTimeOutMs(0);
    _cmdResult = command(_vCmdList);
    setTimeOutMs(iOldTimeOut);

    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        _vListResult.clear();
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else if(_cmdResult[0] == '*') {
        _vListResult.clear();
        if(parseRespSingelArray(_cmdResult,_vListResult, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else if(_llRet >= 1){
                value = _vListResult[0];
                _bRet = true;
            } else {
                _bRet = false;
                _sErrorInfo = "unkown exception";
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将一个member元素加入到有序集合key当中，已经存在于集合的member元素将被忽略
 * 假如key不存在，则创建一个只包含member元素作成员的集合
 * 当key不是集合类型时，返回一个错误
 * @param[in]    key 集合键
 * @param[in]    member 一个集合元素
 * @param[in]    score 集合分数
 * @param[out]   llRet 添加的个数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::zadd(const QString &key, const QString &member, const double &score, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("ZADD");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(score));
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将一个或多个member元素加入到集合key当中，已经存在于集合的member元素将被忽略
 * 假如key不存在，则创建一个只包含member元素作成员的集合
 * 当key不是集合类型时，返回一个错误
 * @param[in]    key 集合键
 * @param[in]    member 一个或多个添加的集合元素
 * @param[out]   llRet 添加的个数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sadd(const QString & key, const QString & member, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SADD");
    _vCmdList.push_back(key);
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回集合key的基数(集合中元素的数量),当key不存在时，返回0
 * @param[in]    key 集合键
 * @param[out]   llRet 集合基数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scard(const QString & key, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SCARD");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除并返回集合中的一个随机元素,当key不存在或key是空集时，返回nil
 * @param[in]    key 集合键
 * @param[out]   value 集合随机返回元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::spop(const QString & key, QByteArray &value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SPOP");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '$') {
        if(parseRespBulkString(_cmdResult,value, _iRet)) {
            if(_iRet == -1) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else
                _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将member元素从source集合移动到destination集合
 * @param[in]    source 源集合键
 * @param[in]    destination 目的集合键
 * @param[in]    member 集合元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::smove(const QString & source, const QString & destination, const QString & member) {
    _sErrorInfo.clear();
    if(source.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }
    if(destination.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SMOVE");
    _vCmdList.push_back(source);
    _vCmdList.push_back(destination);
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet)
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 判断member元素是否集合key的成员
 * @param[in]    key 集合键
 * @param[in]    member 集合元素
 * @return       是true,否false
 * @see
 * @note
 */
bool RedisClient::sismember(const QString & key, const QString & member) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SISMEMBER");
    _vCmdList.push_back(key);
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,_llRet)) {
            if(_llRet)
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 用value参数覆写(overwrite)给定key所储存的字符串值，从偏移量offset开始。
 * 不存在的 key 当作空白字符串处理。
 * 如果给定key原来储存的字符串长度比偏移量小，那么原字符和偏移量之间的空白将用零字节(zerobytes, "\x00" )来填充
 * @param[in]    key 键
 * @param[in]    offset 索引
 * @param[in]    value 字符串值
 * @param[out]   llRet修改之后，字符串的长度
 * @return       成功true,失败false
 * @see
 * @note         注意你能使用的最大偏移量是 2^29-1(536870911) ，因为Redis字符串的大小被限制在512兆以内
 */
bool RedisClient::setrange(const QString & key, int offset, const QString & value, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SETRANGE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(offset));
    _vCmdList.push_back(value);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将集合之间的差集存储在指定的destkey集合中，如果destkey集合已经存在，则将其覆盖。
 * @param[in]    destkey 目的集合键
 * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
 * @param[out]   llRet 结果集中的元素数量
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sdiffstore(const QString & destkey, const QString & keys, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(destkey.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "keys is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SDIFFSTORE");
    _vCmdList.push_back(destkey);
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将给定集合之间的交集存储在指定的destkey集合中。如果指定的destkey集合已经存在，则将其覆盖
 * @param[in]    destkey 目的集合键
 * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
 * @param[out]   llRet 结果集中的元素数量
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sinterstore(const QString & destkey, const QString & keys, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(destkey.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "keys is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SINTERSTORE");
    _vCmdList.push_back(destkey);
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将给定集合的并集存储在指定的集合destination中。如果destination已经存在，则将其覆盖。
 * @param[in]    destkey 目的集合键
 * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
 * @param[out]   llRet 结果集中的元素数量
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sunionstore(const QString & destkey, const QString & keys, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(destkey.trimmed().isEmpty()) {
        _sErrorInfo = "destkey is empty";
        return false;
    }
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "keys is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SUNIONSTORE");
    _vCmdList.push_back(destkey);
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除集合key中的一个或多个member元素，不存在的member元素会被忽略
 * @param[in]    key 集合键
 * @param[in]    member 移除的多个元素,多个可空格分割
 * @param[out]   llRet 被成功移除的元素的数量，不包括被忽略的元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::srem(const QString & key, const QString & member, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SREM");
    _vCmdList.push_back(key);
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除有序集合key中的一个或多个member元素，不存在的member元素会被忽略
 * @param[in]    key 集合键
 * @param[in]    member 移除的多个元素,多个可空格分割
 * @param[out]   llRet 被成功移除的元素的数量，不包括被忽略的元素
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::zrem(const QString &key, const QString &member, qlonglong & llRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("ZREM");
    _vCmdList.push_back(key);
    _vCmdList.push_back(member);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == ':') {
        if(parseRespInteger(_cmdResult,llRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp integer info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 清除所有Lua脚本缓存
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scriptflush() {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("SCRIPT FLUSH");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 杀死当前正在运行的Lua脚本，当且仅当这个脚本没有执行过任何写操作时，这个命令才生效。
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scriptkill() {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("SCRIPT KILL");

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 将脚本 script 添加到脚本缓存中，但并不立即执行这个脚本
 * 如果给定的脚本已经在缓存里面了，那么不做动作
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scriptload(const QString & script) {
    _sErrorInfo.clear();
    if(script.trimmed().isEmpty()) {
        _sErrorInfo = "script is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SCRIPT");
    _vCmdList.push_back("LOAD");
    _vCmdList.push_back(script);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 移除集合key中的一个或多个member元素，不存在的member元素会被忽略
 * @param[in]    host 目的主机
 * @param[in]    port 目的端口
 * @param[in]    key 迁移键
 * @param[in]    database 目的数据库
 * @param[in]    timeout 毫秒为格式，指定当前实例和目标实例进行沟通的最大间隔时间
 * @param[in]    copy 不移除源实例上的key
 * @param[in]    replcace 替换目标实例上已存在的key
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::migrate(const QString & host, int port, const QString & key,
                          int database, int timeout, bool copy, bool replcace) {
    _sErrorInfo.clear();
    if(host.trimmed().isEmpty()) {
        _sErrorInfo = "host is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("MIGRATE");
    _vCmdList.push_back(host);
    _vCmdList.push_back(QString::number(port));
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(database));
    _vCmdList.push_back(QString::number(timeout));
    if(copy) {
        _vCmdList.push_back("COPY");
    }
    if(replcace) {
        _vCmdList.push_back("REPLACE");
    }

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表key中，所有的域和值
 * @param[in]    key 哈希键
 * @param[out]   vKeyPairs 哈希域值对
 * @param[out]   llRet 哈希域数量
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hgetall(const QString & key, QMap<QString, QString> & vKeyPairs, int & iRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HGETALL");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        _vListResult.clear();
        if(parseRespSingelArray(_cmdResult,_vListResult, iRet)) {
            if(iRet <= 0) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else if(iRet >= 2){
                for (int i = 0; i < _vListResult.length(); ++i)
                {
                    vKeyPairs[_vListResult[i]] = _vListResult[i + 1];
                    i = i + 1;
                }
                iRet = iRet / 2;
                _bRet = true;
            } else {
                _bRet = false;
                _sErrorInfo = "unkown exception";
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表key中，一个或多个给定域的值。
 * 如果给定的域不存在于哈希表，那么返回一个nil值。
 * @param[in]    key 哈希键
 * @param[in]    field 一个或多个哈希域,多个空格分割
 * @param[out]   value 哈希域值
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hmget(const QString & key, const QString & field, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HMGET");
    _vCmdList.push_back(key);
    _vCmdList.push_back(field);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            if(_iRet <= 0) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表key中所有域的值。
 * @param[in]    key 哈希键
 * @param[out]   value 哈希域值
 * @param[out]   lRet 域个数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hvals(const QString & key, QList<QByteArray> & value, int & _iRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HVALS");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回一个或多个给定key值。
 * 如果给定的键不存在，那么返回一个nil值。
 * @param[in]    key 一个或多个键，空格分割
 * @param[out]   value 键值
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::mget(const QString & key, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("MGET");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            if(_iRet <= 0) {
                _bRet = false;
                _sErrorInfo = "value is nil";
            } else {
                _bRet = true;
            }
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回集合key中的所有成员。不存在的key被视为空集合。
 * @param[in]    key 集合键
 * @param[out]   value 集合元素值
 * @param[out]   llRet 集合个数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::smembers(const QString & key, QList<QByteArray> & value, int & iRet) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SMEMBERS");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 给定一个或多个脚本的SHA1校验和，返回一个包含0和1的列表，表示校验和所指定的脚本是否已经被保存在缓存当中
 * @param[in]    script 一个或多个SHA1校验和，空格分割
 * @param[out]   value 结果列表
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scriptexists(const QString & script, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(script.trimmed().isEmpty()) {
        _sErrorInfo = "script is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SCRIPT");
    _vCmdList.push_back("EXISTS");
    _vCmdList.push_back(script);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回哈希表 key 中的所有域
 * @param[in]    key 哈希键
 * @param[out]   value 结果列表
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hkeys(const QString & key, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HKEYS");
    _vCmdList.push_back(key);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 查找所有符合给定模式 pattern 的 key
 * @param[in]    pattern 匹配键模式
 * @param[out]   value 结果列表
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::keys(const QString & pattern, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(pattern.isEmpty()) {
        _sErrorInfo = "pattern is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("KEYS");
    _vCmdList.push_back(pattern);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp single array info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回列表key中指定区间内的元素，包括start和stop在内
 * 参数 start和stop都以0表示列表的第一个元素，以1表示列表的第二个元素，以此类推
 * 也可以使用负数下标，以-1表示列表的最后一个元素，-2表示列表的倒数第二个元素，以此类推
 * @param[in]    key 链表键
 * @param[in]    start 链表开始位置
 * @param[in]    stop 链表结束位置
 * @param[out]   value 链表值
 * @return       成功true,失败false
 * @see
 * @note        如果start下标比列表的最大下标end还要大，那么LRANGE返回一个空列表
 *              如果stop下标比end下标还要大，Redis将stop的值设置为end
 */
bool RedisClient::lrange(const QString & key, int start, int stop, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("LRANGE");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(start));
    _vCmdList.push_back(QString::number(stop));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回一个集合的全部成员，该集合是所有给定集合之间的差集
 * 不存在的key被视为空集
 * @param[in]    keys 一个或多个集合
 * @param[out]   value 集合差集
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sdiff(const QString & keys, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SDIFF");
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回一个集合的全部成员，该集合是所有给定集合之间的交集
 * 不存在的key被视为空集
 * @param[in]    keys 一个或多个集合
 * @param[out]   value 集合交集
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sinter(const QString & keys, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SINTER");
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回一个集合的全部成员，该集合是所有给定集合之间的并集
 * 不存在的key被视为空集
 * @param[in]    keys 一个或多个集合
 * @param[out]   value 集合并集
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sunion(const QString & keys, QList<QByteArray> & value) {
    _sErrorInfo.clear();
    if(keys.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SUNION");
    _vCmdList.push_back(keys);

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 只提供了key参数，那么返回集合中的一个随机元素
 * 如果count为正数，且小于集合基数，那么命令返回一个包含count个元素的数组，数组中的元素各不相同。
 * 如果count大于等于集合基数，那么返回整个集合
 * 如果count为负数，那么命令返回一个数组，数组中的元素可能会重复出现多次，而数组的长度为count的绝对值
 * @param[in]    keys 集合键
 * @param[in]    count 集合个数
 * @param[out]   value 结果集
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::srandmember(const QString & key, QList<QByteArray> & value, int count) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SRANDMEMBER");
    _vCmdList.push_back(key);
    if(count) {
        _vCmdList.push_back(QString::number(count));
    }

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else if(_cmdResult[0] == '$') {
        QByteArray sValue;
        if(parseRespBulkString(_cmdResult,sValue, _iRet)) {
            if(_iRet != -1) {
                value << sValue;
            }
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 返回或保存给定列表、集合、有序集合key中经过排序的元素
 * @param[in]    key 键
 * @param[in]    conditions 排序条件
 * @param[out]   value 结果集
 * @return       成功true,失败false
 * @see
 * @note         排序默认以数字作为对象，值被解释为双精度浮点数，然后进行比较
 */
bool RedisClient::sort(const QString & key, QList<QByteArray> & value, const QString & conditions) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SORT");
    _vCmdList.push_back(key);
    if(!conditions.isEmpty()) {
        _vCmdList.push_back(conditions);
    }

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseRespSingelArray(_cmdResult,value, _iRet)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 迭代当前数据库中的数据库键
 * @param[in]    pattern 键匹配模式
 * @param[out]   value   扫描结果
 * @param[in]    cursor  扫描游标
 * @param[in]    count   每次扫描返回数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::scan(const QString &pattern, RespType &value, qulonglong cursor, qulonglong count) {
    _sErrorInfo.clear();

    _vCmdList.clear();
    _vCmdList.push_back("SCAN");
    _vCmdList.push_back(QString::number(cursor));
    if(!pattern.isEmpty()) {
        _vCmdList.push_back("MATCH");
        _vCmdList.push_back(pattern);
    }
    _vCmdList.push_back("COUNT");
    _vCmdList.push_back(QString::number(count));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseResp(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 迭代希键中的键
 * @param[in]    key     哈希键
 * @param[in]    pattern 字段匹配模式
 * @param[out]   value   扫描结果
 * @param[in]    cursor  扫描游标
 * @param[in]    count   每次扫描返回数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::hscan(const QString& key, const QString &pattern, RespType &value, qulonglong cursor, qulonglong count) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("HSCAN");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(cursor));
    if(!pattern.isEmpty()) {
        _vCmdList.push_back("MATCH");
        _vCmdList.push_back(pattern);
    }
    _vCmdList.push_back("COUNT");
    _vCmdList.push_back(QString::number(count));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseResp(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 迭代集合中的键
 * @param[in]    key     集合键
 * @param[in]    pattern 值匹配模式
 * @param[out]   value   扫描结果
 * @param[in]    cursor  扫描游标
 * @param[in]    count   每次扫描返回数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::sscan(const QString& key, const QString &pattern, RespType &value, qulonglong cursor, qulonglong count) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("SSCAN");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(cursor));
    if(!pattern.isEmpty()) {
        _vCmdList.push_back("MATCH");
        _vCmdList.push_back(pattern);
    }
    _vCmdList.push_back("COUNT");
    _vCmdList.push_back(QString::number(count));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseResp(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 迭代有序集合中的键
 * @param[in]    key     有序集合键
 * @param[in]    pattern 值匹配模式
 * @param[out]   value   扫描结果
 * @param[in]    cursor  扫描游标
 * @param[in]    count   每次扫描返回数
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::zscan(const QString& key, const QString &pattern, RespType &value, qulonglong cursor, qulonglong count) {
    _sErrorInfo.clear();
    if(key.trimmed().isEmpty()) {
        _sErrorInfo = "key is empty";
        return false;
    }

    _vCmdList.clear();
    _vCmdList.push_back("ZSCAN");
    _vCmdList.push_back(key);
    _vCmdList.push_back(QString::number(cursor));
    if(!pattern.isEmpty()) {
        _vCmdList.push_back("MATCH");
        _vCmdList.push_back(pattern);
    }
    _vCmdList.push_back("COUNT");
    _vCmdList.push_back(QString::number(count));

    _cmdResult = command(_vCmdList);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseResp(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 迭代希键中的键
 * @param[in]    parameter     config get命令参数匹配模式
 * @param[out]   value         扫描结果
 * @return       成功true,失败false
 * @see
 * @note
 */
bool RedisClient::getcfg(const QString& parameter, RespType &value) {
    _sErrorInfo.clear();
    if(parameter.trimmed().isEmpty()) {
        _sErrorInfo = "parameter is empty";
        return false;
    }

    _cmd.clear();
    _cmd.append("CONFIG GET ");
    _cmd.append(parameter);

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '*') {
        if(parseResp(_cmdResult,value)) {
            _bRet = true;
        } else {
            _sErrorInfo = "parser resp bulk string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

/**
 * 选择db
 * @param[in]    index db索引
 * @return       成功rue,失败false
 * @see
 * @note
 */
bool RedisClient::select(const int &index) {
    _sErrorInfo.clear();
    _cmd.clear();
    _cmd.append("SELECT ");
    _cmd.append(QString::number(index));

    _cmdResult = command(_cmd);
    if(_cmdResult[0] == '-') {
        if(parseRespError(_cmdResult,_sValue))
            _sErrorInfo = _sValue;
        else
            _sErrorInfo = "parser resp error info failed";
        _bRet = false;
    } else if(_cmdResult[0] == '+') {
        if(parseRespString(_cmdResult,_sValue)) {
            if(_sValue == "OK")
                _bRet = true;
            else
                _bRet = false;
        } else {
            _sErrorInfo = "parser resp string info failed";
            _bRet = false;
        }
    } else {
        _bRet = false;
        _sErrorInfo = QString("parser resp type failed:") + _cmdResult[0];
    }
    return _bRet;
}

bool RedisClient::getSubModel() const
{
    return _bSubModel;
}

void RedisClient::setSubModel(bool bSubModel)
{
    _bSubModel = bSubModel;
}
