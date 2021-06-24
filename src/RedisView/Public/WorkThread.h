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

#include "Public/DbMgr.h"
#include "Public/Publib.h"

class WorkThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit WorkThread(TaskMsg *taskMsg = nullptr, QObject *parent = nullptr);
    explicit WorkThread(QList<CmdMsg> &cmd, TaskMsg *taskMsg = nullptr, QObject *parent = nullptr);

protected:
    void run();

private:
    bool prepare(int mode, bool clusterMode, bool customMode);
    bool prepare(int mode = WORK_THREAD_MODE0, int cluster = 0);
    void destroy(int mode = WORK_THREAD_MODE0);
    bool cancle(int mode = WORK_THREAD_MODE0);
    int exportData(std::vector<ImpExpData> &vImpExpData, int taskid);
    int imporData(int taskid);
    int deleteData(int taskid);

private:
    int _taskid;
    int _thread;
    int _dbindex;
    TaskMsg *_taskMsg;
    TaskMsg _sendMsg;
    RedisClient *_redisClient;
    RedisCluster *_redisClusterClient;
    QString _string;
    QString _tableName;
    QString _sql;
    QByteArray _byteArray;
    RespType _respValue;
    QString _cursor;
    qulonglong _count;
    QList<CmdMsg> _cmd;
    DbMgr _dbMgr;
    QSqlDatabase *_db;

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
    void doBatchScanKeyWork();
    void doBatchExportWork(int taskid);
    void doBatchDbDeleWork(int taskid);
    void doBatchImportWork(int taskid);
};

#endif // WORKTHREAD_H
