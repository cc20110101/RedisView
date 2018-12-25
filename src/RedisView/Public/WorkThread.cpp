#include "WorkThread.h"

WorkThread::WorkThread(TaskMsg *taskMsg, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<TaskMsg>();
    setAutoDelete(true);
    _taskMsg = taskMsg;
    _string.clear();
    _respValue.init();
    _sendMsg.init();
    _cursor = 0;
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
}

void WorkThread::doKeyListWork()
{
    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    _redisClient = new RedisClient();
    _redisClient->open(_taskMsg->_host, _taskMsg->_port);
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            return;
        }
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    do {
        if(_redisClient->scan("*", _respValue, _cursor)) {
            _cursor = _respValue._arrayValue[0]._stringValue.toLongLong();
            for(int i = 0; i < _respValue._arrayValue[1]._arrayValue.size(); ++i) {
                _sendMsg._list << _respValue._arrayValue[1]._arrayValue[i]._stringValue;
            }
            emit sendData(_sendMsg);
        } else {
            _cursor = 0;
        }
        _respValue.init();
        _sendMsg._list.clear();
    } while(_cursor);

    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    _redisClient = nullptr;
}

void WorkThread::doValueListWork() {
    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    _redisClient = new RedisClient();
    _redisClient->open(_taskMsg->_host, _taskMsg->_port);
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            return;
        }
    }

    _sendMsg = *_taskMsg;
    _sendMsg._list.clear();
    if(_taskMsg->_type == "hash") {
        do {
            if(_redisClient->hscan(_taskMsg->_key,"*", _respValue, _cursor)) {
                _cursor = _respValue._arrayValue[0]._stringValue.toLongLong();
                for(int i = 0; i < _respValue._arrayValue[1]._arrayValue.size(); ++i) {
                    _sendMsg._list << _respValue._arrayValue[1]._arrayValue[i]._stringValue;
                }
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                _cursor = 0;
            }
            _respValue.init();
            _sendMsg._list.clear();
        } while(_cursor);
    } else if(_taskMsg->_type == "zset") {
        do {
            if(_redisClient->zscan(_taskMsg->_key,"*", _respValue, _cursor)) {
                _cursor = _respValue._arrayValue[0]._stringValue.toLongLong();
                for(int i = 0; i < _respValue._arrayValue[1]._arrayValue.size(); ++i) {
                    _sendMsg._list << _respValue._arrayValue[1]._arrayValue[i]._stringValue;
                }
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                _cursor = 0;
            }
            _respValue.init();
            _sendMsg._list.clear();
        } while(_cursor);
    } else if(_taskMsg->_type == "set") {
        do {
            if(_redisClient->sscan(_taskMsg->_key,"*", _respValue, _cursor)) {
                _cursor = _respValue._arrayValue[0]._stringValue.toLongLong();
                for(int i = 0; i < _respValue._arrayValue[1]._arrayValue.size(); ++i) {
                    _sendMsg._list << _respValue._arrayValue[1]._arrayValue[i]._stringValue;
                }
                emit sendData(_sendMsg);
            } else {
                _string = _redisClient->getErrorInfo();
                emit runError(_taskMsg->_taskid,_string);
                _cursor = 0;
            }
            _respValue.init();
            _sendMsg._list.clear();
        } while(_cursor);
    } else if(_taskMsg->_type == "list") {
        int start = 0;
        int stop = start + 1000;
        do {
            if(_redisClient->lrange(_taskMsg->_key, start, stop, _sendMsg._list)) {
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
        if(_redisClient->get(_taskMsg->_key, _byteArray)) {
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
    _redisClient = nullptr;
}

void WorkThread::doCommitValueWork() {

    if(_cmd.size() <= 0)
        return;

    _string.clear();
    if(_taskMsg == nullptr) {
        _string = "TaskMsg is null";
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    _redisClient = new RedisClient();
    _redisClient->open(_taskMsg->_host, _taskMsg->_port);
    if(!_taskMsg->_passwd.isEmpty()) {
        if(!_redisClient->auth(_taskMsg->_passwd)) {
            _string = "connect host auth failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
            return;
        }
    }

    if(!_redisClient->isOpen()) {
        _string = "connect host failed";
        _redisClient->close();
        emit runError(_taskMsg->_taskid,_string);
        return;
    }

    if(_taskMsg->_dbIndex > 0) {
        if(!_redisClient->select(_taskMsg->_dbIndex)) {
            _string = "select db failed";
            _redisClient->close();
            emit runError(_taskMsg->_taskid,_string);
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

    emit finishWork(_taskMsg->_taskid);
    delete _taskMsg;
    _taskMsg = nullptr;
    _redisClient->close();
    _redisClient = nullptr;
}

void WorkThread::run() {
    if(_taskMsg->_taskid == THREAD_SCAN_KEY_TASK) {
        doKeyListWork();
    } else if(_taskMsg->_taskid == THREAD_SCAN_VALUE_TASK) {
        doValueListWork();
    } else if(_taskMsg->_taskid == THREAD_COMMIT_VALUE_TASK) {
        doCommitValueWork();
    }
}
