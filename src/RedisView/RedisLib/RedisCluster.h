/**
* @file      RedisCluster.h
* @brief     REDIS集群客户端类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef REDISCLUSTER_H
#define REDISCLUSTER_H
#include "RedisLib/RedisClient.h"
#include "RedisLib/RedisRoute.h"

class RedisCluster : public RedisRoute
{
public:

    RedisCluster();
    ~RedisCluster();
    bool isOpen();
    bool openSingel(const QString &hostAddress, const QString passwd = "", int timeOut = 1000);
    bool openCluster(const QString &hostAddress, const QString passwd = "", const bool master = true, int timeOut = 1000);
    bool openCluster();
    bool reOpen();
    void close();
    void setCluster(const bool &isCluster);
    bool getCluster() const;
    void setTimeOutMs(unsigned int iTimeOutMs);
    unsigned int getTimeOutMs() const;
    QString getErrorInfo() const;
    QList<QByteArray> command(QString &str, const QString &split = "", int index = -1);
    QList<QByteArray> clusterCommand(QString &str);
    QByteArray singleCommand(QString &str, int index = -1);
    RedisClient * getClient(int index = 0);
    RedisClient * getClient(QByteArray &key);
    RedisClient * getClient(const QString &key);
    bool parseResp(const QByteArray &data, RespType &rResult);
    QString getConnectName() const;
    void setConnectName(const QString &ConnectName);
    void formatToResp(const RespType &inResp, QByteArray &outResp, int spaceNum);
    void formatToResp(const QByteArray &inResp, QByteArray &outResp, int spaceNum);
    void formatToText(const RespType &inResp, QByteArray &outResp, int spaceNum);
    void formatToText(const QByteArray &inResp, QByteArray &outResp, int spaceNum);
    void formatToJson(const RespType &inResp, QJsonArray &outJson);
    void formatToJson(const QByteArray &inResp, QJsonArray &outJson);
    bool copyNewClients(QList<ClusterClient> &vClients);
    QList<ClusterClient> getClients(bool isMaster = true) const;
    bool persist(const QString &key);
    bool select(const int &index);
    bool renamex(const QString &key, const QString &newkey);
    bool get(const QString &key, QByteArray &value);
    bool set(const QString &key, const QString &value);
    bool del(const QString &key, qlonglong & llRet);
    bool hset(const QString &key, const QString &field, const QString &value, qlonglong & llRet);
    bool pttl(const QString &key, qlonglong & mseconds);
    bool pexpire(const QString &key, uint mseconds);
    bool type(const QString &key, QByteArray &value);
    bool zadd(const QString &key, const QString &member, const double &score, qlonglong & llRet);
    bool sadd(const QString &key, const QString &member, qlonglong & llRet);
    bool lpush(const QString &key, const QString &value, qlonglong & llRet);
    bool refcount(const QString& key, qlonglong & value);
    bool idletime(const QString& key, qlonglong & value);
    bool encoding(const QString& key, QByteArray & value);
    int getClientIndex() const;
    bool getClusterMode() const;
    bool getReplicationMode() const;
    bool getcfg(const QString& parameter, RespType &value);
    bool getDbNum(int &num);
    bool dbsize(qlonglong & llRet);

private:

    bool _isNewOpen;
    bool _isClusterMode;
    bool _isReplicationMode;
    bool _onlyMaster;
    quint16 _port;
    int _dbIndex;
    int _timeOut;
    int _iClientIndex;
    unsigned int _iTimeOut;
    QString _host;
    QString _passwd;
    QString _sErrorInfo;
    QString _ConnectName;
    QString _hostAddress;
    QByteArray _byteArray;
    RedisClient * _redisClient;
    QList<ClusterClient> _vClusterClients;
    QList<ClusterClient> _vClusterMasterClients;
    RespType _respType;
};

#endif // REDISCLUSTER_H
