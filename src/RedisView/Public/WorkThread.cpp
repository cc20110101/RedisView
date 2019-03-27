#include "Public/WorkThread.h"

WorkThread::WorkThread(TaskMsg *taskMsg, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TaskMsg>();
    setAutoDelete(true);
    _taskMsg = taskMsg;
    _string.clear();
    _respValue.init();
    _sendMsg.init();
    _cursor = 0;
    _taskid = 0;
}

WorkThread::WorkThread(QList<CmdMsg> &cmd, TaskMsg *taskMsg, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TaskMsg>();
    qRegisterMetaType<CmdMsg>();
    setAutoDelete(true);
    _cmd = cmd;
    _taskMsg = taskMsg;
    _string.clear();
    _respValue.init();
    _sendMsg.init();
    _cursor = 0;
    _taskid = 0;
}

void WorkThread::cancelWork(const int taskid) {
    _taskid = taskid;
}

void WorkThread::doKeyListWork()
{
    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _redisClient = new RedisClient();
    if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();
    do {
        if(_redisClient->scan(_sendMsg._keyPattern, _sendMsg._respResult, _cursor)) {
            _cursor = _sendMsg._respResult._arrayValue[0]._stringValue.toLongLong();
            emit sendData(_sendMsg);
        } else {
            _cursor = 0;
        }
        _sendMsg._respResult.init();
    } while(_cursor);

    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    delete _redisClient;
    _redisClient = nullptr;
}

void WorkThread::doValueListWork() {
    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _redisClient = new RedisClient();
    if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    _sendMsg._respResult.init();
    if(_taskMsg->_type == "hash") {
        do {
            if(_redisClient->hscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult, _cursor)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue.toLongLong();
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = 0;
            }
            _sendMsg._respResult.init();
        } while(_cursor);
    } else if(_taskMsg->_type == "zset") {
        do {
            if(_redisClient->zscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult, _cursor)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue.toLongLong();
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = 0;
            }
            _sendMsg._respResult.init();
        } while(_cursor);
    } else if(_taskMsg->_type == "set") {
        do {
            if(_redisClient->sscan(_sendMsg._key, _sendMsg._keyPattern, _sendMsg._respResult , _cursor)) {
                _cursor = _sendMsg._respResult._arrayValue[0]._stringValue.toLongLong();
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid, _string);
                _cursor = 0;
            }
            _sendMsg._respResult.init();
        } while(_cursor);
    } else if(_taskMsg->_type == "list") {
        int start = 0;
        int stop = start + 1000;
        do {
            if(_redisClient->lrange(_sendMsg._key, start, stop, _sendMsg._list)) {
                emit sendData(_sendMsg);
                start = stop + 1;
                stop = start + 1000;
                _cursor = _sendMsg._list.size();
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                _cursor = 0;
            }
            _sendMsg._list.clear();
        } while(_cursor);
    } else if(_taskMsg->_type == "string") {
        if(_redisClient->get(_sendMsg._key, _byteArray)) {
            _sendMsg._list << _byteArray;
            emit sendData(_sendMsg);
        } else {
            _string = _redisClient->getErrorInfo();
            emit runError(_taskMsg->_taskid,_string);
        }
        _sendMsg._list.clear();
    }

    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    delete _redisClient;
    _redisClient = nullptr;
}

