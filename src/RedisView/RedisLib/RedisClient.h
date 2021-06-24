/**
* @file      RedisClient.h
* @brief     REDIS客户端类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include "RedisLib/RedisTransMgr.h"

/**
 * Redis内存数据库操作客户端
 * 提供基本命令操作
 */
class RedisClient : public QObject, public RedisRespParser
{
    Q_OBJECT

public:
    RedisClient(const QString &hostAddress, int port, QObject *parent = nullptr);
    RedisClient(QObject *parent = nullptr);

    QString getHostAddress();
    void setHostAddress(const QString &hostAddress);
    void setPort(int port);
    int getPort();
    bool isOpen();
    bool open(int timeOut = 1000);
    bool open(const QString &hostAddress, quint16 port, int timeOut = 1000);
    void close();
    bool getSubModel() const;
    void setSubModel(bool bSubModel);

    /**
     * 设置执行命令等待结果超时时间
     * @param[in]    iTimeOutMs 毫秒超时时间，0阻塞
     * @return
     * @see
     * @note
     */
    void setTimeOutMs(int iTimeOutMs);

    /**
     * 获取执行命令等待结果超时时间
     * @return    毫秒超时时间
     * @see
     * @note
     */
    int getTimeOutMs();

    /**
     * 返回上次错误信息
     * @return    上次错误信息
     * @see
     * @note
     */
    QString getErrorInfo();

    /**
     * 发送REDIS命令到服务端
     * @param[in]    str 命令
     * @return       服务端RESP协议返回值
     * @see
     * @note
     */
    QByteArray command(const QString &str);

    /**
     * 发送REDIS命令到服务端
     * @param[in]    list 命令
     * @return       服务端RESP协议返回值
     * @see
     * @note
     */
    QByteArray command(const QList<QString> &list);

    /**
     * 发送REDIS命令到服务端
     * @param[in]    list 命令
     * @return       执行是否成功
     * @see
     * @note
     */
    bool command(const QList<QString> &list, QByteArray &result);

    /**
     * 解析命令为REDIS协议格式
     * @param[in]    str 命令
     * @return       命令RESP协议格式
     * @see
     * @note
     */
    QByteArray getRespCmd(const QString &str);

    /**
     * 将信息message发送到指定的频道channel
     * @param[in]    channel 频道
     * @param[in]    message 消息
     * @param[out]   llRet   接收到信息 message 的订阅者数量
     * @return       true成功,false失败
     * @see
     * @note
     */
    bool publish(const QString &channel, const QString &message, qlonglong &llRet);

    /**
     * 订阅一个或多个频道
     * @param[in]    pattern 一个或多个频道
     * @return
     * @see
     * @note
     */
    void subscribe(const QString &channel);

    /**
     * 指示客户端退订给定的频道,如果没有频道被指定，也即是，一个无参数的UNSUBSCRIBE调用被执行，
     * 那么客户端使用SUBSCRIBE命令订阅的所有频道都会被退订。在这种情况下，命令会返回一个信息，
     * 告知客户端所有被退订的频道
     * @param[in]    pattern 一个或多个模式
     * @return
     * @see
     * @note
     */
    void unsubscribe(const QString &channel);

    /**
     * 订阅一个或多个符合给定模式的频道。每个模式以*作为匹配符，比如it*匹配所有以it开头的频道
     * @param[in]    pattern 一个或多个模式
     * @return
     * @see
     * @note
     */
    void psubscribe(const QString &pattern);

    /**
     * 指示客户端退订所有给定模式，如果没有模式被指定，也即是，一个无参数的PUNSUBSCRIBE调用被执行，
     * 那么客户端使用 PSUBSCRIBE 命令订阅的所有模式都会被退订。在这种情况下，命令会返回一个信息，
     * 告知客户端所有被退订的模式
     * @param[in]     pattern 一个或多个模式
     * @return
     * @see
     * @note
     */
    void punsubscribe(const QString &pattern);

