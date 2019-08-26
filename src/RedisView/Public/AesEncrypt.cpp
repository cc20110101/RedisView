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
        "@D@6`Ht`T`xPXD|6T``TxDPd|@`7x@p1PPHPXTXX@Lld7D|XdHhdgL3dl|XtL"
        "DpdhDdl3h|d|TLL|`lHH3xlTpL@|lH`TPxdh@Ptlh0tpHhtdxDtllhPp@h@xP"
        "`l|t`4DtDLht`|`Tdt|tDPDTxtHTHp|lD9t@l|h`LtTl`hx3Ll|P`DXDhtXD|"
        "ht@dXx|pxdH1t7|`PlP@HH4HP|@xd|LphllXpLlh32xp@3DhXtxtx@dPTtTXH"
        "|x@lTtDTt@Tt@TtpPLPxtX3hX`X`Lhxx1l|Hp@pHdhxTLldpdp|DHxDXpHxLP"
        "l|@x3PLhpXHT`t@TXldT5TPXPLt@p@@xDtxhXtXHxxT9pDlxH3l@tx|XpDlDX"
        "3t|`txH|8X3hLh`|hdhTHPlDH|T|p@@|`6dhpTxDtPt@||`9X|@@hDptT|pHL"
        "hdXhhdTDxHphllX`0xxLd|dDHxXp|LLdplT|HDX@PhpP`xhXP|lptX`D`@`1d"
        "hDD3p@X@lhdxPXPDxLDhdlxpXdDLTppxD`|ff0e92h3ddv45245f4vfdhgrar";

QByteArray AesEncrypt::_hashKey = QCryptographicHash::hash(_key.toLocal8Bit(), QCryptographicHash::Sha256);

QByteArray AesEncrypt::_hashIV = QCryptographicHash::hash(_key.toLocal8Bit(), QCryptographicHash::Md5);


