/**
* @file      Log.cpp
* @brief     公共日志库
* @author    王长春
* @date      2022-08-25
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Log.h"


void Log::Trace(QString info){
    QString message = QString("[%1] [TRACE]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}

void Log::Debug(QString info){
    QString message = QString("[%1] [DEBUG]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}

void Log::Info(QString info){
    QString message = QString("[%1] [INFO]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}

void Log::Warn(QString info){
    QString message = QString("[%1] [WARN]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}

void Log::Error(QString info){
    QString message = QString("[%1] [ERROR]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}

void Log::Fatal(QString info){
    QString message = QString("[%1] [FATAL]: %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    PubLib::log(message);
}