void WorkThread::doCommitValueWork() {

    if(_cmd.size() <= 0) {
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _redisClient = new RedisClient();
    if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    qlonglong llret;
    for(int i =0; i < _cmd.size(); ++i) {
        switch (_cmd[i]._operate) {
        case 1:
            if(_cmd[i]._type == "hash") {
                if(!_redisClient->hset(_cmd[i]._key,_cmd[i]._filed,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "zset") {
                if(!_redisClient->zadd(_cmd[i]._key,_cmd[i]._value,_cmd[i]._score,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "set") {
                if(!_redisClient->sadd(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "list") {
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
        case 2:
            if(_cmd[i]._type == "hash") {
                if(!_redisClient->hdel(_cmd[i]._key,_cmd[i]._filed,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "zset") {
                if(!_redisClient->zrem(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "set") {
                if(!_redisClient->srem(_cmd[i]._key,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "list") {
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
        case 3:
            if(_cmd[i]._type == "hash") {
                if(!_redisClient->hset(_cmd[i]._key,_cmd[i]._filed,_cmd[i]._value,llret)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "string") {
                if(!_redisClient->set(_cmd[i]._key,_cmd[i]._value)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            } else if(_cmd[i]._type == "list") {
                if(!_redisClient->lset(_cmd[i]._key,_cmd[i]._valueIndex,_cmd[i]._value)) {
                    _string = _redisClient->getErrorInfo();
                    emit runError(_taskMsg->_taskid,_string);
                    continue;
                }
            }
            break;
        case 4:
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
        case 5:
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

    _cmd.clear();
    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    delete _redisClient;
    _redisClient = nullptr;
}

void WorkThread::doDelKeyWork() {
    if(_cmd.size() <= 0) {
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_clientIndex) {
        _redisClusterClient = new RedisCluster();
        if(!_redisClusterClient->openCluster(QString("%1:%2").
                                             arg(_taskMsg->_host).
                                             arg(_taskMsg->_port),
                                             _taskMsg->_passwd)) {
            _string = "connect host failed";
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            _redisClusterClient->close();
            delete _redisClusterClient;
            _redisClusterClient = nullptr;
            return;
        }

        qlonglong qllRet;
        for(int i =0; i < _cmd.size(); ++i) {
            if(!_redisClusterClient->del(_cmd[i]._key, qllRet)) {
                _string = "delete key failed";
                emit runError(_taskMsg->_taskid,_string);
                continue;
            }
        }
        _redisClusterClient->close();
        delete _redisClusterClient;
        _redisClusterClient = nullptr;
    } else {
        _redisClient = new RedisClient();
        if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
            _string = "connect host failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
        if(!_taskMsg->_passwd.isEmpty()) {
            if(!_redisClient->auth(_taskMsg->_passwd)) {
                _string = "connect host auth failed";
                _redisClient->close();
                emit runError(_taskMsg->_taskid,_string);
                emit finishWork(_taskMsg->_taskid);
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                return;
            }
        }

        if(!_redisClient->isOpen()) {
            _string = "connect host failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            return;
        }

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
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
    }

    _cmd.clear();
    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
}

void WorkThread::doBatchDelKeyWork() {
    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _redisClient = new RedisClient();
    if(!_redisClient->open(_taskMsg->_host, _taskMsg->_port)) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit runError(_taskMsg->_taskid,_string);
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit runError(_taskMsg->_taskid,_string);
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    if(_taskid == _taskMsg->_taskid) {
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
        emit finishWork(_taskMsg->_taskid);
        return;
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    qlonglong llret;
    QString key;
    int keyNum;
    QStringList vKeyPattern = _taskMsg->_keyPattern.split(_taskMsg->_key, QString::SkipEmptyParts);
    for(int i = 0; i < vKeyPattern.size(); ++i) {
        _sendMsg._key = vKeyPattern[i];
        do {
            if(_redisClient->scan(vKeyPattern[i], _respValue, _cursor)) {
                _cursor = _respValue._arrayValue[0]._stringValue.toLongLong();
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
                _cursor = 0;
            }
            _respValue.init();
            _sendMsg._list.clear();
            if(_taskid == _taskMsg->_taskid) {
                _redisClient->close();
                delete _redisClient;
                _redisClient = nullptr;
                emit finishWork(_taskMsg->_taskid);
                return;
            }
        } while(_cursor);
        if(_taskid == _taskMsg->_taskid) {
            _redisClient->close();
            delete _redisClient;
            _redisClient = nullptr;
            emit finishWork(_taskMsg->_taskid);
            return;
        }
    }

    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    delete _redisClient;
    _redisClient = nullptr;
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
    }
}