    /**
     * 是一个查看订阅与发布系统状态的内省命令
     * @param[in]     subcommand 子命令
     * @param[in]     argument   子命令参数
     * @param[out]    value      返回值
     * @return        true成功,false失败
     * @see
     * @note
     */
    bool pubsub(const QString &subcommand, const QString &argument, RespType &value);

    /**
     * 如果key已经存在并且是一个字符串，APPEND命令将value追加到key原来的值的末尾
     * 如果key不存在，APPEND就简单地将给定key设为value，就像执行SET key value一样
     * @param[in]    key 一个或多个键
     * @param[in]    value 追加的值
     * @return       追加value之后，key中字符串的长度
     * @see
     * @note
     */
    qlonglong append(const QString &key, const QString &value);

    /**
     * 判断是否是集群模式
     * @param[out]    value 是否集群模式
     * @return        成功是ture，否false
     * @see
     * @note
     */
    bool isCluster(bool & value);

    /**
     * 获取复制集参数信息
     * @param[out]    infoMap复制集信息
     * @return        成功是ture，否false
     * @see
     * @note
     */
    bool getReplicationInfo(QMap<QString,QString> &infoMap);

    /**
     * 获取Redis信息
     * @param[out]    infoMap信息
     * @return        成功是ture，否false
     * @see
     * @note
     */
    bool getRedisInfo(QMap<QString,QString> &infoMap);

    /**
     * 判断是否是集群模式下主节点
     * @param[out]    value 是否集群主
     * @return       是ture，否false
     * @see
     * @note
     */
    bool isMaster(bool & value);

    /**
     * 获取集群模式节点信息
     * @param[out]    value 集群节点信息
     * @return       是ture，否false
     * @see
     * @note
     */
    bool getClusterNodes(QByteArray &value);

    /**
     * 获取集群槽信息
     * @param[out]    value 集群槽信息
     * @return       是ture，否false
     * @see
     * @note
     */
    bool getClusterSlots(RespType &value);

    /**
     * 如果AUTH命令给定的密码password和配置文件中的密码相符的话，服务器会返回OK并开始接受命令输入
     * @param[in]    password 认证密码
     * @return       成功ture，失败false
     * @see
     * @note
     */
    bool auth(const QString &password);

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
    bool decr(const QString &key, qlonglong & llRet);

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
    bool decrby(const QString &key, int interval, qlonglong & llRet);

    /**
     * 删除给定的一个key，不存在的key会被忽略
     * @param[in]    key 一个键
     * @param[out]   llRet 删除数量
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool del(const QString &key, qlonglong & llRet);

    /**
     * 返回当前数据库的key的数量
     * @param[in]    key 一个或多个键
     * @param[out]   llRet 键数量
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool dbsize(qlonglong & llRet);

    /**
     * 序列化给定key，并返回被序列化的值
     * @param[in]    key 字符串键
     * @param[out]   value 序列化后值
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool dump(const QString &key, QByteArray & value);

    /**
     * 检查给定key是否存在
     * @param[in]    key 键
     * @return       存在rue,不存在false
     * @see
     * @note
     */
    bool exists(const QString &key);

