/**
* @file      Log.h
* @brief     公共日志库
* @author    王长春
* @date      2022-08-25
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef LOG_H
#define LOG_H

#include "Publib.h"

class Log
{
public:
    static void Trace(QString info);
    static void Debug(QString info);
    static void Info(QString info);
    static void Warn(QString info);
    static void Error(QString info);
    static void Fatal(QString info);
};

#endif // LOG_H
