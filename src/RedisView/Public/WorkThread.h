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
    void doKeyListWork();
    void doValueListWork();
    void doCommitValueWork();
    void doDelKeyWork();

};

#endif // WORKTHREAD_H