    /**
     * 为key设置生存时间，命令接受的时间参数秒
     * 当key不存在或没办法设置生存时间，返回0
     * @param[in]    key 键
     * @param[in]    seconds 生存秒
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool expire(const QString &key, uint seconds);

    /**
     * 为key设置生存时间，命令接受的时间参数是UNIX时间戳，如果生存时间设置成功，返回1
     * 当key不存在或没办法设置生存时间，返回0
     * @param[in]    key 键
     * @param[in]    timestamp UNIX秒时间戳
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool expireat(const QString &key, uint timestamp);

    /**
     * 返回key所关联的字符串值。如果key不存在那么返回特殊值nil
     * @param[in]    key 键
     * @param[out]   value 值
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool get(const QString &key, QByteArray &value);

    /**
     *返回key中字符串值的子字符串，字符串的截取范围由start和stop两个偏移量决定(包括start和stop在内)。
     *负数偏移量表示从字符串最后开始计数， -1表示最后一个字符， -2表示倒数第二个，以此类推。
     * @param[in]    key 字符串键
     * @param[in]    start 字符串开始位置
     * @param[in]    stop 字符串结束位置
     * @param[out]   value 子字符串值
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool getrange(const QString & key, int start, int stop, QByteArray &value);

    /**
     *删除哈希表key中的一个或多个指定域，不存在的域将被忽略
     * @param[in]    key 哈希键
     * @param[in]    field 哈希字段
     * @param[out]   llRet 被成功移除的域的数量，不包括被忽略的域
     * @return       存在rue,不存在false
     * @see
     * @note
     */
    bool hdel(const QString &key, const QString &field, qlonglong & llRet);

    /**
     *查看哈希表key中，给定域field是否存在
     * @param[in]    key 哈希键
     * @param[in]    field 哈希字段
     * @return       存在rue,不存在false
     * @see
     * @note
     */
    bool hexists(const QString &key, const QString &field);

    /**
     *返回哈希表key中给定域field的值。
     * @param[in]    key 哈希键
     * @param[in]    field 哈希字段
     * @param[in]    value 哈希字段值
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hget(const QString &key, const QString &field, QByteArray &value);

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
    bool hincrby(const QString &key, const QString &field, int increment, qlonglong & llRet);

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
    bool hincrbyfloat(const QString &key, const QString &field, double increment, double & fRet);

    /**
     *返回哈希表key中域的数量。当 key 不存在时，返回 0 。
     * @param[in]    key 哈希键值
     * @param[out]   llRet 哈希域长度
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hlen(const QString &key, qlonglong & llRet);

    /**
     *同时将多个field-value(域-值)对设置到哈希表key中。
     *此命令会覆盖哈希表中已存在的域。
     *如果key不存在，一个空哈希表被创建并执行HMSET操作。
     * @param[in]    key 哈希键值
     * @param[in]    map 插入哈希字段与值
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hmset(const QString &key, QMap<QString, QVariant> & map);

    /**
     *将哈希表key中的域field的值设为value 。
     *如果key不存在，一个新的哈希表被创建并进行HSET操作。
     *如果域field已经存在于哈希表中，旧值将被覆盖。
     * @param[in]    key 哈希键
     * @param[in]    field 哈希字段
     * @param[in]    value 哈希字段值
     * @param[out]   llRet 如果field是哈希表中的一个新建域，并且值设置成功，值为1。
     *               如果哈希表中域field已经存在且旧值已被新值覆盖，值为0。
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool hset(const QString &key, const QString &field, const QString &value, qlonglong & llRet);

    /**
     *将哈希表key中的域field的值设置为value，当且仅当域field不存在。
     *若域field 已经存在，该操作无效。
     *如果 key 不存在，一个新哈希表被创建并执行 HSETNX 命令。
     * @param[in]    key 哈希键
     * @param[in]    field 哈希字段
     * @param[in]    value 哈希字段值
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool hsetnx(const QString &key, const QString &field, const QString &value);

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
    bool incr(const QString &key, qlonglong & llRet);

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
    bool incrby(const QString &key, int interval, qlonglong & llRet);

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
    bool lindex(const QString &key, qlonglong index, QByteArray &value);

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
    bool linsert(const QString &key, const int position, const QString &pivot, const QString &value, qlonglong & llRet);

    /**
     * 链表长度
     * @param[in]    key 链表键值
     * @param[out]   llRet 链表长度
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool llen(const QString &key, qlonglong & llRet);

    /**
     * 移除并返回列表key的头元素，当key不存在时，返回nil
     * @param[in]    key 链表键值
     * @param[out]   value 链表返回元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool lpop(const QString &key, QByteArray &value);

    /**
     * 将一个或多个值value插入到列表key的表头
     * @param[in]    key 链表键值
     * @param[in]    value 链表添加一个或多个键值
     * @param[out]   llRet 链表长度
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool lpush(const QString &key, const QString &value, qlonglong & llRet);

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
    bool lpushx(const QString &key, const QString &value, qlonglong & llRet);

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
    bool lrem(const QString &key, int count, const QString &value, qlonglong & llRet);

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
    bool lset(const QString &key, int index, const QString &value);

    /**
     * 让列表只保留指定区间内的元素，不在指定区间之内的元素都将被删除
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
     */
    bool ltrim(const QString &key, int start, int stop);

