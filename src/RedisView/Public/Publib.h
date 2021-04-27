/**
* @file      Publib.h
* @brief     公共函数库
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef PUBLIB_H
#define PUBLIB_H

#include "Public/Define.h"

class PubLib
{
public:
    static void getList(const QString & str, QList<QString> & strList);
    static void setConfig(const QString & key, const QString & value);
    static QString getConfig(const QString & key, const QString &defaultValue = "");
    static void setConfigB(const QString & key, const bool & value);
    static bool getConfigB(const QString & key, const bool &defaultValue = false);
    static void setSequenceId(int64_t sequenceId);
    static int getKeyType(const QByteArray & type);
    static int64_t getSequenceId();
    static void log(QString info);
    static QStringList getMacAddr();
    static QString getCpuId();
    static bool checkCode();
    static void saveCode(QString code);
    static QString getCode();
    static QString getSoftCode();

private:
    static void _getcpuid(unsigned int CPUInfo[4], unsigned int InfoType);
    static void _getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue);
    static int64_t _sequenceId;
};

#endif // PUBLIB_H
