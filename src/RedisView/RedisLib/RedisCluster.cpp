/**
* @file      RedisCluster.h
* @brief     REDIS集群客户端类
* @author    wangcc3
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#include "RedisLib/RedisCluster.h"

RedisCluster::RedisCluster() {
    _port = 0;
    _dbIndex = 0;
    _iClientIndex = -2;
    _isNewOpen = true;
    _onlyMaster = true;
    _isClusterMode = false;
    _isReplicationMode = false;
    _host.clear();
    _passwd.clear();
    _sErrorInfo.clear();
    _ConnectName.clear();
    _hostAddress.clear();
    _vClusterClients.clear();
    _vClusterMasterClients.clear();
    _redisClient = nullptr;
}

RedisCluster::~RedisCluster() {
    close();
}

unsigned int RedisCluster::getTimeOutMs() const {
    return _iTimeOut;
}

QString RedisCluster::getErrorInfo() const {
    return _sErrorInfo;
}

//不支持记录SCAN、HSCAN、BLPOP、BRPOP、BRPOPLPUSH、RPOPLPUSH、SDIFF、
//SDIFFSTORE、SINTER、SINTERSTORE、SMOVE、SUNION、SUNIONSTORE 、SSCAN、
//ZUNIONSTORE、ZINTERSTORE、ZSCAN
//下一个PSUBSCRIBE
QList<QByteArray> RedisCluster::clusterCommand(QString &str) {

    QString key = "";
    RedisClient * redisClient = nullptr;
    QList<QByteArray> result;
    QList<ClusterCmdRoute> cmd;
    result.clear();
    cmd.clear();

    str = str.trimmed();
    //str.replace(QRegExp("[\\s]+"), " ");

    if(str.isEmpty()) {
        result << "-command is empty\r\n";
        return result;
    }

    if(str.startsWith("randomkey",Qt::CaseInsensitive) ||
            str.startsWith("cluster",Qt::CaseInsensitive) ||
            str.startsWith("info",Qt::CaseInsensitive)) {
        key = getRandKey(); //randomkey
    } else if(str.startsWith("get",Qt::CaseInsensitive) ||
              str.startsWith("dump",Qt::CaseInsensitive) ||
              str.startsWith("exists",Qt::CaseInsensitive) ||
              str.startsWith("keys",Qt::CaseInsensitive) ||
              str.startsWith("persist",Qt::CaseInsensitive) ||
              str.startsWith("pttl",Qt::CaseInsensitive) ||
              str.startsWith("ttl",Qt::CaseInsensitive) ||
              str.startsWith("type",Qt::CaseInsensitive) ||
              str.startsWith("decr",Qt::CaseInsensitive) ||
              str.startsWith("incr",Qt::CaseInsensitive) ||
              str.startsWith("strlen",Qt::CaseInsensitive) ||
              str.startsWith("hgetall",Qt::CaseInsensitive) ||
              str.startsWith("hkeys",Qt::CaseInsensitive) ||
              str.startsWith("hlen",Qt::CaseInsensitive) ||
              str.startsWith("hvals",Qt::CaseInsensitive) ||
              str.startsWith("llen",Qt::CaseInsensitive) ||
              str.startsWith("lpop",Qt::CaseInsensitive) ||
              str.startsWith("rpop",Qt::CaseInsensitive) ||
              str.startsWith("scard",Qt::CaseInsensitive) ||
              str.startsWith("smembers",Qt::CaseInsensitive) ||
              str.startsWith("spop",Qt::CaseInsensitive) ||
              str.startsWith("zcard",Qt::CaseInsensitive)) {
        if(!getKey(str,key,1,1)) {
            result << "-command is error\r\n";
            return result;
        } // ttl key
    } else if(str.startsWith("set",Qt::CaseInsensitive) ||
              str.startsWith("expire",Qt::CaseInsensitive) ||
              str.startsWith("expireat",Qt::CaseInsensitive) ||
              str.startsWith("move",Qt::CaseInsensitive) ||
              str.startsWith("pexpire",Qt::CaseInsensitive) ||
              str.startsWith("pexpireat",Qt::CaseInsensitive) ||
              str.startsWith("rename",Qt::CaseInsensitive) ||
              str.startsWith("renamenx",Qt::CaseInsensitive) ||
              str.startsWith("restore",Qt::CaseInsensitive) ||
              str.startsWith("sort",Qt::CaseInsensitive) ||
              str.startsWith("append",Qt::CaseInsensitive) ||
              str.startsWith("decrby",Qt::CaseInsensitive) ||
              str.startsWith("getbit",Qt::CaseInsensitive) ||
              str.startsWith("getrange",Qt::CaseInsensitive) ||
              str.startsWith("getset",Qt::CaseInsensitive) ||
              str.startsWith("incrby",Qt::CaseInsensitive) ||
              str.startsWith("incrbyfloat",Qt::CaseInsensitive) ||
              str.startsWith("psetex",Qt::CaseInsensitive) ||
              str.startsWith("setbit",Qt::CaseInsensitive) ||
              str.startsWith("setex",Qt::CaseInsensitive) ||
              str.startsWith("setnx",Qt::CaseInsensitive) ||
              str.startsWith("setrange",Qt::CaseInsensitive) ||
              str.startsWith("hdel",Qt::CaseInsensitive) ||
              str.startsWith("hexists",Qt::CaseInsensitive) ||
              str.startsWith("hget",Qt::CaseInsensitive) ||
              str.startsWith("hincrby",Qt::CaseInsensitive) ||
              str.startsWith("hincrbyfloat",Qt::CaseInsensitive) ||
              str.startsWith("hmget",Qt::CaseInsensitive) ||
              str.startsWith("hmset",Qt::CaseInsensitive) ||
              str.startsWith("hset",Qt::CaseInsensitive) ||
              str.startsWith("hsetnx",Qt::CaseInsensitive) ||
              str.startsWith("lindex",Qt::CaseInsensitive) ||
              str.startsWith("linsert",Qt::CaseInsensitive) ||
              str.startsWith("lpush",Qt::CaseInsensitive) ||
              str.startsWith("lpushx",Qt::CaseInsensitive) ||
              str.startsWith("lrange",Qt::CaseInsensitive) ||
              str.startsWith("lrem",Qt::CaseInsensitive) ||
              str.startsWith("lset",Qt::CaseInsensitive) ||
              str.startsWith("ltrim",Qt::CaseInsensitive) ||
              str.startsWith("rpush",Qt::CaseInsensitive) ||
              str.startsWith("rpushx",Qt::CaseInsensitive) ||
              str.startsWith("sadd",Qt::CaseInsensitive) ||
              str.startsWith("sismember",Qt::CaseInsensitive) ||
              str.startsWith("srem",Qt::CaseInsensitive) ||
              str.startsWith("zadd",Qt::CaseInsensitive) ||
              str.startsWith("zcount",Qt::CaseInsensitive) ||
              str.startsWith("zincrby",Qt::CaseInsensitive) ||
              str.startsWith("zrange",Qt::CaseInsensitive) ||
              str.startsWith("zrangebyscore",Qt::CaseInsensitive) ||
              str.startsWith("zrank",Qt::CaseInsensitive) ||
              str.startsWith("zrem",Qt::CaseInsensitive) ||
              str.startsWith("zremrangebyrank",Qt::CaseInsensitive) ||
              str.startsWith("zremrangebyscore",Qt::CaseInsensitive) ||
              str.startsWith("zrevrange",Qt::CaseInsensitive) ||
              str.startsWith("zrevrangebyscore",Qt::CaseInsensitive) ||
              str.startsWith("zrevrank ",Qt::CaseInsensitive) ||
              str.startsWith("zscore",Qt::CaseInsensitive)) {
        if(!getKey(str,key,1,0)) {
            result << "-command is error\r\n";
            return result;
        } //set key cc
    } else if(str.startsWith("migrate",Qt::CaseInsensitive)) {
        if(!getKey(str,key,3,0)) {
            result << "-command is error\r\n";
            return result;
        } //migrate host port key destination-db timeout ...
    } else if(str.startsWith("bitcount ",Qt::CaseInsensitive) ||
              str.startsWith("srandmember",Qt::CaseInsensitive)) {
        if(!getKey(str,key,1,2)) {
            result << "-command is error\r\n";
            return result;
        } //bitcount key [start] [end]
    } else if(str.startsWith("object",Qt::CaseInsensitive)) {
        if(!getKey(str,key,2,2)) {
            result << "-command is error\r\n";
            return result;
        } //object refcount <key>
    } else if(str.startsWith("bitop",Qt::CaseInsensitive)) {
        if(!getKey(str,key,2,2)) {
            result << "-command is error\r\n";
            return result;
        } //BITOP operation destkey key [key ...],命令要求key同槽,因此去一个key路由
    } else if(str.startsWith("mget",Qt::CaseInsensitive) ||
              str.startsWith("del",Qt::CaseInsensitive)) {
        if(!getKey(str,cmd,1,0,0,-1)) {
            result << "-command is error\r\n";
            return result;
        } //mget key [key ...]
    } else if(str.startsWith("mset",Qt::CaseInsensitive) ||
              str.startsWith("msetnx",Qt::CaseInsensitive)) {
        if(!getKey(str,cmd,1,1,2,1)) {
            result << "-command is error\r\n";
            return result;
        } //mset key value [key value ...]
    } else {
        result << "-command is not supported in cluster mode now\r\n";
        return result;
    }

    if(cmd.size() > 0) {
        for(int i = 0; i < cmd.size(); ++i) {
            redisClient = getClient(cmd[i]._key);
            if(!redisClient) {
                result << "-redisClient is null\r\n";
                continue;
            }
            key.clear();
            key.append(cmd[i]._cmd);
            key.append(" ");
            key.append(cmd[i]._key);
            result << redisClient->command(key);
        }
    } else {
        redisClient = getClient(key);
        if(!redisClient) {
            result << "-redisClient is null\r\n";
            return result;
        }
        result << redisClient->command(str);
    }
    return result;
}

bool RedisCluster::pttl(const QString &key, qlonglong & mseconds) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->pttl(key, mseconds)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::zadd(const QString &key, const QString &member, const double &score, qlonglong & llRet) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->zadd(key, member, score, llRet)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::sadd(const QString &key, const QString &member, qlonglong & llRet) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->sadd(key, member, llRet)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::lpush(const QString &key, const QString &value, qlonglong & llRet) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->lpush(key, value, llRet)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::select(const int &index) {
    if(_isClusterMode && index == 0) {
        _dbIndex = index;
        return true;
    } else if(!_isClusterMode) {
        if(!_redisClient) {
            _sErrorInfo = "redis client is null";
            return false;
        }
        if(_redisClient->select(index)) {
            _dbIndex = index;
            return true;
        } else {
            _sErrorInfo = _redisClient->getErrorInfo();
            return false;
        }
    } else {
        _sErrorInfo = "cluster mode unsupport select";
        return false;
    }
}

bool RedisCluster::pexpire(const QString &key, uint mseconds) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->pexpire(key, mseconds)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::hset(const QString &key, const QString &field, const QString &value, qlonglong & llRet) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->hset(key, field, value, llRet)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::del(const QString &key, qlonglong & llRet) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->del(key, llRet)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::set(const QString &key, const QString &value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->set(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::persist(const QString &key) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->persist(key)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::renamex(const QString &key, const QString &newkey) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->renamex(key, newkey)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::get(const QString &key, QByteArray &value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->get(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::type(const QString &key, QByteArray &value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->type(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::getcfg(const QString& parameter, RespType &value) {
    _redisClient = getClient();
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->getcfg(parameter, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::dbsize(qlonglong & llRet) {
    llRet = 0;
    int dbIndex = _dbIndex;
    qlonglong llCount = 0;
    if(_isClusterMode) {
        for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
            if(!_vClusterMasterClients[i]._client) {
                _sErrorInfo = "redis client is null";
                return false;
            }
            if(_vClusterMasterClients[i]._client->dbsize(llCount)) {
                llRet += llCount;
            } else {
                _sErrorInfo = _vClusterMasterClients[i]._client->getErrorInfo();
                return false;
            }
        }
    } else {
        int dbNum = 0;
        if(getDbNum(dbNum)) {
            for(int j = 0; j < dbNum; ++j) {
                if(!select(j)) {
                    return false;
                }
                if(_redisClient->dbsize(llCount)) {
                    llRet += llCount;
                } else {
                    _sErrorInfo = _redisClient->getErrorInfo();
                    return false;
                }
            }
            if(!select(dbIndex)) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

bool RedisCluster::getReplicationMode() const
{
    return _isReplicationMode;
}

bool RedisCluster::getDbNum(int &num) {
    _respType.init();

    if(getcfg("databases", _respType)) {
        num = _respType._arrayValue[1]._stringValue.toInt();
    } else
        return false;

    return true;
}

bool RedisCluster::refcount(const QString& key, qlonglong & value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->refcount(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::idletime(const QString& key, qlonglong & value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->idletime(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

bool RedisCluster::encoding(const QString& key, QByteArray & value) {
    _redisClient = getClient(key);
    if(!_redisClient) {
        _sErrorInfo = "get client by key failed";
        return false;
    }

    if(!_redisClient->encoding(key, value)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    return true;
}

int RedisCluster::getClientIndex() const
{
    return _iClientIndex;
}

bool RedisCluster::getClusterMode() const
{
    return _isClusterMode;
}

QList<QByteArray> RedisCluster::command(QString &str, const QString &split, int index) {
    if(index == -1) {
        if(split.isEmpty()) {
            if(_isClusterMode)
                return clusterCommand(str);
            else {
                QList<QByteArray> result;
                result << singleCommand(str);
                return result;
            }
        } else {
            QList<QByteArray> result;
            QStringList vCmd = str.split(split,QString::SkipEmptyParts);
            for(int i =0 ; i < vCmd.size(); ++i) {
                if(_isClusterMode)
                    result << clusterCommand(vCmd[i]);
                else {
                    result << singleCommand(vCmd[i]);
                }
            }
            return result;
        }
    } else if(index > -1 && index < _vClusterClients.size()) {
        if(split.isEmpty()) {
            QList<QByteArray> result;
            result << singleCommand(str, index);
            return result;
        } else {
            QList<QByteArray> result;
            QStringList vCmd = str.split(split,QString::SkipEmptyParts);
            for(int i =0 ; i < vCmd.size(); ++i) {
                result << singleCommand(vCmd[i], index);
            }
            return result;
        }
    } else {
        QList<QByteArray> result;
        return result;
    }
}

QByteArray RedisCluster::singleCommand(QString &str, int index)
{
    str = str.trimmed();
    //str.replace(QRegExp("[\\s]+"), " ");

    if(str.isEmpty())
        return "-command is empty\r\n";

    if(index == -1) {
        if(_redisClient)
            return _redisClient->command(str);
    } else if(index > -1 && index < _vClusterClients.size()) {
        if(_vClusterClients[index]._client)
            return _vClusterClients[index]._client->command(str);
    }
    return "-client is empty\r\n";
}

RedisClient * RedisCluster::getClient(const QString &key) {
    _byteArray = key.toLocal8Bit();
    return getClient(_byteArray);
}

RedisClient * RedisCluster::getClient(int index) {
    if(_isClusterMode) {
        if(_vClusterMasterClients.size() > index) {
            _iClientIndex = index;
            return _vClusterMasterClients[index]._client;
        } else
            return nullptr;
    } else {
        _iClientIndex = -1;
        return _redisClient;
    }
}

RedisClient *RedisCluster::getClient(QByteArray &key)
{
    if(_isClusterMode) {
        unsigned int slot = getKeySlot(key.data(), key.length());
        for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
            if(slot < _vClusterMasterClients[i]._endSlot) {
                _iClientIndex = i;
                return _vClusterMasterClients[i]._client;
            }
        }
    } else {
        _iClientIndex = -1;
        return _redisClient;
    }
    return nullptr;
}

QString RedisCluster::getConnectName() const
{
    return _ConnectName;
}

void RedisCluster::setConnectName(const QString &ConnectName)
{
    _ConnectName = ConnectName;
}

void RedisCluster::setTimeOutMs(unsigned int iTimeOutMs) {
    _iTimeOut = iTimeOutMs;

    if(_isClusterMode) {
        for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
            if(_vClusterMasterClients[i]._client)
                _vClusterMasterClients[i]._client->setTimeOutMs(_iTimeOut);
        }
    } else {
        if(_redisClient)
            _redisClient->setTimeOutMs(_iTimeOut);
    }
}

void RedisCluster::formatToResp(const RespType &inResp, QByteArray &outResp, int spaceNum) {
    if(_isClusterMode) {
        if(_vClusterMasterClients.size() > 0) {
            if(_vClusterMasterClients[0]._client)
                _vClusterMasterClients[0]._client->formatToResp(inResp, outResp, spaceNum);
        }
    } else {
        if(_redisClient)
            _redisClient->formatToResp(inResp, outResp, spaceNum);
    }
}

void RedisCluster::formatToResp(const QByteArray &inResp, QByteArray &outResp, int spaceNum) {
    _respType.init();
    if(parseResp(inResp,_respType)) {
        formatToResp(_respType, outResp, spaceNum);
    }
}

void RedisCluster::formatToText(const RespType &inResp, QByteArray &outResp, int spaceNum) {
    if(_isClusterMode) {
        if(_vClusterMasterClients.size() > 0) {
            if(_vClusterMasterClients[0]._client)
                _vClusterMasterClients[0]._client->formatToText(inResp, outResp, spaceNum);
        }
    } else {
        if(_redisClient)
            _redisClient->formatToText(inResp, outResp, spaceNum);
    }
}

void RedisCluster::formatToText(const QByteArray &inResp, QByteArray &outResp, int spaceNum) {
    _respType.init();
    if(parseResp(inResp,_respType)) {
        formatToText(_respType, outResp, spaceNum);
    }
}

bool RedisCluster::parseResp(const QByteArray &data, RespType &rResult) {
    if(_isClusterMode) {
        if(_vClusterMasterClients.size() > 0) {
            if(_vClusterMasterClients[0]._client)
                return _vClusterMasterClients[0]._client->parseResp(data, rResult);
        }
    } else {
        if(_redisClient)
            return _redisClient->parseResp(data, rResult);
    }
    return false;
}

void RedisCluster::formatToJson(const RespType &inResp, QJsonArray &outJson) {
    if(_isClusterMode) {
        if(_vClusterMasterClients.size() > 0) {
            if(_vClusterMasterClients[0]._client)
                _vClusterMasterClients[0]._client->formatToJson(inResp, outJson);
        }
    } else {
        if(_redisClient)
            _redisClient->formatToJson(inResp, outJson);
    }
}

void RedisCluster::formatToJson(const QByteArray &inResp, QJsonArray &outJson) {
    _respType.init();
    if(parseResp(inResp,_respType)) {
        formatToJson(_respType, outJson);
    }
}

bool RedisCluster::isOpen() {
    if(!_isClusterMode) {
        if(_redisClient)
            return _redisClient->isOpen();
        else
            return false;
    }

    for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
        if(!_vClusterMasterClients[i]._client ||
                !_vClusterMasterClients[i]._client->isOpen()) {
            return false;
        }
    }

    return true;
}

void RedisCluster::close() {
    if(_isClusterMode || _isReplicationMode) {
        for(int i = 0; i < _vClusterClients.size(); ++i) {
            if(_vClusterClients[i]._client) {
                _vClusterClients[i]._client->close();
                delete _vClusterClients[i]._client;
                _vClusterClients[i]._client = nullptr;
            }
        }
        _redisClient = nullptr;
    } else {
        if(_redisClient) {
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
        }
    }
}

bool RedisCluster::reOpen() {

    if(_isNewOpen) {
        _sErrorInfo = "no old open info";
        return false;
    }

    close();
    if(_isClusterMode) {
        for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
            _vClusterMasterClients[i]._client = new RedisClient();
            _vClusterMasterClients[i]._client->open(_vClusterMasterClients[i]._host,
                                                    _vClusterMasterClients[i]._port);
            if(!_vClusterMasterClients[i]._passwd.isEmpty()) {
                if(!_vClusterMasterClients[i]._client->auth(_vClusterMasterClients[i]._passwd)) {
                    _sErrorInfo = _vClusterMasterClients[i]._client->getErrorInfo();
                    return false;
                }
            }
            if(!_vClusterMasterClients[i]._client->isOpen()) {
                _sErrorInfo = "open redis failed";
                return false;
            }
        }
    } else {
        _redisClient = new RedisClient();
        _redisClient->open(_host, _port);
        if(!_passwd.isEmpty()) {
            if(!_redisClient->auth(_passwd)) {
                _sErrorInfo = _redisClient->getErrorInfo();
                return false;
            }
        }
        if(!_redisClient->isOpen()) {
            _sErrorInfo = "open redis failed";
            return false;
        }
    }

    return true;
}

bool RedisCluster::copyNewClients(QList<ClusterClient> &vClients) {

    ClusterClient clusterClient;
    if(_isClusterMode) {
        for(int i = 0; i < _vClusterMasterClients.size(); ++i) {
            clusterClient.init();
            clusterClient = _vClusterMasterClients[i];
            clusterClient._client = new RedisClient();
            clusterClient._client->open(clusterClient._host,
                                        clusterClient._port);
            if(!clusterClient._passwd.isEmpty()) {
                if(!clusterClient._client->auth(clusterClient._passwd)) {
                    _sErrorInfo = clusterClient._client->getErrorInfo();
                    vClients.clear();
                    return false;
                }
            }
            if(!clusterClient._client->isOpen()) {
                _sErrorInfo = "open redis failed";
                vClients.clear();
                return false;
            }
            vClients << clusterClient;
        }
    } else {
        clusterClient.init();
        clusterClient._passwd = _passwd;
        clusterClient._host = _host;
        clusterClient._port = _port;
        clusterClient._client = new RedisClient();
        clusterClient._client->open(_host, _port);
        if(!_passwd.isEmpty()) {
            if(!clusterClient._client->auth(_passwd)) {
                _sErrorInfo = clusterClient._client->getErrorInfo();
                return false;
            }
        }
        if(!clusterClient._client->isOpen()) {
            _sErrorInfo = "open redis failed";
            return false;
        }
        vClients << clusterClient;
    }
    return true;
}

QList<ClusterClient> RedisCluster::getClients(bool isMaster) const
{
    if(_isClusterMode || _isReplicationMode) {
        if(isMaster)
            return _vClusterMasterClients;
        else
            return _vClusterClients;
    } else {
        QList<ClusterClient> clients;
        ClusterClient clusterClient;
        clusterClient._client = _redisClient;
        clusterClient._endSlot = 16383;
        clusterClient._host = _host;
        clusterClient._master = true;
        clusterClient._passwd = _passwd;
        clusterClient._startSlot = 0;
        clusterClient._port = _port;
        clusterClient._slotNum = 16384;
        clusterClient._nodeId.clear();
        clients << clusterClient;
        return clients;
    }
}

bool RedisCluster::openSingel(const QString &hostAddress, const QString passwd, int timeOut) {

    close();
    int index = hostAddress.indexOf(":");
    if(index == -1 || hostAddress.size() < 6) {
        _sErrorInfo = "redis node string is error";
        return false;
    }
    _host = hostAddress.mid(0,index);
    _port = hostAddress.mid(index + 1).toUShort();
    _passwd = passwd;
    _timeOut = timeOut;
    _hostAddress = hostAddress;

    _redisClient = new RedisClient();
    if(!_redisClient->open(_host, _port, timeOut)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        return false;
    }

    if(!_passwd.isEmpty()) {
        if(!_redisClient->auth(_passwd)) {
            _sErrorInfo = _redisClient->getErrorInfo();
            return false;
        }
    }

    if(!_redisClient->isOpen()) {
        _sErrorInfo = "open redis failed";
        return false;
    }
    _isNewOpen = false;
    _isClusterMode = false;

    return true;
}

bool RedisCluster::openCluster() {
    return openCluster(_hostAddress, _passwd, _onlyMaster, _timeOut);
}

bool RedisCluster::openCluster(const QString &hostAddress,
                               const QString passwd,
                               const bool master,
                               int timeOut) {
    close();
    int index;
    bool open = false;
    RespType respType;
    QStringList hostList;
    RespType respArray;
    QString strMasterId;
    ClusterClient clusterClient;
    _passwd = passwd;
    _timeOut = timeOut;
    _onlyMaster = master;
    _hostAddress = hostAddress;
    _vClusterClients.clear();
    _vClusterMasterClients.clear();

    clusterClient._passwd = passwd;
    hostList =  hostAddress.split(',',QString::SkipEmptyParts);
    for(int i = 0; i < hostList.size(); ++i) {
        index = hostList[i].indexOf(":");
        if(index == -1) {
            _sErrorInfo = "redis node string info is error";
            return false;
        }
        clusterClient._host = hostList[i].mid(0, index);
        clusterClient._port = hostList[i].mid(index + 1).toUShort();

        if(_redisClient == nullptr)
            _redisClient = new RedisClient();
        if(_redisClient->open(clusterClient._host, clusterClient._port, timeOut)) {
            if(!clusterClient._passwd.isEmpty()) {
                if(!_redisClient->auth(clusterClient._passwd)) {
                    _sErrorInfo = _redisClient->getErrorInfo();
                    _redisClient->close();
                    continue;
                }
            }
            open = true;
            break;
        } else {
            _sErrorInfo = _redisClient->getErrorInfo();
            _redisClient->close();
            continue;
        }
    }

    if(!open) {
        if(_redisClient) {
            delete _redisClient;
            _redisClient = nullptr;
        }
        return false;
    }

    if(!_redisClient->isCluster(_isClusterMode)) {
        _sErrorInfo = _redisClient->getErrorInfo();
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        return false;
    }

    if(!_isClusterMode) {
        QMap<QString,QString> infoMap;
        if(!_redisClient->getReplicationInfo(infoMap)) {
            _sErrorInfo = _redisClient->getErrorInfo();
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return false;
        }

        QString role = infoMap.value("role", "nil");
        if(role == "slave") {
            clusterClient._host = infoMap.value("master_host", "nil");
            if(clusterClient._host == "nil") {
                _sErrorInfo = "get client master ip failed";
                return false;
            }
            QString port = infoMap.value("master_port", "nil");
            if(port == "nil") {
                _sErrorInfo = "get client master port failed";
                return false;
            }
            clusterClient._port = port.toUShort();
            _redisClient->close();
            delete _redisClient;
            _redisClient = new RedisClient();
            if(_redisClient->open(clusterClient._host, clusterClient._port, timeOut)) {
                if(!clusterClient._passwd.isEmpty()) {
                    if(!_redisClient->auth(clusterClient._passwd)) {
                        _sErrorInfo = _redisClient->getErrorInfo();
                        _redisClient->close();
                        return false;
                    }
                }
            } else {
                _sErrorInfo = _redisClient->getErrorInfo();
                _redisClient->close();
                return false;
            }
            infoMap.clear();
            if(!_redisClient->getReplicationInfo(infoMap)) {
                _sErrorInfo = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
        }

        role = infoMap.value("role", "nil");
        if(role == "master") {
            int iSlaveNum = infoMap.value("connected_slaves", "-1").toInt();
            if(iSlaveNum == -1) {
                _sErrorInfo = "get client slave num failed";
                return false;
            } else if(iSlaveNum == 0) {
                _isNewOpen = false;
                _isReplicationMode = false;
                _host = clusterClient._host;
                _port = clusterClient._port;
                return true;
            } else {
                _isReplicationMode = true;
                clusterClient._client = nullptr;
                clusterClient._endSlot = 16383;
                clusterClient._master = true;
                clusterClient._passwd = _passwd;
                clusterClient._startSlot = 0;
                clusterClient._slotNum = 16384;
                clusterClient._nodeId.clear();
                _vClusterClients << clusterClient;
                QString slaveInfo;
                for(int k = 0; k < iSlaveNum; ++k) {
                    slaveInfo = infoMap.value(QString("slave%1").arg(k), "nil");
                    hostList =  slaveInfo.split(',',QString::SkipEmptyParts);
                    for(int n = 0; n < hostList.size(); ++n) {
                        index = hostList[n].indexOf("ip=");
                        if(index != -1)
                            clusterClient._host = hostList[n].mid(3);

                        index = hostList[n].indexOf("port=");
                        if(index != -1)
                            clusterClient._port = hostList[n].mid(5).toUShort();
                    }
                    clusterClient._master = false;
                    _vClusterClients << clusterClient;
                }
            }
        } else {
            _sErrorInfo = "get client role failed";
            return false;
        }
    } else {
        if(!_redisClient->getClusterSlots(respType)) {
            _sErrorInfo = _redisClient->getErrorInfo();
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return false;
        }

        for(int j = 0; j < respType._arrayLength; ++j) {
            respArray.init();
            respArray = respType._arrayValue[j];
            clusterClient.init();
            clusterClient._passwd = passwd;
            clusterClient._startSlot = respArray._arrayValue[0]._integerValue;
            clusterClient._endSlot = respArray._arrayValue[1]._integerValue;
            clusterClient._slotNum = clusterClient._endSlot - clusterClient._startSlot + 1;
            for(int k = 2; k < respArray._arrayLength; ++k) {
                clusterClient._host = respArray._arrayValue[k]._arrayValue[0]._stringValue;
                clusterClient._port = respArray._arrayValue[k]._arrayValue[1]._integerValue;
                clusterClient._nodeId = respArray._arrayValue[k]._arrayValue[2]._stringValue;
                if(k == 2) {
                    strMasterId = clusterClient._nodeId;
                    clusterClient._master = true;
                    clusterClient._masterId.clear();
                } else {
                    clusterClient._master = false;
                    clusterClient._masterId = strMasterId;
                }
                _vClusterClients << clusterClient;
            }
        }
    }

    _redisClient->close();
    delete _redisClient;
    _redisClient = nullptr;

    for(int m = 0; m < _vClusterClients.size(); ++m) {
        if(master && !_vClusterClients[m]._master)
            continue;

        _redisClient = new RedisClient();
        if(_redisClient->open(_vClusterClients[m]._host,
                              _vClusterClients[m]._port,
                              timeOut)) {
            if(!_vClusterClients[m]._passwd.isEmpty()) {
                if(!_redisClient->auth(_vClusterClients[m]._passwd)) {
                    _sErrorInfo = _redisClient->getErrorInfo();
                    _redisClient->close();
                    delete _redisClient;
                    _redisClient = nullptr;
                    return false;
                }
            }
        } else {
            _sErrorInfo = _redisClient->getErrorInfo();
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return false;
        }
        _vClusterClients[m]._client = _redisClient;

        if(_vClusterClients[m]._master)
            _vClusterMasterClients << _vClusterClients[m];
    }

    if(_vClusterMasterClients.size() == 0) {
        _sErrorInfo = "redis node num is empty";
        return false;
    }

    if(_isReplicationMode && _vClusterClients[0]._master) {
        _redisClient = _vClusterClients[0]._client;
    } else {
        _redisClient = nullptr;
    }

    // 按_startSlot升序排序
    qSort(_vClusterMasterClients.begin(),_vClusterMasterClients.end(),
          [](const ClusterClient &infoA,const ClusterClient &infoB) {
        return infoA._startSlot < infoB._startSlot;
    });

    _isNewOpen = false;
    return true;
}

void RedisCluster::setCluster(const bool &isCluster) {
    _isClusterMode = isCluster;
}

bool RedisCluster::getCluster() const {
    return _isClusterMode;
}