    /**
     * 将当前数据库的key移动到给定的数据库db当中
     * 如果当前数据库(源数据库)和给定数据库(目标数据库)有相同名字的给定key，
     * 或者key不存在于当前数据库，那么MOVE没有任何效果
     * @param[in]    key 键值
     * @param[in]    database 目的数据库
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool move(const QString &key, int database);

    /**
     * 同时设置一个或多个key-value对
     * 如果某个给定key已经存在，那么MSET会用新值覆盖原来的旧值
     * @param[in]    map 键值对
     * @return       成功true,失败false
     * @see
     * @note         MSET是一个原子性(atomic)操作，所有给定key都会在同一时间内被设置，
     *               某些给定key被更新而另一些给定key没有改变的情况，不可能发生
     */
    bool mset(QMap<QString, QVariant> & map);

    /**
     * 移除给定 key 的生存时间,如果key不存在或key没有设置生存时间，返回0
     * @param[in]    key 键
     * @return       成功true,失败false
     * @see
     * @note
     *
     */
    bool persist(const QString &key);

    /**
     * 为key设置生存时间，命令接受的时间参数毫秒
     * 当key不存在或没办法设置生存时间，返回0
     * @param[in]    key 键
     * @param[in]    mseconds 生存豪秒
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool pexpire(const QString &key, uint mseconds);

    /**
     * 为key设置生存时间，毫秒为单位设置key的过期unix时间戳
     * 当key不存在或没办法设置生存时间，返回0
     * @param[in]    key 键
     * @param[in]    mstimestamp UNIX毫秒时间戳
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool pexpireat(const QString &key, uint mstimestamp);

    /**
     * 以毫秒为单位返回key的剩余生存时间
     * @param[in]    key 键
     * @param[out]   mseconds 当key不存在时，为-2。当key存在但没有设置剩余生存时间时，
     *              值-1，否则值为key的剩余生存时间
     * @return      设置成功rue,设置失败false
     * @see
     * @note
     */
    bool pttl(const QString &key, qlonglong & mseconds);

    /**
     * 设置字符串key的键值
     * @param[in]    key 键
     * @param[in]    value 值
     * @return       设置成功rue,设置失败false
     * @see
     * @note
     */
    bool set(const QString &key, const QString &value);

    /**
     * 以秒为单位返回key的剩余生存时间
     * @param[in]    key 键
     * @param[out]   seconds 当key不存在时，为-2。当key存在但没有设置剩余生存时间时，
     *              值-1，否则值为key的剩余生存时间
     * @return      设置成功rue,设置失败false
     * @see
     * @note
     */
    bool ttl(const QString &key, qlonglong & seconds);

