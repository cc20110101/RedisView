/**
* @file      WorkThread.cpp
* @brief     线程工作类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Public/WorkThread.h"

WorkThread::WorkThread(TaskMsg *taskMsg, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TaskMsg>();
    setAutoDelete(true);
    _taskMsg = taskMsg;
    _string.clear();
    _sql.clear();
    _respValue.init();
    _sendMsg.init();
    _cursor = "0";
    _taskid = -1;
    _db = nullptr;
    _redisClient = nullptr;
    _redisClusterClient = nullptr;
}

WorkThread::WorkThread(QList<CmdMsg> &cmd, TaskMsg *taskMsg, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TaskMsg>();
    qRegisterMetaType<CmdMsg>();
    setAutoDelete(true);
    _cmd = cmd;
    _taskMsg = taskMsg;
    _string.clear();
    _sql.clear();
    _respValue.init();
    _sendMsg.init();
    _cursor = "0";
    _taskid = -1;
    _db = nullptr;
    _redisClient = nullptr;
    _redisClusterClient = nullptr;
}

void WorkThread::run() {
    if(_taskMsg->_taskid == THREAD_SCAN_KEY_TASK) {
        doKeyListWork();
    } else if(_taskMsg->_taskid == THREAD_SCAN_VALUE_TASK) {
        doValueListWork();
    } else if(_taskMsg->_taskid == THREAD_COMMIT_VALUE_TASK) {
        doCommitValueWork();
    } else if (_taskMsg->_taskid == THREAD_DEL_KEY_TASK) {
        doDelKeyWork();
    } else if (_taskMsg->_taskid == THREAD_BATCH_DEL_KEY_TASK) {
        doBatchDelKeyWork();
    } else if (_taskMsg->_taskid == THREAD_BATCH_SCAN_KEY_TASK) {
        doBatchScanKeyWork();
    } else if (_taskMsg->_taskid == THREAD_BATCH_MEM_KEY_TASK ||
               _taskMsg->_taskid == THREAD_BATCH_OEM_KEY_TASK) {
        doBatchExportWork(_taskMsg->_taskid);
    } else if (_taskMsg->_taskid == THREAD_BATCH_OIM_KEY_TASK ||
               _taskMsg->_taskid == THREAD_BATCH_MIM_KEY_TASK) {
        doBatchImportWork(_taskMsg->_taskid);
    } else if (_taskMsg->_taskid == THREAD_BATCH_MDE_KEY_TASK ||
               _taskMsg->_taskid == THREAD_BATCH_ODE_KEY_TASK) {
        doBatchDbDeleWork(_taskMsg->_taskid);
    } else {
        emit runError(_taskMsg->_taskid, "This operation does not exist");
        emit finishWork(_taskMsg->_taskid);
    }
}

void WorkThread::cancelWork(const int taskid) {
    _taskid = taskid;
}

bool WorkThread::prepare(int mode, bool clusterMode, bool customMode) {
    if(mode == WORK_THREAD_MODE1) {
        _string.clear();
        if(_taskMsg == nullptr) {
            _string = "TaskMsg is null";
            return false;
        }

        if(clusterMode && !customMode) {
            _redisClusterClient = new RedisCluster();
            if(!_redisClusterClient->openCluster(QString("%1:%2").
                                                 arg(_taskMsg->_host).
                                                 arg(_taskMsg->_port),
                                                 _taskMsg->_passwd)) {
                _string = _redisClusterClient->getErrorInfo();
                _redisClusterClient->close();
                delete _redisClusterClient;
                _redisClusterClient = nullptr;
                return false;
            }
        } else {
            _redisClient = new RedisClient();
            if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
                _string = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
            if(!_taskMsg->_passwd.isEmpty()) {
                if(!_redisClient->auth(_taskMsg->_passwd)) {
                    _string = _redisClient->getErrorInfo();
                    _redisClient->close();
                    delete _redisClient;
                    _redisClient = nullptr;
                    return false;
                }
            }

            if(!_redisClient->isOpen()) {
                _string = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
        }
    } else {
        _string = "function prepare mode error";
        return false;
    }

    return true;
}

bool WorkThread::prepare(int mode, int cluster) {

    if(mode == WORK_THREAD_MODE1) {
        _string.clear();
        if(_taskMsg == nullptr) {
            _string = "TaskMsg is null";
            return false;
        }

        if(cluster) {
            _redisClusterClient = new RedisCluster();
            if(!_redisClusterClient->openCluster(QString("%1:%2").
                                                 arg(_taskMsg->_host).
                                                 arg(_taskMsg->_port),
                                                 _taskMsg->_passwd)) {
                _string = _redisClusterClient->getErrorInfo();
                _redisClusterClient->close();
                delete _redisClusterClient;
                _redisClusterClient = nullptr;
                return false;
            }
        } else {
            _redisClient = new RedisClient();
            if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
                _string = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
            if(!_taskMsg->_passwd.isEmpty()) {
                if(!_redisClient->auth(_taskMsg->_passwd)) {
                    _string = _redisClient->getErrorInfo();
                    _redisClient->close();
                    delete _redisClient;
                    _redisClient = nullptr;
                    return false;
                }
            }

            if(!_redisClient->isOpen()) {
                _string = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
        }
    } else if(mode == WORK_THREAD_MODE0) {
        _string.clear();
        if(_taskMsg == nullptr) {
            _string = "TaskMsg is null";
            return false;
        }

        _redisClient = new RedisClient();
        if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
            _string = _redisClient->getErrorInfo();
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return false;
        }
        if(!_taskMsg->_passwd.isEmpty()) {
            if(!_redisClient->auth(_taskMsg->_passwd)) {
                _string = _redisClient->getErrorInfo();
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
        }

        if(!_redisClient->isOpen()) {
            _string = _redisClient->getErrorInfo();
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return false;
        }

        if(_taskMsg->_dbIndex > 0) {
            if(!_redisClient->select(_taskMsg->_dbIndex)) {
                _string = "select db failed";
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return false;
            }
        }
    } else {
        _string = "function prepare mode error";
        return false;
    }

    return true;
}

bool WorkThread::cancle(int mode) {
    if(_taskid == _taskMsg->_taskid) {
        destroy(mode);
        return true;
    }
    return false;
}

void WorkThread::destroy(int mode) {
    if(mode == WORK_THREAD_MODE0) {
        if(_redisClient) {
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
        }
    } else if(mode == WORK_THREAD_MODE1) {
        if(_taskMsg->_clusterMode && !_taskMsg->_customMode) {
            if(_redisClusterClient) {
                _redisClusterClient->close();
                delete _redisClusterClient;
                _redisClusterClient = nullptr;
            }
        } else {
            if(_redisClient) {
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
            }
        }
    }

    emit finishWork(_taskMsg->_taskid);

    if(_taskMsg) {
        delete _taskMsg;
        _taskMsg = nullptr;
    }

    if(_db) {
        delete _db;
        _db = NULL;
    }
    _dbMgr.close();
    _sql.clear();
    _cmd.clear();
}

int WorkThread::deleteData(int taskid) {
    int count = 0;
    qlonglong ret = 0;
    ImpExpData recordData;

    if(!_db)
        _db = new QSqlDatabase();

    if(!_db->isValid()) {
        if(taskid == THREAD_BATCH_ODE_KEY_TASK) {
            _dbMgr.setCfg(ORACLE_DB, ORACLE_DRIVE);
            _sql = QString(
                        "SELECT KEY,DB_INDEX FROM %1 WHERE MOD(ID,%2)"
                        "=%3 AND STATE=0 ORDER BY DB_INDEX ASC")
                    .arg(_tableName).arg(MAX_THREAD_COUNT).arg(_thread);
        } else if(taskid == THREAD_BATCH_MDE_KEY_TASK) {
            _dbMgr.setCfg(MYSQL_DB, MYSQL_DRIVE);
            _sql = QString(
                        "SELECT `KEY`, `DB_INDEX` FROM `%1` WHERE ID "
                        "MOD %2 = %3 AND `STATE`=0 ORDER BY `DB_INDEX` "
                        "ASC").arg(_tableName).arg(MAX_THREAD_COUNT)
                    .arg(_thread);
        }
        _dbMgr.setLable(QString("%1%2").
                        arg(_dbMgr.lable()).arg(_thread));

        if(!_dbMgr.getDb(_db)) {
            _string = _dbMgr.error();
            return -1;
        }
    }

    QSqlQuery sql_query(*_db);
    sql_query.exec(_sql);
    while(sql_query.next()) {
        recordData.sKey = sql_query.value(0).toString();
        recordData.iState = sql_query.value(1).toInt();
        if(_taskMsg->_clusterMode && !_taskMsg->_customMode) {
            //_redisClusterClient
            if(!_redisClusterClient->del(recordData.sKey, ret)){
                continue;
            }
        } else {
            //_redisClient
            if(!_redisClient->del(recordData.sKey, ret)){
                continue;
            }
        }

        count += ret;
        if(count >= 1000) {
            _sendMsg._sequence = count;
            emit sendData(_sendMsg);
            count = 0;
        }

        if(cancle(WORK_THREAD_MODE1)) {
            sql_query.clear();
            return count + 1;
        }
    }

    sql_query.clear();
    _sendMsg._sequence = count;
    emit sendData(_sendMsg);
    count = 0;
    return count;
}

int WorkThread::imporData(int taskid) {

    int dbindex = -1;
    qlonglong ret = 0;
    int count = 0;
    ImpExpData recordData;

    if(!_db)
        _db = new QSqlDatabase();

    if(!_db->isValid()) {
        if(taskid == THREAD_BATCH_OIM_KEY_TASK) {
            _dbMgr.setCfg(ORACLE_DB, ORACLE_DRIVE);
            if(_dbindex) {
                _sql = QString(
                            "SELECT KEY,KEY_TYPE,FILED,VALUE,TIME_OUT "
                            "- (SYSDATE-EXP_DATE)*86400, WEIGHT,DB_INDEX"
                            " FROM %1 WHERE MOD(ID,%2) = %3 AND STATE=0 "
                            "ORDER BY DB_INDEX, WEIGHT ASC")
                        .arg(_tableName).arg(MAX_THREAD_COUNT).arg(_thread);
            } else {
                _sql = QString(
                            "SELECT KEY,KEY_TYPE,FILED,VALUE,TIME_OUT,"
                            "WEIGHT,DB_INDEX FROM %1 WHERE MOD(ID,%2)"
                            "=%3 AND STATE=0 ORDER BY DB_INDEX, WEIGHT "
                            "ASC").arg(_tableName).arg(MAX_THREAD_COUNT)
                        .arg(_thread);
            }
        } else if(taskid == THREAD_BATCH_MIM_KEY_TASK) {
            _dbMgr.setCfg(MYSQL_DB, MYSQL_DRIVE);
            if(_dbindex) {
                _sql = QString(
                            "SELECT `KEY`, `KEY_TYPE`,`FILED`,`VALUE`, "
                            "`TIME_OUT` - TIMESTAMPDIFF(SECOND,`EXP_DATE`"
                            ",SYSDATE()),`WEIGHT`, `DB_INDEX` FROM `%1` "
                            "WHERE ID MOD %2 = %3 AND `STATE`=0 ORDER BY "
                            "`DB_INDEX`,`WEIGHT` ASC")
                        .arg(_tableName).arg(MAX_THREAD_COUNT).arg(_thread);
            } else {
                _sql = QString(
                            "SELECT `KEY`, `KEY_TYPE`,`FILED`,`VALUE`, "
                            "`TIME_OUT`,`WEIGHT`, `DB_INDEX` FROM `%1` "
                            "WHERE ID MOD %2 = %3 AND `STATE`=0 ORDER BY "
                            "`DB_INDEX`,`WEIGHT` ASC")
                        .arg(_tableName).arg(MAX_THREAD_COUNT).arg(_thread);
            }
        }
        _dbMgr.setLable(QString("%1%2").
                        arg(_dbMgr.lable()).arg(_thread));

        if(!_dbMgr.getDb(_db)) {
            _string = _dbMgr.error();
            return -1;
        }
    }

    QSqlQuery sql_query(*_db);
    if(!sql_query.exec(_sql)) {
        _string = sql_query.lastError().text();
        sql_query.clear();
        return -1;
    }

    while(sql_query.next()) {
        recordData.sKey = sql_query.value(0).toString();
        recordData.sKeyType = sql_query.value(1).toString();
        recordData.sFiled = sql_query.value(2).toString();
        recordData.sValue = sql_query.value(3).toString();
        recordData.lTimeOut = sql_query.value(4).toLongLong();
        recordData.lWeight = sql_query.value(5).toLongLong();
        recordData.iState = sql_query.value(6).toInt();
        if(_taskMsg->_clusterMode && !_taskMsg->_customMode) {
            if(recordData.lTimeOut > 0) {
                if(!_redisClusterClient->pexpire(recordData.sKey, recordData.lTimeOut)) {
                    PubLib::log(QString("import set timeout failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else {
                continue;
            }

            if(recordData.sKeyType == "string") {
                if(!_redisClusterClient->set(recordData.sKey, recordData.sValue)) {
                    PubLib::log(QString("import string failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "list") {
                if(!_redisClusterClient->lpush(recordData.sKey, recordData.sValue, ret)) {
                    PubLib::log(QString("import list failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "hash") {
                if(!_redisClusterClient->hset(recordData.sKey, recordData.sFiled, recordData.sValue, ret)) {
                    PubLib::log(QString("import hash failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "set") {
                if(!_redisClusterClient->sadd(recordData.sKey, recordData.sValue, ret)) {
                    PubLib::log(QString("import set failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "zset") {
                if(!_redisClusterClient->zadd(recordData.sKey, recordData.sValue, recordData.lWeight, ret)) {
                    PubLib::log(QString("import zset failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else {
                PubLib::log(QString("import failed,error type %1").arg(recordData.sKeyType));
                continue;
            }

        } else {
            if(recordData.lTimeOut > 0) {
                if(!_redisClient->pexpire(recordData.sKey, recordData.lTimeOut)) {
                    PubLib::log(QString("import set timeout failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else {
                continue;
            }

            if(dbindex != recordData.iState) {
                dbindex = recordData.iState;
                if(!_redisClient->select(dbindex)) {
                    PubLib::log(QString("import failed,select index %1 failed").arg(dbindex));
                    continue;
                }
            }

            if(recordData.sKeyType == "string") {
                if(!_redisClient->set(recordData.sKey, recordData.sValue)) {
                    PubLib::log(QString("import string failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "list") {
                if(!_redisClient->lpush(recordData.sKey, recordData.sValue, ret)) {
                    PubLib::log(QString("import list failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "hash") {
                if(!_redisClient->hset(recordData.sKey, recordData.sFiled, recordData.sValue, ret)) {
                    PubLib::log(QString("import hash failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "set") {
                if(!_redisClient->sadd(recordData.sKey, recordData.sValue, ret)) {
                    PubLib::log(QString("import set failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else if(recordData.sKeyType == "zset") {
                if(!_redisClient->zadd(recordData.sKey, recordData.sValue, recordData.lWeight, ret)) {
                    PubLib::log(QString("import zset failed,key=%1").arg(recordData.sKey));
                    continue;
                }
            } else {
                PubLib::log(QString("import failed,error type %1").arg(recordData.sKeyType));
                continue;
            }
        }

        ++count;
        if(count >= 1000) {
            _sendMsg._sequence = count;
            emit sendData(_sendMsg);
            count = 0;
        }

        if(cancle(WORK_THREAD_MODE1)) {
            sql_query.clear();
            return count + 1;
        }
    }

    sql_query.clear();
    _sendMsg._sequence = count;
    emit sendData(_sendMsg);
    count = 0;
    return count;
}

int WorkThread::exportData(std::vector<ImpExpData> &vImpExpData, int taskid)
{
    if(vImpExpData.size() <= 0)
        return 0;

    if(!_db)
        _db = new QSqlDatabase();

    if(!_db->isValid()) {
        if(taskid == THREAD_BATCH_OEM_KEY_TASK) {
            _dbMgr.setCfg(ORACLE_DB, ORACLE_DRIVE);
            _sql = QString(
                        "INSERT INTO %1 (ID,DB_INDEX,STATE,WEIGHT,TIME_OUT,"
                        "KEY,KEY_TYPE,FILED,VALUE, EXP_DATE) VALUES "
                        "(?,%2,?,?,?,?,?,?,?,TO_DATE(?, 'yyyymmddhh24miss'))")
                    .arg(_tableName).arg(_dbindex);
        } else if(taskid == THREAD_BATCH_MEM_KEY_TASK) {
            _dbMgr.setCfg(MYSQL_DB, MYSQL_DRIVE);
            _sql = QString(
                        "INSERT INTO `%1` (ID,`DB_INDEX`,`STATE`,`WEIGHT`,"
                        "`TIME_OUT`, `KEY`,`KEY_TYPE`,`FILED`,`VALUE`, "
                        "`EXP_DATE`) VALUES (?,%2,?,?,?,?,?,?,?,STR_TO_DATE(?,"
                        "'%Y%m%d%H%i%s'))")
                    .arg(_tableName).arg(_dbindex);
        }
        _dbMgr.setLable(QString("%1%2%3").
                        arg(_dbMgr.lable()).arg(_thread).arg(_dbindex));

        if(!_dbMgr.getDb(_db)) {
            _string = _dbMgr.error();
            return -1;
        }
    }

    QSqlQuery sql_query(*_db);
    sql_query.prepare(_sql);
    QVariantList v0, v1, v2, v3, v4, v5, v6, v7, v8;
    for(std::size_t i = 0; i < vImpExpData.size(); ++i) {
        v0 << PubLib::getSequenceId() ;
        v1 << vImpExpData[i].iState;
        v2 << vImpExpData[i].lWeight;
        v3 << vImpExpData[i].lTimeOut;
        v4 << vImpExpData[i].sKey;
        v5 << vImpExpData[i].sKeyType;
        v6 << vImpExpData[i].sFiled;
        v7 << vImpExpData[i].sValue;
        v8 << vImpExpData[i].sExpDate;
    }
    sql_query.addBindValue(v0);
    sql_query.addBindValue(v1);
    sql_query.addBindValue(v2);
    sql_query.addBindValue(v3);
    sql_query.addBindValue(v4);
    sql_query.addBindValue(v5);
    sql_query.addBindValue(v6);
    sql_query.addBindValue(v7);
    sql_query.addBindValue(v8);

    if(!sql_query.execBatch()) {
        _string = sql_query.lastError().text();
        sql_query.clear();
        return -1;
    }
    sql_query.clear();

    return vImpExpData.size();
}

void WorkThread::doKeyListWork()
{
    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();
    do {
        if(_redisClient->scan(_sendMsg._keyPattern, _sendMsg._respResult, _cursor, BATCH_SCAN_NUM)) {
            _cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
            emit sendData(_sendMsg);
        } else {
            _cursor = "0";
        }
        _sendMsg._respResult.init();
    } while(_cursor.toLongLong());

    destroy();
}

void WorkThread::doValueListWork() {

    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();
    if(_taskMsg->_type == KEY_HASH) {
        do {
            if(_redisClient->hscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult, _cursor, BATCH_SCAN_NUM)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = "0";
            }
            _sendMsg._respResult.init();
        } while(_cursor.toLongLong());
    } else if(_taskMsg->_type == KEY_ZSET) {
        do {
            if(_redisClient->zscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult, _cursor, BATCH_SCAN_NUM)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = "0";
            }
            _sendMsg._respResult.init();
        } while(_cursor.toLongLong());
    } else if(_taskMsg->_type == KEY_SET) {
        do {
            if(_redisClient->sscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult , _cursor, BATCH_SCAN_NUM)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = "0";
            }
            _sendMsg._respResult.init();
        } while(_cursor.toLongLong());
    } else if(_taskMsg->_type == KEY_LIST) {
        int start = 0;
        int stop = start + BATCH_SCAN_NUM;
        do {
            if(_redisClient->lrange(_sendMsg._key, start, stop, _sendMsg._list)) {
                emit sendData(_sendMsg);
                start = stop + 1;
                stop = start + BATCH_SCAN_NUM;
                _cursor = _sendMsg._list.size();
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                _cursor = "0";
            }
            _sendMsg._list.clear();
        } while(_cursor.toLongLong());
    } else if(_taskMsg->_type == KEY_STRING) {
        if(_redisClient->get(_sendMsg._key, _byteArray)) {
            _sendMsg._list << _byteArray;
            emit sendData(_sendMsg);
        } else {
            _string = _redisClient->getErrorInfo();
            emit runError(_taskMsg->_taskid,_string);
        }
        _sendMsg._list.clear();
    }

    destroy();
}

void WorkThread::doCommitValueWork() {

    if(_cmd.size() <= 0) {
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    qlonglong llret;
    for(int i =0; i < _cmd.size(); ++i) {
        switch (_cmd[i]._operate) {
        case OPERATION_ADD:
            if(_cmd[i]._type == KEY_HASH) {
                if(!_redisClient->hset(_cmd[i]._key,_cmd[i]._filed,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_ZSET) {
                if(!_redisClient->zadd(_cmd[i]._key,_cmd[i]._value,_cmd[i]._score,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_SET) {
                if(!_redisClient->sadd(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_LIST) {
                if(_cmd[i]._valueIndex == 1) {
                    if(!_redisClient->rpush(_cmd[i]._key,_cmd[i]._value,llret)) {
                        _string = _redisClient->getErrorInfo();
                        emit runError(_taskMsg->_taskid,_string);
                        continue;
                    }
                } else if(_cmd[i]._valueIndex == -1) {
                    if(!_redisClient->lpush(_cmd[i]._key,_cmd[i]._value,llret)) {
                        _string = _redisClient->getErrorInfo();
                        emit runError(_taskMsg->_taskid,_string);
                        continue;
                    }
                }
            }
            break;
        case OPERATION_DELETE:
            if(_cmd[i]._type == KEY_HASH) {
                if(!_redisClient->hdel(_cmd[i]._key,_cmd[i]._filed,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_ZSET) {
                if(!_redisClient->zrem(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_SET) {
                if(!_redisClient->srem(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_LIST) {
                if(_cmd[i]._valueIndex == 1) {
                    if(!_redisClient->rpop(_cmd[i]._key,_byteArray)) {
                        _string = _redisClient->getErrorInfo();
                        emit runError(_taskMsg->_taskid,_string);
                        continue;
                    }
                } else if(_cmd[i]._valueIndex == -1) {
                    if(!_redisClient->lpop(_cmd[i]._key,_byteArray)) {
                        _string = _redisClient->getErrorInfo();
                        emit runError(_taskMsg->_taskid,_string);
                        continue;
                    }
                }
            }
            break;
        case OPERATION_ALTER:
            if(_cmd[i]._type == KEY_HASH) {
                if(!_redisClient->hset(_cmd[i]._key,_cmd[i]._filed,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_STRING) {
                if(!_redisClient->set(_cmd[i]._key,_cmd[i]._value)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == KEY_LIST) {
                if(!_redisClient->lset(_cmd[i]._key,_cmd[i]._valueIndex,_cmd[i]._value)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            }
            break;
        case OPERATION_TIMEOUT:
            if(!_cmd[i]._value.isEmpty() && _cmd[i]._value.toLongLong() >= 0) {
                if(!_redisClient->pexpire(_cmd[i]._key,_cmd[i]._value.toLongLong())) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else {
                if(_cmd[i]._valueIndex >= 0) {
                    if(!_redisClient->persist(_cmd[i]._key)) {
                        _string = _redisClient->getErrorInfo();
                        emit runError(_taskMsg->_taskid,_string);
                        continue;
                    }
                }
            }
            break;
        case OPERATION_RENAME:
            if(!_redisClient->renamex(_cmd[i]._key,_cmd[i]._value)) {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                continue;
            }
            break;
        default:
            break;
        }
    }

    destroy();
}

void WorkThread::doDelKeyWork() {
    if(_cmd.size() <= 0) {
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(!prepare(WORK_THREAD_MODE1, _taskMsg->_clusterMode, _taskMsg->_customMode)) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_clusterMode && !_taskMsg->_customMode) {
        qlonglong qllRet;
        for(int i =0; i < _cmd.size(); ++i) {
            if(!_redisClusterClient->del(_cmd[i]._key, qllRet)) {
                _string = "delete key failed";
                emit runError(_taskMsg->_taskid,_string);
                continue;
            }
        }
    } else {
        int iLastIndex = -1;
        qlonglong qllRet;
        for(int i = 0; i < _cmd.size(); ++i) {
            if(iLastIndex != _cmd[i]._dbIndex) {
                if(!_redisClient->select(_cmd[i]._dbIndex)) {
                    _string = "select db failed";
                    iLastIndex = -1;
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                } else {
                    iLastIndex = _cmd[i]._dbIndex;
                }
            }

            if(!_redisClient->del(_cmd[i]._key, qllRet)) {
                _string = "delete key failed";
                emit runError(_taskMsg->_taskid,_string);
                continue;
            }
        }
    }

    destroy(WORK_THREAD_MODE1);
}

void WorkThread::doBatchDelKeyWork() {

    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(cancle())
        return;

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    qlonglong llret;
    QString key;
    int keyNum;
    QStringList vKeyPattern = _taskMsg->_keyPattern.split(_taskMsg->_key, QString::SkipEmptyParts);
    for(int i = 0; i < vKeyPattern.size(); ++i) {
        _sendMsg._key = vKeyPattern[i];
        do {
            if(_redisClient->scan(vKeyPattern[i], _respValue, _cursor, BATCH_SCAN_NUM)) {
                _cursor = _respValue._arrayValue[0]._stringValue;
                key.clear();
                keyNum = 0;
                for(int j = 0; j < _respValue._arrayValue[1]._arrayValue.size(); ++j) {
                    key = QString(_respValue._arrayValue[1]._arrayValue[j]._stringValue);
                    if(!_redisClient->del(key, llret)) {
                        llret = 0;
                        emit runError(_taskMsg->_taskid, QString("delete key %1 failed").arg(key));
                        continue;
                    }
                    keyNum += llret;
                }
                _sendMsg._sequence = keyNum;
                emit sendData(_sendMsg);
            } else {
                _cursor = "0";
            }
            _respValue.init();
            _sendMsg._list.clear();
            if(cancle())
                return;
        } while(_cursor.toLongLong());
        if(cancle())
            return;
    }

    destroy();
}

void WorkThread::doBatchScanKeyWork()
{
    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(cancle())
        return;

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    QStringList vKeyPattern = _taskMsg->_keyPattern.split(_taskMsg->_key, QString::SkipEmptyParts);
    for(int i = 0; i < vKeyPattern.size(); ++i) {
        _sendMsg._key = vKeyPattern[i];
        do {
            if(_redisClient->scan(vKeyPattern[i], _respValue, _cursor, BATCH_SCAN_NUM)) {
                _cursor = _respValue._arrayValue[0]._stringValue;
                _sendMsg._sequence = _respValue._arrayValue[1]._arrayValue.size();
                emit sendData(_sendMsg);
            } else {
                _cursor = "0";
            }
            _respValue.init();
            if(cancle())
                return;
        } while(_cursor.toLongLong());
        if(cancle())
            return;
    }

    destroy();
}

void WorkThread::doBatchExportWork(int taskid) {
    if(!prepare()) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(cancle())
        return;

    _thread = _taskMsg->_sequence;
    _dbindex = _taskMsg->_dbIndex;
    _tableName = _taskMsg->_tableName;

    int ret = 0;
    int keyNum = 0;
    int start = 0;
    int stop = 0;
    QString key;
    QByteArray value;
    qlonglong timeout = 0;
    QString cursor = "0";
    ImpExpData stImpExpData;
    std::vector<ImpExpData> vecImpExpData;
    _cursor = "0";
    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();
    QStringList vKeyPattern = _taskMsg->_keyPattern.split(_taskMsg->_key, QString::SkipEmptyParts);
    for(int i = 0; i < vKeyPattern.size(); ++i) {
        _sendMsg._key = vKeyPattern[i];
        do {
            if(_redisClient->scan(vKeyPattern[i], _respValue, _cursor, BATCH_SCAN_NUM)) {
                _cursor = _respValue._arrayValue[0]._stringValue;
                for(int j = 0; j < _respValue._arrayValue[1]._arrayValue.size(); ++j) {
                    key = QString(_respValue._arrayValue[1]._arrayValue[j]._stringValue);
                    if(!_redisClient->type(key, ret)) {
                        emit runError(_taskMsg->_taskid, QString("get key %1 type failed:%2")
                                      .arg(key).arg(_redisClient->getErrorInfo()));
                        continue;
                    }

                    if(!_redisClient->ttl(key, timeout)) {
                        emit runError(_taskMsg->_taskid, QString("get key %1 timeout failed:%2")
                                      .arg(key).arg(_redisClient->getErrorInfo()));
                        stImpExpData.lTimeOut = -2;
                    } else {
                        stImpExpData.lTimeOut = timeout;
                    }

                    cursor = "0";
                    switch(ret) {
                    case KeyType::STRING:
                        if(!_redisClient->get(key, value)) {
                            emit runError(_taskMsg->_taskid, QString("get string key %1 value failed:%2")
                                          .arg(key).arg(_redisClient->getErrorInfo()));
                            break;
                        }

                        stImpExpData.sKey = key;
                        stImpExpData.sKeyType = "string";
                        stImpExpData.sFiled = "";
                        stImpExpData.sValue = QTextCodec::codecForLocale()->toUnicode(value);
                        stImpExpData.lWeight = 0;
                        stImpExpData.sExpDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
                        stImpExpData.iState = 0;
                        vecImpExpData.push_back(stImpExpData);

                        if(vecImpExpData.size() >= 1000) {
                            if(exportData(vecImpExpData, taskid) < 0) {
                                emit runError(_taskMsg->_taskid, _string);
                                destroy();
                                return;
                            }
                            vecImpExpData.clear();
                        }
                        break;
                    case KeyType::LIST:
                        stImpExpData.sKey = key;
                        stImpExpData.sKeyType = "list";
                        stImpExpData.sFiled = "";
                        stImpExpData.iState = 0;
                        _sendMsg._list.clear();
                        start = 0;
                        stop = start + BATCH_SCAN_NUM;
                        _count = 0;

                        do {
                            if(_redisClient->lrange(key, start, stop, _sendMsg._list)) {
                                emit sendData(_sendMsg);
                                for(int k = 0; k < _sendMsg._list.size(); ++k) {
                                    stImpExpData.sValue = QTextCodec::codecForLocale()->toUnicode(_sendMsg._list[k]);
                                    stImpExpData.lWeight = start + k;
                                    stImpExpData.sExpDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
                                    vecImpExpData.push_back(stImpExpData);
                                    if(vecImpExpData.size() >= 1000) {
                                        if(exportData(vecImpExpData, taskid) < 0) {
                                            emit runError(_taskMsg->_taskid, _string);
                                            destroy();
                                            return;
                                        }
                                        vecImpExpData.clear();
                                    }
                                }
                                start = stop + 1;
                                stop = start + BATCH_SCAN_NUM;
                                _count = _sendMsg._list.size();
                            } else {
                                _string = _redisClient->getErrorInfo();
                                emit runError(_taskMsg->_taskid,_string);
                                _count = 0;
                            }
                            _sendMsg._list.clear();
                            if(cancle())
                                return;
                        } while(_count);
                        _count = 0;
                        break;
                    case KeyType::SET:
                        stImpExpData.sKey = key;
                        stImpExpData.sKeyType = "set";
                        stImpExpData.sFiled = "";
                        stImpExpData.lWeight = 0;
                        stImpExpData.iState = 0;

                        do {
                            if(_redisClient->sscan(key, "*", _sendMsg._respResult , cursor, BATCH_SCAN_NUM)) {
                                cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                                for(int m = 0; m < _sendMsg._respResult._arrayValue[1]._arrayValue.size(); ++m) {
                                    stImpExpData.sValue = QTextCodec::codecForLocale()->toUnicode(
                                                _sendMsg._respResult._arrayValue[1]._arrayValue[m]._stringValue);
                                    stImpExpData.sExpDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
                                    vecImpExpData.push_back(stImpExpData);
                                    if(vecImpExpData.size() >= 1000) {
                                        if(exportData(vecImpExpData, taskid) < 0) {
                                            emit runError(_taskMsg->_taskid, _string);
                                            destroy();
                                            return;
                                        }
                                        vecImpExpData.clear();
                                    }
                                }
                            } else {
                                _string = _redisClient->getErrorInfo();
                                emit runError(_taskMsg->_taskid, _string);
                                cursor = "0";
                            }
                            _sendMsg._respResult.init();
                            if(cancle())
                                return;
                        } while(cursor.toLongLong());
                        break;
                    case KeyType::ZSET:
                        stImpExpData.sKey = key;
                        stImpExpData.sKeyType = "zset";
                        stImpExpData.sFiled = "";
                        stImpExpData.iState = 0;

                        do {
                            if(_redisClient->zscan(key, "*", _sendMsg._respResult, cursor, BATCH_SCAN_NUM)) {
                                cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                                for(int p = 0; p < _sendMsg._respResult._arrayValue[1]._arrayValue.size(); ++++p) {
                                    stImpExpData.sValue = QTextCodec::codecForLocale()->toUnicode(
                                                _sendMsg._respResult._arrayValue[1]._arrayValue[p]._stringValue);
                                    stImpExpData.lWeight =
                                            _sendMsg._respResult._arrayValue[1]._arrayValue[p+1]._stringValue.toLong();
                                    stImpExpData.sExpDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
                                    vecImpExpData.push_back(stImpExpData);
                                    if(vecImpExpData.size() >= 1000) {
                                        if(exportData(vecImpExpData, taskid) < 0) {
                                            emit runError(_taskMsg->_taskid, _string);
                                            destroy();
                                            return;
                                        }
                                        vecImpExpData.clear();
                                    }
                                }
                            } else {
                                _string = _redisClient->getErrorInfo();
                                emit runError(_taskMsg->_taskid, _string);
                                cursor = "0";
                            }
                            _sendMsg._respResult.init();
                            if(cancle())
                                return;
                        } while(cursor.toLongLong());
                        break;
                    case KeyType::HASH:
                        stImpExpData.sKey = key;
                        stImpExpData.sKeyType = "hash";
                        stImpExpData.sFiled = "";
                        stImpExpData.lWeight = 0;
                        stImpExpData.iState = 0;

                        do {
                            if(_redisClient->hscan(key, "*", _sendMsg._respResult, cursor, BATCH_SCAN_NUM)) {
                                cursor = _sendMsg._respResult._arrayValue[0]._stringValue;
                                for(int n = 0; n < _sendMsg._respResult._arrayValue[1]._arrayValue.size(); ++++n) {
                                    stImpExpData.sFiled = QTextCodec::codecForLocale()->toUnicode(
                                                _sendMsg._respResult._arrayValue[1]._arrayValue[n]._stringValue);
                                    stImpExpData.sValue = QTextCodec::codecForLocale()->toUnicode(
                                                _sendMsg._respResult._arrayValue[1]._arrayValue[n+1]._stringValue);
                                    stImpExpData.sExpDate = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
                                    vecImpExpData.push_back(stImpExpData);
                                    if(vecImpExpData.size() >= 1000) {
                                        if(exportData(vecImpExpData, taskid) < 0) {
                                            emit runError(_taskMsg->_taskid, _string);
                                            destroy();
                                            return;
                                        }
                                        vecImpExpData.clear();
                                    }
                                }
                            } else {
                                _string = _redisClient->getErrorInfo();
                                emit runError(_taskMsg->_taskid, _string);
                                cursor = "0";
                            }
                            _sendMsg._respResult.init();
                            if(cancle())
                                return;
                        } while(cursor.toLongLong());
                        break;
                    case KeyType::NONE:
                    default:
                        break;
                    }

                    ++keyNum;
                    if(keyNum >= 1000) {
                        _sendMsg._sequence = keyNum;
                        emit sendData(_sendMsg);
                        keyNum = 0;
                    }

                    if(cancle())
                        return;
                }
            } else {
                _cursor = "0";
            }
            _respValue.init();
            if(cancle())
                return;
        } while(_cursor.toLongLong());

        if(vecImpExpData.size() > 0) {
            if(exportData(vecImpExpData, taskid) < 0) {
                emit runError(_taskMsg->_taskid, _string);
                destroy();
                return;
            }
            vecImpExpData.clear();
        }

        _sendMsg._sequence = keyNum;
        emit sendData(_sendMsg);
        keyNum = 0;

        if(cancle())
            return;
    }

    destroy();
}

void WorkThread::doBatchImportWork(int taskid) {

    if(!prepare(WORK_THREAD_MODE1, _taskMsg->_clusterMode, _taskMsg->_customMode)) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(cancle(WORK_THREAD_MODE1))
        return;

    _thread = _taskMsg->_sequence;
    _dbindex = _taskMsg->_dbIndex;
    _tableName = _taskMsg->_tableName;
    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();

    int ret = imporData(taskid);
    if(ret < 0) {
        emit runError(_taskMsg->_taskid,_string);
    } else if(ret > 0) {
        return;
    }

    destroy(WORK_THREAD_MODE1);
}

void WorkThread::doBatchDbDeleWork(int taskid) {
    if(!prepare(WORK_THREAD_MODE1, _taskMsg->_clusterMode, _taskMsg->_customMode)) {
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(cancle(WORK_THREAD_MODE1))
        return;

    _thread = _taskMsg->_sequence;
    _dbindex = _taskMsg->_dbIndex;
    _tableName = _taskMsg->_tableName;
    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();

    int ret = deleteData(taskid);
    if(ret < 0) {
        emit runError(_taskMsg->_taskid,_string);
        destroy(WORK_THREAD_MODE1);
        return;
    } else if(ret > 0) {
        return;
    }

    destroy(WORK_THREAD_MODE1);
}

