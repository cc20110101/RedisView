/**
* @file      WorkThread.h
* @brief     线程工作类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include "Public/Define.h"

class WorkThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit WorkThread(TaskMsg *taskMsg = nullptr, QObject *parent = nullptr);
    explicit WorkThread(QList<CmdMsg> &cmd, TaskMsg *taskMsg = nullptr, QObject *parent = nullptr);

protected:
    void run();

private:
    int _taskid;
    TaskMsg *_taskMsg;
    TaskMsg _sendMsg;
    RedisClient *_redisClient;
    RedisCluster *_redisClusterClient;
    QString _string;
    QByteArray _byteArray;
    RespType _respValue;
    qulonglong _cursor;
    qulonglong _count;
    QList<CmdMsg> _cmd;

signals:
    //注意！要使用信号，采用QObejct 和 QRunnable多继承，记得QObject要放在前面
    void finishWork(const int taskid);
    void runError(const int taskid, const QString & error);
    void sendData(const TaskMsg taskMsg);

public slots:
    void cancelWork(const int taskid);
    void doKeyListWork();
    void doValueListWork();
    void doCommitValueWork();
    void doDelKeyWork();
    void doBatchDelKeyWork();
};

#endif // WORKTHREAD_H