    /**
     * 返回key所储存的值的类型
     * @param[in]    key 键
     * @param[out]   value 键类型，none，string，list，set，zset，hash
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool type(const QString &key, QByteArray &value);

    /**
     * 返回key所储存的值的类型
     * @param[in]    key 键
     * @param[out]   value 键类型，none 0，string 1，list 2，set 3，zset 4，hash 5
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool type(const QString &key, int & value);

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
    bool rename(const QString &key, const QString &newkey);

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
    bool renamex(const QString &key, const QString &newkey);

    /**
     * 移除并返回列表key的尾元素，当key不存在时，返回nil
     * @param[in]    key 链表键值
     * @param[out]   value 链表返回元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool rpop(const QString &key, QByteArray &value);

    /**
     * 将一个或多个值value插入到列表key的表尾,如果key不存在，一个空列表会被创建并执行RPUSH操作
     * @param[in]    key 链表键值
     * @param[in]    value 链表添加一个或多个键值
     * @param[out]   llRet 链表长度
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool rpush(const QString &key, const QString &value, qlonglong & llRet);

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
    bool rpushx(const QString &key, const QString &value, qlonglong & llRet);

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
    bool rpoplpush(const QString &source, const QString &destination, QByteArray &value);

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
    bool sadd(const QString &key, const QString &member, qlonglong & llRet);

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
    bool zadd(const QString &key, const QString &member, const double &score, qlonglong & llRet);

    /**
     * 返回集合key的基数(集合中元素的数量),当key不存在时，返回0
     * @param[in]    key 集合键
     * @param[out]   llRet 集合基数
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool scard(const QString &key, qlonglong & llRet);

    /**
     * 移除并返回集合中的一个随机元素,当key不存在或key是空集时，返回nil
     * @param[in]    key 集合键
     * @param[out]   value 集合随机返回元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool spop(const QString &key, QByteArray &value);

    /**
     * 将member元素从source集合移动到destination集合
     * @param[in]    source 源集合键
     * @param[in]    destination 目的集合键
     * @param[in]    member 集合元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool smove(const QString &source, const QString &destination, const QString &member);

    /**
     * 判断member元素是否集合key的成员
     * @param[in]    key 集合键
     * @param[in]    member 集合元素
     * @return       是true,否false
     * @see
     * @note
     */
    bool sismember(const QString &key, const QString &member);

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
    bool setrange(const QString &key, int offset, const QString &value, qlonglong & llRet);

