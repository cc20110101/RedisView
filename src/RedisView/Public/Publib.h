#ifndef PUBLIB_H
#define PUBLIB_H

#include "Public/Define.h"

class PubLib
{
public:
    static void getList(const QString & str, QList<QString> & strList);
    static void setConfig(const QString & key, const QString & value);
    static QString getConfig(const QString & key, const QString &defaultValue = "");

};

#endif // PUBLIB_H
