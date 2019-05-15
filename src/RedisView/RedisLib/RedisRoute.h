/**
* @file      RedisRoute.cpp
* @brief     REDIS键值路由类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef REDISROUTE_H
#define REDISROUTE_H

#include "RedisLib/RedisDefine.h"

class RedisRoute
{
public:
    RedisRoute();
    // 获取key对应槽
    unsigned int getKeySlot(char *key, int keylen);
    // 分析str字符串，从kpos位置取key[kpos最小1开始]，从vpos位置取value[vpos最小1开始]
    // kskip与vskip标识取值时跳过个数,kskip、vskip为0不跳过全取，小余0不取，大于0跳过取
    bool getKey(const QString & str, QList<ClusterCmdRoute> & cmd,
                int kpos = 1, int kskip = -1, int vpos = 1, int vskip = -1);
    // 分析str字符串，从pos位置取key[pos最小1开始]，end为key后命令结束标识[0未结束，1结束，2可结束]
    bool getKey(const QString & str, QString &key, int pos =1, int end = 1);
    // 获取随机KEY
    QString getRandKey();
private:
    unsigned short crc16(const char *buf, int len);
    static const unsigned short _crc16tab[256];
    QString _strBuffer;
    QList<QString> _strList;
};

#endif // REDISROUTE_H
