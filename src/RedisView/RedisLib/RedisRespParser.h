/**
* @file      RedisRespParser.h
* @brief     REDIS客户端RESP解析类
* @author    wangcc3
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#ifndef REDISRESPPARSER_H
#define REDISRESPPARSER_H

#include "RedisLib/RedisDefine.h"

/**
 * Redis内存数据库RESP协议解析打包类
 */
class RedisRespParser
{

public:

    RedisRespParser();

    // 打包成RESP协议格式消息
    bool packRespCmd(const QByteArray &sInCmd, QByteArray &sOutRespCmd);
    // 打包成RESP协议格式消息
    bool packRespCmd(const QList<QString> &vInCmd, QByteArray &sOutRespCmd);
    // 打包成RESP协议格式消息
    bool packRespCmd(const QString &sInCmd, QByteArray &sOutRespCmd);
    // 解析RESP协议单数组
    bool parseRespSingelArray(const QByteArray &data, QList<QByteArray> &vResult, int &iResult);
    // 解析RESP协议数组
    bool parseRespArray(const QByteArray &data, QList<RespType> &vResult, int &iResult);
    // 解析RESP协议数组长度
    bool parseRespArrayLength(const QByteArray &data, int &pos);
    // 解析RESP协议整形
    bool parseRespInteger(const QByteArray &data, qlonglong &llResult);
    // 解析RESP协议浮点型
    bool parseRespDouble(const QByteArray &data, double &dResult);
    // 解析RESP协议字符串
    bool parseRespString(const QByteArray &data, QByteArray &sResult);
    // 解析RESP协议错误
    bool parseRespError(const QByteArray &data, QByteArray &sResult);
    // 解析RESP协议大字符串
    bool parseRespBulkString(const QByteArray &data, QString &sResult, int &iResult);
    // 解析RESP协议大字符串
    bool parseRespBulkString(const QByteArray &data, QByteArray &sResult, int &iResult);
    // 解析RESP协议
    bool parseResp(const QByteArray &data, RespType &rResult);
    // 解析msg消息是否是完整RESP
    bool isValidResp(QByteArray &msg, int &ipos);
    // 从TCP获取消息后,解析msg消息是否是完整RESP
    bool getTcpResp(QByteArray &msg, int &ipos);
    // 获取错误信息
    QString getParseInfo() const;
    // 格式化RESP类型转RESP协议并格式化
    void formatToResp(const RespType &inResp, QByteArray &outResp, int spaceNum);
    // 格式化RESP类型转RESP协议并格式化为文本
    void formatToText(const RespType &inResp, QByteArray &outResp, int spaceNum);
    // 格式化RESP类型为JSON格式
    void formatToJson(const RespType &inResp, QJsonArray &outJson);

private:

    QString _sErrorInfo;
    QByteArray _strCmd;
    QByteArray _strBuffer;
    QList<QByteArray> _strList;

};


#endif // REDISRESPPARSER_H