    /**
     * 将结果保存到destination集合，如果destkey集合已经存在，则将其覆盖。
     * @param[in]    destkey 目的集合键
     * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
     * @param[out]   llRet 结果集中的元素数量
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool sdiffstore(const QString &destkey, const QString &keys, qlonglong & llRet);

    /**
     * 将给定集合的并集存储在指定的集合destination中。如果destination已经存在，则将其覆盖。
     * @param[in]    destkey 目的集合键
     * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
     * @param[out]   llRet 结果集中的元素数量
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool sunionstore(const QString &destination, const QString &keys, qlonglong & llRet);

    /**
     * 将给定集合之间的交集存储在指定的destkey集合中。如果指定的destkey集合已经存在，则将其覆盖
     * @param[in]    destkey 目的集合键
     * @param[in]    keys 要加入目的集合的集合key,多个可空格分割
     * @param[out]   llRet 结果集中的元素数量
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool sinterstore(const QString &destination, const QString &keys, qlonglong & llRet);

    /**
     * 移除集合key中的一个或多个member元素，不存在的member元素会被忽略
     * @param[in]    key 集合键
     * @param[in]    member 移除的多个元素,多个可空格分割
     * @param[out]   llRet 被成功移除的元素的数量，不包括被忽略的元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool srem(const QString &key, const QString &member, qlonglong & llRet);

    /**
     * 移除有序集合key中的一个或多个member元素，不存在的member元素会被忽略
     * @param[in]    key 集合键
     * @param[in]    member 移除的多个元素,多个可空格分割
     * @param[out]   llRet 被成功移除的元素的数量，不包括被忽略的元素
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool zrem(const QString &key, const QString &member, qlonglong & llRet);

    /**
     * 清除所有Lua脚本缓存
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool scriptflush();

    /**
     * 杀死当前正在运行的Lua脚本，当且仅当这个脚本没有执行过任何写操作时，这个命令才生效。
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool scriptkill();

    /**
     * 将脚本 script 添加到脚本缓存中，但并不立即执行这个脚本
     * 如果给定的脚本已经在缓存里面了，那么不做动作
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool scriptload(const QString &script);

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
    bool migrate(const QString & host, int port, const QString &key, int database,
                 int timeout, bool copy = false, bool replcace = false);

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
    bool blpop(const QString &key, QString & outkey, QByteArray &value, int timeout = 0);

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
    bool brpop(const QString & key, QString & outkey, QByteArray &value, int timeout =0);

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
    bool brpoplpush(const QString &source, const QString &destination,
                    QByteArray &value, int timeout=0);

    /**
     * 返回哈希表key中，所有的域和值
     * @param[in]    key 哈希键
     * @param[out]   vKeyPairs 哈希域值对
     * @param[out]   llRet 哈希域数量
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hgetall(const QString &key, QMap<QString, QString> &vKeyPairs, int &iRet);

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
    bool hmget(const QString &key, const QString &field, QList<QByteArray> &value);

    /**
     * 返回哈希表key中所有域的值。
     * @param[in]    key 哈希键
     * @param[out]   value 哈希域值
     * @param[out]   lRet 域个数
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hvals(const QString &key, QList<QByteArray> &value, int &_iRet);

    /**
     * 返回一个或多个给定key值。
     * 如果给定的键不存在，那么返回一个nil值。
     * @param[in]    key 一个或多个键，空格分割
     * @param[out]   value 键值
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool mget(const QString &key, QList<QByteArray> &value);

    /**
     * 返回集合key中的所有成员。不存在的key被视为空集合。
     * @param[in]    key 集合键
     * @param[out]   value 集合元素值
     * @param[out]   llRet 集合个数
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool smembers(const QString & key, QList<QByteArray> &value, int &iRet);

    /**
     * 给定一个或多个脚本的SHA1校验和，返回一个包含0和1的列表，表示校验和所指定的脚本是否已经被保存在缓存当中
     * @param[in]    script 一个或多个SHA1校验和，空格分割
     * @param[out]   value 结果列表
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool scriptexists(const QString &script, QList<QByteArray> &value);

    /**
     * 返回哈希表 key 中的所有域
     * @param[in]    key 哈希键
     * @param[out]   value 结果列表
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool hkeys(const QString &key, QList<QByteArray> &value);

    /**
     * 查找所有符合给定模式 pattern 的 key
     * @param[in]    pattern 匹配键模式
     * @param[out]   value 结果列表
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool keys(const QString & pattern, QList<QByteArray> &value);

    /**
     * 返回列表key中指定区间内的元素，包括start和stop在内
     * 参数 start和stop都以0表示列表的第一个元素，以1表示列表的第二个元素，以此类推
     * 也可以使用负数下标，以-1表示列表的最后一个元素，-2表示列表的倒数第二个元素，以此类推
     * @param[in]    key 链表键
     * @param[in]    start 链表开始位置
     * @param[in]    stop 链表结束位置
     * @param[out]   value 链表值
     * @return       成功rue,失败false
     * @see
     * @note        如果start下标比列表的最大下标end还要大，那么LRANGE返回一个空列表
     *              如果stop下标比end下标还要大，Redis将stop的值设置为end
     */
    bool lrange(const QString &key, int start, int stop, QList<QByteArray> &value);

