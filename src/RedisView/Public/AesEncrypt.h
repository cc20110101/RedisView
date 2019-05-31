/**
* @file      AesEncrypt.h
* @brief     AES加解密工具
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef AESENCRYPT_H
#define AESENCRYPT_H

#include "Public/QaesEncryption.h"

class AesEncrypt
{
public:
    static QByteArray CBC256Crypt(QString inputStr);
    static QString CBC256Decrypt(QByteArray inputStr);

private:
    static QByteArray _hashKey;
    static QByteArray _hashIV;
    static QString _key;
};

#endif // AESENCRYPT_H
