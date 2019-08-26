/**
* @file      Define.cpp
* @brief     公共定义
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Public/Define.h"

QMutex  G_DB_MUTEX;
QMutex  G_SEQUENCE_MUTEX;
QMutex  G_PUBLIC_LIB_MUTEX;

Global::Global() {
}
Global::~Global() {
}

QString Global::gEncode = "";
QString Global::gConnectName = "";
QString Global::gTheme = "";
QTranslator * Global::gTrans = nullptr;

