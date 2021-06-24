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
//加密库crypto
//对称加密(单密钥加密):同一个密钥可以同时用来加密和解密,常用算法低级到高级DES 3DES AES
//非对称加密:公钥与私钥是一对，用公钥对数据进行加密，只有用对应的私钥才能解密,常用算法RSA
//哈希算法(散列算法):md5不是加密，是不可逆的哈希算法
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