    /**
     * 返回一个集合的全部成员，该集合是所有给定集合之间的差集
     * 不存在的key被视为空集
     * @param[in]    keys 一个或多个集合
     * @param[out]   value 集合差集
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool sdiff(const QString &keys, QList<QByteArray> &value);

    /**
     * 返回一个集合的全部成员，该集合是所有给定集合之间的交集
     * 不存在的key被视为空集
     * @param[in]    keys 一个或多个集合
     * @param[out]   value 集合交集
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool sinter(const QString &keys, QList<QByteArray> &value);

    /**
     * 返回一个集合的全部成员，该集合是所有给定集合之间的并集
     * 不存在的key被视为空集
     * @param[in]    keys 一个或多个集合
     * @param[out]   value 集合并集
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool sunion(const QString &keys, QList<QByteArray> &value);

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
    bool srandmember(const QString &key, QList<QByteArray> &value, int count = 0);

    /**
     * 返回或保存给定列表、集合、有序集合key中经过排序的元素
     * @param[in]    key 键
     * @param[in]    conditions 排序条件
     * @param[out]   value 结果集
     * @return       成功true,失败false
     * @see
     * @note         排序默认以数字作为对象，值被解释为双精度浮点数，然后进行比较
     */
    bool sort(const QString &key, QList<QByteArray> &value, const QString &conditions = "");

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
    bool scan(const QString &pattern, RespType &value, QString cursor = "0", qulonglong count = 2000);

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
    bool sscan(const QString& key, const QString &pattern, RespType &value, QString cursor = "0", qulonglong count = 2000);

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
    bool zscan(const QString& key, const QString &pattern, RespType &value, QString cursor = "0", qulonglong count = 2000);

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
    bool hscan(const QString& key, const QString &pattern, RespType &value, QString cursor = "0", qulonglong count = 2000);

    /**
     * 迭代希键中的键
     * @param[in]    parameter     config get命令参数匹配模式
     * @param[out]   value         扫描结果
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool getcfg(const QString& parameter, RespType &value);

    /**
     * 获取引用计数
     * @param[in]    key     键
     * @param[out]   value   结果
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool refcount(const QString& key, qlonglong & value);

    /**
     * 获取空闲时间
     * @param[in]    key     键
     * @param[out]   value   空闲秒
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool idletime(const QString& key, qlonglong & value);

    /**
     * 获取编码
     * @param[in]    key     键
     * @param[out]   value   结果
     * @return       成功true,失败false
     * @see
     * @note
     */
    bool encoding(const QString& key, QByteArray & value);

    /**
     * 选择db
     * @param[in]    index db索引
     * @return       成功rue,失败false
     * @see
     * @note
     */
    bool select(const int &index);

private:

    /**
     * 临时返回值变量
     */
    qlonglong _llRet;

    /**
     * 临时返回值变量
     */
    int _iRet;

    /**
     * 临时返回值变量
     */
    QByteArray _sValue;

    /**
     * 临时返回值变量
     */
    QList<QByteArray> _vListResult;

    /**
     * 命令变量
     */
    QList<QString> _vCmdList;

    /**
     * 临时返回值变量
     */
    bool _bRet;

    /**
     * 错误信息
     */
    QString _sErrorInfo;

    /**
     * 是否超时变量
     */
    bool _bTimeout;

    /**
     * 超时时间
     */
    int _iTimeOut;

    /**
     * 发送到REDIS服务端命令
     */
    QString _cmd;

    /**
     * 发送到REDIS服务端RESP消息
     */
    QByteArray _sendData;

    /**
     * 命令运行后REDIS服务端返回RESP消息
     */
    QByteArray _cmdResult;

    /**
     * REDIS服务端返回RESP原始消息
     */
    QByteArray _msg;

    /**
     * REDIS服务端返回RESP原始消息长度
     */
    int _msgLength;

    /**
     * REDIS通信TCP传输
     */
    RedisTransMgr *_redisTransMgr;

    /**
     * 事超时件循环机制
     */
    QEventLoop _qEventloop;

    /**
     * 定时器
     */
    QTimer _qTimer;

    /**
     * Resp消息
     */
    RespType _respMsg;

    /**
     * 是否订阅模式
     */
    bool _bSubModel;

private:
    void initClient();
    int  GetDataType(QByteArray &type);

signals:
    void sigConnected();
    void sigDisconnected();
    void sigError(const QString &sError);
    void sigMessage(const RespType &msg);
};

#endif // REDISCLIENT_H
