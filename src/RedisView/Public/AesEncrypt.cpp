/**
* @file      AesEncrypt.cpp
* @brief     AES加解密工具
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AesEncrypt.h"

QByteArray AesEncrypt::CBC256Crypt(QString inputStr) {
    QaesEncryption encryption(QaesEncryption::AES_256, QaesEncryption::CBC);
    return encryption.encode(inputStr.toLocal8Bit(),
                             _hashKey, _hashIV);
}

QString AesEncrypt::CBC256Decrypt(QByteArray inputStr) {
    QaesEncryption encryption(QaesEncryption::AES_256, QaesEncryption::CBC);
    return QString(encryption.removePadding(
                       encryption.decode(inputStr, _hashKey, _hashIV)));
}

QString AesEncrypt::_key =
        "@D@6`Ht`T`xPXD|X@Lld7D|XdHh6T``TxDPd|@`7x@p1PPHPXTXdgL3dl|XtL"
        "DpdhDdpHhtdxDtllTLL|`lHH3xlTpL@|lH`TPxdhl3h|d|Ptlh0t@hPp@h@xP"
        "`l|t`4Dt|tDP3Ll|P`DXDTxtHTHp|lD9t@l|h`LtTl`hxtDLht`|`TdDhtXD|"
        "ht@dXxlXpL|pxd4HP|@xd|Lphllh32xp@3DhXtxtx@dH1t7|`PlP@HHPTtTXH"
        "|x@lTtDxtXldpdp|DHxTt@Tt@TtpPLPDXp3hX`X`Lhxx1l|Hp@pHdhxTLHxLP"
        "l|@x3PLTXldT5TPXPhpXHT`t@Lt@p@@xHxxT9pDlxH3l@tx|XpDtxhXtXDlDX"
        "3t|`@@|`6dhptxH|8X3hLh`|hlDH|T|pTxDtPt@||`9X|@@dhTHPhDptT|pHL"
        "hdXDHxXp|LhhdTDxHphllX`0xxLd|dLdplT|HDX@PhtX`D`@pP`xhXP|lp`1d"
        "hX@lhDD3p@dxPXPlxpXdDLTDxLDhdppxD`|h3ddv45245ff0e92f4vfdhgrar";

QByteArray AesEncrypt::_hashKey = QCryptographicHash::hash(_key.toLocal8Bit(), QCryptographicHash::Sha256);

QByteArray AesEncrypt::_hashIV = QCryptographicHash::hash(_key.toLocal8Bit(), QCryptographicHash::Md5);


